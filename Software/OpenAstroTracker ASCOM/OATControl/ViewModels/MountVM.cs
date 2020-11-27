using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;
using System.Xml.Linq;
using ASCOM.Utilities;
using OATCommunications;
using OATCommunications.Model;
using OATCommunications.WPF.CommunicationHandlers;
using OATCommunications.Utilities;

namespace OATControl.ViewModels
{
	public class MountVM : ViewModelBase
	{
		int _targetRAHour = 0;
		int _targetRAMinute = 0;
		int _targetRASecond = 0;
		int _targetDECDegree = 90;
		int _targetDECMinute = 0;
		int _targetDECSecond = 0;
		int _curRAHour = 0;
		int _curRAMinute = 0;
		int _curRASecond = 0;
		int _curDECDegree = 90;
		int _curDECMinute = 0;
		int _curDECSecond = 0;
		int _raStepper = 0;
		int _decStepper = 0;
		int _trkStepper = 0;
		int _raStepsPerDegree = 0;
		int _decStepsPerDegree = 0;
		bool _connected = false;
		bool _slewInProgress = false;
		bool _isTracking = false;
		bool _isGuiding = false;
		bool _isSlewingNorth = false;
		bool _isSlewingSouth = false;
		bool _isSlewingWest = false;
		bool _isSlewingEast = false;
		bool _driftAlignRunning = false;
		int _runningOATCommand = 0;
		SemaphoreSlim exclusiveAccess = new SemaphoreSlim(1, 1);
		string _driftAlignStatus = "Drift Alignment";
		float _driftPhase = 0;

		private float _maxMotorSpeed = 2.5f;
		double _speed = 1.0;
		string _scopeName = string.Empty;
		string _scopeHardware = string.Empty;
		string _mountStatus = string.Empty;
		string _currentHA = string.Empty;
		CultureInfo _oatCulture = new CultureInfo("en-US");
		Util _util;
		ASCOM.Astrometry.Transform.Transform _transform;
		bool _raIsNEMA;
		bool _decIsNEMA;
		List<string> _oatAddonStates = new List<string>();

		DelegateCommand _arrowCommand;
		DelegateCommand _connectScopeCommand;
		DelegateCommand _slewToTargetCommand;
		DelegateCommand _syncToTargetCommand;
		DelegateCommand _syncToCurrentCommand;
		DelegateCommand _startSlewingCommand;
		DelegateCommand _stopSlewingCommand;
		DelegateCommand _homeCommand;
		DelegateCommand _setHomeCommand;
		DelegateCommand _parkCommand;
		DelegateCommand _driftAlignCommand;
		DelegateCommand _polarAlignCommand;
		DelegateCommand _showLogFolderCommand;

		DispatcherTimer _timerStatus;
		DispatcherTimer _timerFineSlew;

		private ICommunicationHandler _commHandler;
		private OatmealTelescopeCommandHandlers _oatMount;
		private List<PointOfInterest> _pointsOfInterest;
		int _selectedPointOfInterest;
		private float _slewYSpeed;
		private float _slewXSpeed;
		private int _slewRate = 4;
		private object _speedUpdateLock = new object();
		private bool _updatedSpeeds;
		private bool _isCoarseSlewing = true;
		DateTime _startTime;
		private string _parkString = "Park";
		private TimeSpan _remainingRATime;
		private int _slewStartRA;
		private int _slewStartDEC;
		private int _slewTargetRA;
		private int _slewTargetDEC;

		// private float _trackingSpeed;

		public float RASpeed { get; private set; }
		public float DECSpeed { get; private set; }

		public MountVM()
		{
			Log.WriteLine("Mount: Initialization starting...");
			CommunicationHandlerFactory.DiscoverDevices();
			Log.WriteLine("Mount: Device discovery started...");

			_startTime = DateTime.UtcNow;
			_timerStatus = new DispatcherTimer(TimeSpan.FromMilliseconds(500), DispatcherPriority.Normal, async (s, e) => await OnTimer(s, e), Application.Current.Dispatcher);
			_timerFineSlew = new DispatcherTimer(TimeSpan.FromMilliseconds(200), DispatcherPriority.Normal, async (s, e) => await OnFineSlewTimer(s, e), Application.Current.Dispatcher);
			_arrowCommand = new DelegateCommand(s => OnAdjustTarget(s.ToString()));
			_connectScopeCommand = new DelegateCommand(() => OnConnectToTelescope());
			_slewToTargetCommand = new DelegateCommand(async () => await OnSlewToTarget(), () => MountConnected);
			_syncToTargetCommand = new DelegateCommand(async () => await OnSyncToTarget(), () => MountConnected);
			_syncToCurrentCommand = new DelegateCommand(() => OnSyncToCurrent(), () => MountConnected);
			_startSlewingCommand = new DelegateCommand(async s => await OnStartSlewing(s.ToString()), () => MountConnected);
			_stopSlewingCommand = new DelegateCommand(async () => await OnStopSlewing('a'), () => MountConnected);
			_homeCommand = new DelegateCommand(async () => await OnHome(), () => MountConnected);
			_setHomeCommand = new DelegateCommand(async () => await OnSetHome(), () => MountConnected);
			_parkCommand = new DelegateCommand(async () => await OnPark(), () => MountConnected);
			_driftAlignCommand = new DelegateCommand(async dur => await OnRunDriftAlignment(int.Parse(dur.ToString())), () => MountConnected);
			_polarAlignCommand = new DelegateCommand(() => OnRunPolarAlignment(), () => MountConnected);
			_showLogFolderCommand = new DelegateCommand(() => OnShowLogFolder(), () => true);

			_util = new Util();
			_transform = new ASCOM.Astrometry.Transform.Transform();

			var poiFile = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "PointsOfInterest.xml");
			Log.WriteLine("Mount: Attempting to read Point of Interest from {0}...", poiFile);
			if (File.Exists(poiFile))
			{
				XDocument doc = XDocument.Load(poiFile);
				_pointsOfInterest = doc.Element("PointsOfInterest").Elements("Object").Select(e => new PointOfInterest(e)).ToList();
				_pointsOfInterest.Sort((p1, p2) =>
				{
					if (p1.Name.StartsWith("Polaris")) return -1;
					if (p2.Name.StartsWith("Polaris")) return 1;
					return p1.Name.CompareTo(p2.Name);
				});
				_pointsOfInterest.Insert(0, new PointOfInterest("--- Select Target Object ---"));
				_selectedPointOfInterest = 0;
				Log.WriteLine("Mount: Successfully read {0} Points of Interest.", _pointsOfInterest.Count - 1);
			}

			this.Version = Assembly.GetExecutingAssembly().GetName().Version;
			Log.WriteLine("Mount: Initialization of OATControl {0} complete...", this.Version);
		}

		private void OnShowLogFolder()
		{
			ProcessStartInfo info = new ProcessStartInfo("explorer.exe", Path.GetDirectoryName(Log.Filename)) { UseShellExecute = true };
			Process.Start(info);
		}

		private async Task OnSetHome()
		{
			Log.WriteLine("Mount: Setting Home...");
			await RunCustomOATCommandAsync(":SHP#,n");
			await ReadHA();
		}

		private async Task ReadHA()
		{
			Log.WriteLine("Mount: Reading HA");
			for (int i = 0; i < 20; i++)
			{
				var ha = await RunCustomOATCommandAsync(":XGH#,#");
				if (!string.IsNullOrEmpty(ha))
				{
					CurrentHA = String.Format("{0}h {1}m {2}s", ha.Substring(0, 2), ha.Substring(2, 2), ha.Substring(4, 2));
					break;
				}
				await Task.Delay(50);
			}
		}

		async private Task<string> RunCustomOATCommandAsync(string command)
		{
			string result = string.Empty;
			//Log.WriteLine("OATCustom:  StartRunning [{0}], checking access", command);
			if (Interlocked.CompareExchange(ref _runningOATCommand, 1, 0) == 0)
			{
				try
				{
					await exclusiveAccess.WaitAsync();
					//Log.WriteLine("OATCustom:  Access granted. -> Sending {0} ", command);
					var commandResult = await _oatMount.SendCommand(command);
					if (!string.IsNullOrEmpty(commandResult.Data))
					{
						//Log.WriteLine("OATCustom: Result [{0}]: '{1}'", command, commandResult.Data);
						result = commandResult.Data;
					}
					Interlocked.Decrement(ref _runningOATCommand);
				}
				finally
				{
					//Log.WriteLine("OATCustom:  StopRunning [{0}]", command);
					exclusiveAccess.Release();
				}
			}
			else
			{
				await Task.Run(async () =>
				{
					//Log.WriteLine("OATCustom2:  Await access for {0} ", command);
					while (Interlocked.CompareExchange(ref _runningOATCommand, 1, 0) != 0)
					{
						Thread.Sleep(1);
					}
					try
					{
						await exclusiveAccess.WaitAsync();
						//Log.WriteLine("OATCustom2:  Access granted. -> Sending {0} ", command);
						var commandResult = await _oatMount.SendCommand(command);
						if (!string.IsNullOrEmpty(commandResult.Data))
						{
							//Log.WriteLine("OATCustom2: Result [{0}]: '{1}'", command, commandResult.Data);
							result = commandResult.Data;
						}
						Interlocked.Decrement(ref _runningOATCommand);
					}
					finally
					{
						//Log.WriteLine("OATCustom2:  StopRunning [{0}]", command);
						exclusiveAccess.Release();
					}

				});

				//Log.WriteLine("OATCustom2:  Completed '{0}'", command);
			}

			//Log.WriteLine("OATCustom: Returning '{0}'", result);
			return result.TrimEnd("#".ToCharArray());
		}

		private async Task OnTimer(object s, EventArgs e)
		{
			_timerStatus.Stop();
			if (MountConnected)
			{
				await UpdateStatus();
			}

			_timerStatus.Start();
		}

		long gxRequest = 1;
		private async Task UpdateStatus()
		{
			if (MountConnected)
			{
				//   0   1  2 3 4    5      6
				// Idle,--T,0,0,31,080300,+900000,#
				string status;

				status = await RunCustomOATCommandAsync(":GX#,#");

				if (!string.IsNullOrWhiteSpace(status))
				{
					try
					{
						var parts = status.Split(",".ToCharArray());
						string prevStatus = MountStatus;
						MountStatus = parts[0];
						if ((MountStatus == "SlewToTarget") && (prevStatus != "SlewToTarget"))
						{
							var targetRa = await RunCustomOATCommandAsync(":Gr#,#");
							var targetDec = await RunCustomOATCommandAsync(":Gd#,#");
							_slewStartRA = (CurrentRAHour * 60 + CurrentRAMinute) * 60 + CurrentRASecond;
							_slewStartDEC = (CurrentDECDegree * 60 + CurrentDECMinute) * 60 + CurrentDECSecond;

							TargetRAHour = int.Parse(targetRa.Substring(0, 2));
							TargetRAMinute = int.Parse(targetRa.Substring(3, 2));
							TargetRASecond = int.Parse(targetRa.Substring(6, 2));

							TargetDECDegree = int.Parse(targetDec.Substring(0, 3));
							TargetDECMinute = int.Parse(targetDec.Substring(4, 2));
							TargetDECSecond = int.Parse(targetDec.Substring(7, 2));

							_slewTargetRA = (TargetRAHour * 60 + TargetRAMinute) * 60 + TargetRASecond;
							_slewTargetDEC = (TargetDECDegree * 60 + TargetDECMinute) * 60 + TargetDECSecond;

							OnPropertyChanged("RASlewProgress");
							OnPropertyChanged("DECSlewProgress");

							DisplaySlewProgress = true;
						}
						else if ((MountStatus != "SlewToTarget") && (prevStatus == "SlewToTarget"))
						{
							DisplaySlewProgress = false;
						}
						else if (MountStatus == "SlewToTarget")
						{
							OnPropertyChanged("RASlewProgress");
							OnPropertyChanged("DECSlewProgress");
						}


						switch (parts[1][0])
						{
							case 'R': IsSlewingEast = true; IsSlewingWest = false; break;
							case 'r': IsSlewingEast = false; IsSlewingWest = true; break;
							default: IsSlewingEast = false; IsSlewingWest = false; break;
						}
						switch (parts[1][1])
						{
							case 'd': IsSlewingNorth = true; IsSlewingSouth = false; break;
							case 'D': IsSlewingNorth = false; IsSlewingSouth = true; break;
							default: IsSlewingNorth = false; IsSlewingSouth = false; break;
						}

						// Don't use property here since it sends a command.
						_isTracking = parts[1][2] == 'T';
						OnPropertyChanged("IsTracking");


						RAStepper = int.Parse(parts[2]);
						DECStepper = int.Parse(parts[3]);
						TrkStepper = int.Parse(parts[4]);

						CurrentRAHour = int.Parse(parts[5].Substring(0, 2));
						CurrentRAMinute = int.Parse(parts[5].Substring(2, 2));
						CurrentRASecond = int.Parse(parts[5].Substring(4, 2));

						CurrentDECDegree = int.Parse(parts[6].Substring(0, 3));
						CurrentDECMinute = int.Parse(parts[6].Substring(3, 2));
						CurrentDECSecond = int.Parse(parts[6].Substring(5, 2));
					}
					catch (Exception ex)
					{
						Log.WriteLine("UpdateStatus: Failed to process GX reply [{0}]", status);
					}
				}
			}
		}

		public async Task<string> SetSiteLatitude(float latitude)
		{
			return await _oatMount.SetSiteLatitude(latitude);
		}

		public async Task<string> SetSiteLongitude(float longitude)
		{
			return await _oatMount.SetSiteLongitude(longitude);
		}

		private async Task OnHome()
		{
			Log.WriteLine("Mount: Home requested");
			await RunCustomOATCommandAsync(":hF#");

			// The next call actually blocks because Homeing is synchronous
			await UpdateStatus();

			await ReadHA();
		}

		private async Task OnPark()
		{
			if (ParkCommandString == "Park")
			{
				Log.WriteLine("Mount: Parking requested");
				await RunCustomOATCommandAsync(":hP#");
				ParkCommandString = "Unpark";
			}
			else
			{
				Log.WriteLine("Mount: Unparking requested");
				await RunCustomOATCommandAsync(":hU#,n");
				ParkCommandString = "Park";
			}

			// The next call actually blocks because Homeing is synchronous
			await UpdateStatus();
		}

		private bool IsSlewing(char dir)
		{
			if (dir == 'n') return IsSlewingNorth;
			if (dir == 'e') return IsSlewingEast;
			if (dir == 'w') return IsSlewingWest;
			if (dir == 's') return IsSlewingSouth;
			if (dir == 'a') return IsSlewingNorth | IsSlewingSouth | IsSlewingWest | IsSlewingEast;
			return false;
		}

		private async Task OnStopSlewing(char dir)
		{
			await RunCustomOATCommandAsync(string.Format(":Q{0}#", dir));
		}

		private async Task OnStartSlewing(char dir)
		{
			await RunCustomOATCommandAsync(string.Format(":M{0}#", dir));
		}

		private async Task OnStartSlewing(string direction)
		{
			bool turnOn = direction[0] == '+';
			char dir = char.ToLower(direction[1]);
			if (turnOn)
			{
				await OnStartSlewing(dir);
			}
			else
			{
				await OnStopSlewing(dir);
			}
		}

		private async Task OnSlewToTarget()
		{
			await _oatMount.Slew(new TelescopePosition(1.0 * TargetRASecond / 3600.0 + 1.0 * TargetRAMinute / 60.0 + TargetRAHour, 1.0 * TargetDECSecond / 3600.0 + 1.0 * TargetDECMinute / 60.0 + TargetDECDegree, Epoch.JNOW));
		}

		private async Task OnSyncToTarget()
		{
			await _oatMount.Sync(new TelescopePosition(1.0 * TargetRASecond / 3600.0 + 1.0 * TargetRAMinute / 60.0 + TargetRAHour, 1.0 * TargetDECSecond / 3600.0 + 1.0 * TargetDECMinute / 60.0 + TargetDECDegree, Epoch.JNOW));
		}

		private void OnSyncToCurrent()
		{
			TargetRAHour = CurrentRAHour;
			TargetRAMinute = CurrentRAMinute;
			TargetRASecond = CurrentRASecond;
			TargetDECDegree = CurrentDECDegree;
			TargetDECMinute = CurrentDECMinute;
			TargetDECSecond = CurrentDECSecond;
		}

		private void FloatToHMS(double val, out int h, out int m, out int s)
		{
			h = (int)Math.Floor(val);
			val = (val - h) * 60;
			m = (int)Math.Floor(val);
			val = (val - m) * 60;
			s = (int)Math.Round(val);
		}

		private async Task UpdateCurrentCoordinates()
		{
			int h, m, s;
			var pos = await _oatMount.GetPosition();
			FloatToHMS(pos.Declination, out h, out m, out s);
			CurrentDECDegree = h;
			CurrentDECMinute = m;
			CurrentDECSecond = s;

			FloatToHMS(pos.RightAscension, out h, out m, out s);
			CurrentRAHour = h;
			CurrentRAMinute = m;
			CurrentRASecond = s;
		}

		private async void OnConnectToTelescope()
		{
			if (MountConnected)
			{
				MountConnected = false;
				_commHandler.Disconnect();
				ScopeName = string.Empty;
				ScopeHardware = string.Empty;
				_oatMount = null;
				_commHandler = null;
				RequeryCommands();
			}
			else
			{
				RequeryCommands();

				Log.WriteLine("Mount: Connect to OAT requested");

				if (this.ChooseTelescope())
				{
					try
					{
						_transform.SiteElevation = 0; //  _oatMount.SiteElevation;
						Log.WriteLine("Mount: Getting OAT Latitude");
						_transform.SiteLatitude = await _oatMount.GetSiteLatitude();
						Log.WriteLine("Mount: Received Latitude {0}. Getting OAT Longitude...", _util.HoursToHMS(_transform.SiteLatitude, ":", ":", ":"));
						_transform.SiteLongitude = await _oatMount.GetSiteLongitude();
						Log.WriteLine("Mount: Received Longitude {0}.", _util.HoursToHMS(_transform.SiteLongitude, ":", ":", ":"));
						_transform.SetAzimuthElevation(0, 90);
						var lst = _transform.RATopocentric;
						var lstS = _util.HoursToHMS(lst, "", "", "");

						Log.WriteLine("Mount: Current LST is {0}. Sending to OAT.", _util.HoursToHMS(lst, "h", "m", "s"));
						var stringResult = await RunCustomOATCommandAsync(string.Format(":SHL{0}#,n", _util.HoursToHMS(lst, "", "", "")));

						Log.WriteLine("Mount: Getting current OAT position");
						await UpdateCurrentCoordinates();
						TargetDECDegree = CurrentDECDegree;
						TargetDECMinute = CurrentDECMinute;
						TargetDECSecond = CurrentDECSecond;

						TargetRAHour = CurrentRAHour;
						TargetRAMinute = CurrentRAMinute;
						TargetRASecond = CurrentRASecond;
						Log.WriteLine("Mount: Current OAT position is RA: {0:00}:{1:00}:{2:00} and DEC: {3:000}*{4:00}'{5:00}", CurrentRAHour, CurrentRAMinute, CurrentRASecond, CurrentDECDegree, CurrentDECMinute, CurrentDECSecond);

						Log.WriteLine("Mount: Getting current OAT RA steps/degree...");
						string steps = await RunCustomOATCommandAsync(string.Format(":XGR#,#"));
						Log.WriteLine("Mount: Current RA steps/degree is {0}. Getting current DEC steps/degree...", steps);
						_raStepsPerDegree = int.Parse(steps);
						steps = await RunCustomOATCommandAsync(string.Format(":XGD#,#"));
						Log.WriteLine("Mount: Current DEC steps/degree is {0}. Getting current Speed factor...", steps);
						_decStepsPerDegree = int.Parse(steps);
						OnPropertyChanged("RAStepsPerDegree");
						OnPropertyChanged("DECStepsPerDegree");

						steps = await RunCustomOATCommandAsync(string.Format(":XGS#,#"));
						Log.WriteLine("Mount: Current Speed factor is {0}...", steps);
						SpeedCalibrationFactor = float.Parse(steps, _oatCulture);

						//Log.WriteLine("Mount: Get OAT Tracking speed...");
						//TrackingSpeed = await GetTrackingSpeed();
						//Log.WriteLine("Mount: Tracking speed is {0:0.0000}.", TrackingSpeed);

						Log.WriteLine("Mount: Reading Current OAT HA...");
						await ReadHA();
						MountConnected = true;
						Log.WriteLine("Mount: Successfully connected and configured!");
					}
					catch (FormatException fex)
					{
						ScopeName = string.Empty;
						ScopeHardware = string.Empty;
						Log.WriteLine("Mount: Failed to connect and configure OAT! {0}", fex.Message);
						MessageBox.Show("Connected to OpenAstroTracker, but protocol could not be established.\n\nIs the firmware compiled with DEBUG_LEVEL set to DEBUG_NONE?", "Protocol Error", MessageBoxButton.OK, MessageBoxImage.Error);
					}
					catch (Exception ex)
					{
						ScopeName = string.Empty;
						ScopeHardware = string.Empty;
						Log.WriteLine("Mount: Failed to connect and configure OAT! {0}", ex.Message);
						MessageBox.Show("Error trying to connect to OpenAstroTracker.\n\n" + ex.Message, "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
					}
					finally
					{
						//exclusiveAccess.Release();
					}
				}
			}
		}

		private void OnRunPolarAlignment()
		{
			Log.WriteLine("Mount: Running Polar Alignment Wizard");
			DlgRunPolarAlignment dlg = new DlgRunPolarAlignment(this.RunCustomOATCommandAsync) { Owner = Application.Current.MainWindow, WindowStartupLocation = WindowStartupLocation.CenterOwner }; ;
			dlg.ShowDialog();
			Log.WriteLine("Mount: Polar Alignment Wizard completed");
		}

		private async Task OnRunDriftAlignment(int driftDuration)
		{
			_timerStatus.Stop();

			Log.WriteLine("Mount: Running Drift Alignment");

			DriftAlignStatus = "Drift Alignment running...";
			var tracking = await RunCustomOATCommandAsync(":GIT#,#");
			bool wasTracking = tracking == "1";
			IsTracking = false;
			DateTime startTime = DateTime.UtcNow;
			TimeSpan duration = TimeSpan.FromSeconds(2 * driftDuration + 2);
			await Task.Delay(200);

			try
			{
				await RunCustomOATCommandAsync(string.Format(":XD{0:000}#", driftDuration));
				IsDriftAligning = true;
				RequeryCommands();
			}
			finally
			{
				await Task.Run(() =>
				{
					DateTime endTime = startTime + duration;
					while (DateTime.UtcNow < endTime)
					{
						Thread.Sleep(50);
						float driftPhase = (float)((DateTime.UtcNow - startTime).TotalSeconds / duration.TotalSeconds);
						DriftPhase = driftPhase;
					}
				});
			}

			DriftAlignStatus = "Drift Alignment";
			IsDriftAligning = false;
			IsTracking = wasTracking;
			RequeryCommands();
			_timerStatus.Start();
			Log.WriteLine("Mount: Completed Drift Alignment");
		}

		private void RequeryCommands()
		{
			_connectScopeCommand.Requery();
			_slewToTargetCommand.Requery();
			_syncToTargetCommand.Requery();
			_syncToCurrentCommand.Requery();
			_startSlewingCommand.Requery();
			_stopSlewingCommand.Requery();
			_homeCommand.Requery();
			_setHomeCommand.Requery();
			_parkCommand.Requery();
			_driftAlignCommand.Requery();
			_polarAlignCommand.Requery();
			_showLogFolderCommand.Requery();

			OnPropertyChanged("ConnectCommandString");
		}

		public async Task<bool> ConnectToOat(string device)
		{
			_commHandler = CommunicationHandlerFactory.ConnectToDevice(device);
			_oatMount = new OatmealTelescopeCommandHandlers(_commHandler);

			_oatAddonStates.Clear();

			Log.WriteLine("Mount: Request OAT Firmware version");
			var result = await _oatMount.SendCommand("GVP#,#");
			if (!result.Success)
			{
				Log.WriteLine("Mount: Unable to communicate with OAT. {0}", result.StatusMessage);
				return false;
			}

			Log.WriteLine("Mount: Connected to OAT. Requesting firmware version..");
			var resultNr = await _oatMount.SendCommand("GVN#,#");
			ScopeName = $"{result.Data} {resultNr.Data}";

			var hardware = await _oatMount.SendCommand("XGM#,#");
			Log.WriteLine("Mount: Hardware is {0}", hardware);
			var hwParts = hardware.Data.Split(',');
			var raParts = hwParts[1].Split('|');
			var decParts = hwParts[2].Split('|');
			ScopeHardware = $"{hwParts[0]} board    RA {raParts[0]}, {raParts[1]}T    DEC {decParts[0]}, {decParts[1]}T";
			_raIsNEMA = raParts[0] == "NEMA";
			_decIsNEMA = decParts[0] == "NEMA";
			for (int i = 3; i < hwParts.Length; i++)
			{
				_oatAddonStates.Add(hwParts[i]);
			}
			return true;
		}

		public bool IsAddonSupported(string addon)
		{
			return _oatAddonStates.Contains(addon);
		}

		public IList<string> Addons
		{
			get
			{
				return _oatAddonStates;
			}
		}

		private bool ChooseTelescope()
		{
			var dlg = new DlgChooseOat(this, this.RunCustomOATCommandAsync) { Owner = Application.Current.MainWindow, WindowStartupLocation = WindowStartupLocation.CenterOwner };

			Log.WriteLine("Mount: Showing OAT comms Chooser Wizard");
			dlg.ShowDialog();

			if (dlg.Result == true)
			{
				Log.WriteLine("OAT Connected!");
				return true;
			}
			else if (dlg.Result == null)
			{
				Log.WriteLine("Mount: Unable to connect");
				string extraMessage = "Is something else connected?";
				if (Process.GetProcesses().FirstOrDefault(d => d.ProcessName.Contains("ASCOM.OpenAstroTracker")) != null)
				{
					extraMessage = "Another process is connected via ASCOM.";
				}
				MessageBox.Show("Cannot connect to mount. " + extraMessage, "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
				return false;
			}

			RequeryCommands();
			Log.WriteLine("Mount: Chooser cancelled");
			ScopeName = string.Empty;
			ScopeHardware = string.Empty;
			_oatAddonStates.Clear();
			if (_commHandler != null)
			{
				_commHandler.Disconnect();
			}

			_oatMount = null;
			_commHandler = null;

			return false;
		}

		// Adjust the given number by the given adjustment, wrap around the limits.
		// Limits are inclusive, so they represent the lowest and highest valid number.
		private int AdjustWrap(int current, int adjustBy, int minVal, int maxVal)
		{
			current += adjustBy;
			if (current > maxVal) current -= (maxVal + 1 - minVal);
			if (current < minVal) current += (maxVal + 1 - minVal);
			return current;
		}

		// Adjust the given number by the given adjustment, clamping to the limits.
		// Limits are inclusive, so they represent the lowest and highest valid number.
		private int AdjustClamp(int current, int adjustBy, int minVal, int maxVal)
		{
			current += adjustBy;
			if (current > maxVal) current = maxVal;
			if (current < minVal) current = minVal;
			return current;
		}

		public void OnAdjustTarget(string command)
		{
			int inc = command[2] == '-' ? -1 : 1;
			char comp = command[1];
			switch (command[0])
			{
				case 'R':
					if (comp == 'H') TargetRAHour = AdjustWrap(TargetRAHour, inc, 0, 23);
					else if (comp == 'M') TargetRAMinute = AdjustWrap(TargetRAMinute, inc, 0, 59);
					else if (comp == 'S') TargetRASecond = AdjustWrap(TargetRASecond, inc, 0, 59);
					else { throw new ArgumentException("Invalid RA component!"); }
					break;
				case 'D':
					if (comp == 'D') TargetDECDegree = AdjustClamp(TargetDECDegree, inc, -90, 90);
					else if (comp == 'M') TargetDECMinute = AdjustWrap(TargetDECMinute, inc, 0, 59);
					else if (comp == 'S') TargetDECSecond = AdjustWrap(TargetDECSecond, inc, 0, 59);
					else { throw new ArgumentException("Invalid DEC component!"); }
					break;
			}
		}

		public ICommand ArrowCommand { get { return _arrowCommand; } }
		public ICommand ConnectScopeCommand { get { return _connectScopeCommand; } }
		public ICommand SlewToTargetCommand { get { return _slewToTargetCommand; } }
		public ICommand SyncToTargetCommand { get { return _syncToTargetCommand; } }
		public ICommand SyncToCurrentCommand { get { return _syncToCurrentCommand; } }
		public ICommand StartSlewingCommand { get { return _startSlewingCommand; } }
		public ICommand StopSlewingCommand { get { return _stopSlewingCommand; } }
		public ICommand HomeCommand { get { return _homeCommand; } }
		public ICommand SetHomeCommand { get { return _setHomeCommand; } }
		public ICommand ParkCommand { get { return _parkCommand; } }
		public ICommand DriftAlignCommand { get { return _driftAlignCommand; } }
		public ICommand PolarAlignCommand { get { return _polarAlignCommand; } }
		public ICommand ShowLogFolderCommand { get { return _showLogFolderCommand; } }

		/// <summary>
		/// Gets or sets the RAHour
		/// </summary>
		public int TargetRAHour
		{
			get { return _targetRAHour; }
			set { SetPropertyValue(ref _targetRAHour, value); }
		}

		/// <summary>
		/// Gets or sets the RAMinute
		/// </summary>
		public int TargetRAMinute
		{
			get { return _targetRAMinute; }
			set { SetPropertyValue(ref _targetRAMinute, value); }
		}

		/// <summary>
		/// Gets or sets the RASecond
		/// </summary>
		public int TargetRASecond
		{
			get { return _targetRASecond; }
			set { SetPropertyValue(ref _targetRASecond, value); }
		}

		/// <summary>
		/// Gets or sets the DECDegree
		/// </summary>
		public int TargetDECDegree
		{
			get { return _targetDECDegree; }
			set { SetPropertyValue(ref _targetDECDegree, value); }
		}

		/// <summary>
		/// Gets or sets the DECMinute
		/// </summary>
		public int TargetDECMinute
		{
			get { return _targetDECMinute; }
			set { SetPropertyValue(ref _targetDECMinute, value); }
		}

		/// <summary>
		/// Gets or sets the DECSecond
		/// </summary>
		public int TargetDECSecond
		{
			get { return _targetDECSecond; }
			set { SetPropertyValue(ref _targetDECSecond, value); }
		}

		/// <summary>
		/// Gets or sets the RAHour
		/// </summary>
		public int CurrentRAHour
		{
			get { return _curRAHour; }
			set { SetPropertyValue(ref _curRAHour, value); }
		}

		/// <summary>
		/// Gets or sets the RAMinute
		/// </summary>
		public int CurrentRAMinute
		{
			get { return _curRAMinute; }
			set { SetPropertyValue(ref _curRAMinute, value); }
		}

		/// <summary>
		/// Gets or sets the RASecond
		/// </summary>
		public int CurrentRASecond
		{
			get { return _curRASecond; }
			set { SetPropertyValue(ref _curRASecond, value); }
		}

		/// <summary>
		/// Gets or sets the DECDegree
		/// </summary>
		public int CurrentDECDegree
		{
			get { return _curDECDegree; }
			set { SetPropertyValue(ref _curDECDegree, value); }
		}

		/// <summary>
		/// Gets or sets the DECMinute
		/// </summary>
		public int CurrentDECMinute
		{
			get { return _curDECMinute; }
			set { SetPropertyValue(ref _curDECMinute, value); }
		}

		/// <summary>
		/// Gets or sets the DECSecond
		/// </summary>
		public int CurrentDECSecond
		{
			get { return _curDECSecond; }
			set { SetPropertyValue(ref _curDECSecond, value); }
		}

		/// <summary>
		/// Gets or sets the TRK Stepper position
		/// </summary>
		public int TrkStepper
		{
			get { return _trkStepper; }
			set { SetPropertyValue(ref _trkStepper, value, OnRAPosChanged); }
		}

		/// <summary>
		/// Gets or sets the RA Stepper position
		/// </summary>
		public int RAStepper
		{
			get { return _raStepper; }
			set { SetPropertyValue(ref _raStepper, value, OnRAPosChanged); }
		}

		private void OnRAPosChanged(int a, int b)
		{
			int raPos = RAStepper + TrkStepper;
			int raStepsLeft = 2 * (21000 - raPos); // Half stepped, so twice as many steps left
			double secondsLeft = (3600.0 / 15.0) * raStepsLeft / (RAStepsPerDegree * SpeedCalibrationFactor);
			RemainingRATime = TimeSpan.FromSeconds(secondsLeft);
		}

		/// <summary>
		/// Gets or sets the DEC Stepper position
		/// </summary>
		public int DECStepper
		{
			get { return _decStepper; }
			set { SetPropertyValue(ref _decStepper, value); }
		}

		/// <summary>
		/// Gets or sets the DECSecond
		/// </summary>
		public double SpeedCalibrationFactor
		{
			get { return _speed; }
			set { SetPropertyValue(ref _speed, value, OnSpeedFactorChanged); }
		}

		private void OnSpeedFactorChanged(double oldVal, double newVal)
		{
			Task.Run(async () =>
			{
				await RunCustomOATCommandAsync(string.Format(_oatCulture, ":XSS{0:0.0000}#", newVal));
				// TrackingSpeed = await GetTrackingSpeed();
			});

		}

		//private async Task<float> GetTrackingSpeed()
		//{
		//	string trackingSpeed = await RunCustomOATCommandAsync(string.Format(_oatCulture, ":XGT#,#"));
		//	return float.Parse(trackingSpeed);
		//}

		/// <summary>
		/// Gets or sets the RA steps per degree
		/// </summary>
		public int RAStepsPerDegree
		{
			get { return _raStepsPerDegree; }
			set { SetPropertyValue(ref _raStepsPerDegree, value, OnRAStepsChanged); }
		}

		private void OnRAStepsChanged(int oldVal, int newVal)
		{
			Task.Run(async () => await RunCustomOATCommandAsync(string.Format(":XSR{0:0}#", newVal)));
		}

		/// <summary>
		/// Gets or sets the DEC steps per degree
		/// </summary>
		public int DECStepsPerDegree
		{
			get { return _decStepsPerDegree; }
			set { SetPropertyValue(ref _decStepsPerDegree, value, OnDECStepsChanged); }
		}

		private void OnDECStepsChanged(int oldVal, int newVal)
		{
			Task.Run(async () => await RunCustomOATCommandAsync(string.Format(":XSD{0:0}#", newVal)));
		}

		public bool DisplaySlewProgress
		{
			get { return _slewInProgress; }
			set { SetPropertyValue(ref _slewInProgress, value); }
		}

		public float RASlewProgress
		{
			get
			{
				if (_slewTargetRA == _slewStartRA) return 1.0f;
				var currentRA = (CurrentRAHour * 60 + CurrentRAMinute) * 60 + CurrentRASecond;
				return 1.0f * (currentRA - _slewStartRA) / (_slewTargetRA - _slewStartRA);
			}
		}

		public float DECSlewProgress
		{
			get
			{
				if (_slewTargetDEC == _slewStartDEC) return 1.0f;
				var currentDEC = (CurrentDECDegree * 60 + CurrentDECMinute) * 60 + CurrentDECSecond;
				return 1.0f * (currentDEC - _slewStartDEC) / (_slewTargetDEC - _slewStartDEC);
			}
		}


		public bool MountConnected
		{
			get { return _connected; }
			set { SetPropertyValue(ref _connected, value, MountConnectedChanged); }
		}

		private void MountConnectedChanged(bool oldVal, bool newVal)
		{
			RequeryCommands();
			OnPropertyChanged("ConnectCommandString");
		}

		/// <summary>
		/// Gets or sets the name of the scope
		/// </summary>
		public string ScopeName
		{
			get { return _scopeName; }
			set { SetPropertyValue(ref _scopeName, value); }
		}

		/// <summary>
		/// Gets or sets the name of the scope
		/// </summary>
		public string ScopeHardware
		{
			get { return _scopeHardware; }
			set { SetPropertyValue(ref _scopeHardware, value); }
		}

		/// <summary>
		/// Gets or sets the status of the scope
		/// </summary>
		public string MountStatus
		{
			get { return _mountStatus; }
			set { SetPropertyValue(ref _mountStatus, value); }
		}

		/// <summary>
		/// Gets or sets the status of the scope
		/// </summary>
		public string DriftAlignStatus
		{
			get { return _driftAlignStatus; }
			set { SetPropertyValue(ref _driftAlignStatus, value); }
		}

		/// <summary>
		/// Gets or sets the name of the scope
		/// </summary>
		public string CurrentHA
		{
			get { return _currentHA; }
			set { SetPropertyValue(ref _currentHA, value); }
		}


		/// <summary>
		/// Gets or sets 
		/// </summary>
		public bool IsCoarseSlewing
		{
			get { return _isCoarseSlewing; }
			set
			{
				SetPropertyValue(ref _isCoarseSlewing, value, OnCoarseSlewingChanged);
			}
		}

		private void OnCoarseSlewingChanged(bool oldVal, bool newVal)
		{
			if (MountConnected)
			{
				try
				{
					Task.Run(async () => await RunCustomOATCommandAsync(string.Format(":XSM{0}#", newVal ? 0 : 1)));
				}
				catch (Exception ex)
				{
					Debug.WriteLine("Unable to set Slewing mode mode." + ex.Message);
				}
			}
		}

		/// <summary>
		/// Gets or sets the status of the scope
		/// </summary>
		public bool IsTracking
		{
			get { return _isTracking; }
			set
			{
				SetPropertyValue(ref _isTracking, value, OnTrackingChanged);
			}
		}

		private void OnTrackingChanged(bool oldVal, bool newVal)
		{
			if (MountConnected)
			{
				try
				{
					Task.Run(async () => await _oatMount.SetTracking(newVal));
				}
				catch (Exception ex)
				{
					Debug.WriteLine("Unable to set Tracking mode." + ex.Message);
				}
			}
		}

		/// <summary>
		/// Gets or sets the status of the scope
		/// </summary>
		public bool IsGuiding
		{
			get { return _isGuiding; }
			set { SetPropertyValue(ref _isGuiding, value); }
		}

		/// <summary>
		/// Gets or sets the status of the scope
		/// </summary>
		public bool IsSlewingNorth
		{
			get { return _isSlewingNorth; }
			set { SetPropertyValue(ref _isSlewingNorth, value); }
		}


		/// <summary>
		/// Gets or sets the status of the scope
		/// </summary>
		public bool IsSlewingSouth
		{
			get { return _isSlewingSouth; }
			set { SetPropertyValue(ref _isSlewingSouth, value); }
		}

		/// <summary>
		/// Gets or sets the status of the scope
		/// </summary>
		public bool IsSlewingEast
		{
			get { return _isSlewingEast; }
			set { SetPropertyValue(ref _isSlewingEast, value); }
		}

		/// <summary>
		/// Gets or sets the status of the scope
		/// </summary>
		public bool IsSlewingWest
		{
			get { return _isSlewingWest; }
			set { SetPropertyValue(ref _isSlewingWest, value); }
		}

		public TimeSpan RemainingRATime
		{
			get { return _remainingRATime; }
			set { SetPropertyValue(ref _remainingRATime, value); }
		}

		/// <summary>
		/// </summary>
		public bool IsDriftAligning
		{
			get { return _driftAlignRunning; }
			set { SetPropertyValue(ref _driftAlignRunning, value); }
		}

		public float DriftPhase
		{
			get { return _driftPhase; }
			set { SetPropertyValue(ref _driftPhase, value); }
		}

		//public float TrackingSpeed
		//{
		//	get { return _trackingSpeed; }
		//	set { SetPropertyValue(ref _trackingSpeed, value, OnRAPosChangedFloat); }
		//}

		private void OnRAPosChangedFloat(float arg1, float arg2)
		{
			OnRAPosChanged(0, 0);
		}

		public float MaxMotorSpeed
		{
			get { return _maxMotorSpeed; }
			set { SetPropertyValue(ref _maxMotorSpeed, value, SlewSpeedChanged); }
		}

		public int SlewRate
		{
			get { return _slewRate; }
			set { SetPropertyValue(ref _slewRate, value, SlewRateChanged); }
		}

		public float SlewXSpeed
		{
			get { return _slewXSpeed; }
			set { SetPropertyValue(ref _slewXSpeed, value, SlewSpeedChanged); }
		}

		public float SlewYSpeed
		{
			get { return _slewYSpeed; }
			set { SetPropertyValue(ref _slewYSpeed, value, SlewSpeedChanged); }
		}

		private async void SlewRateChanged(int arg1, int newRate)
		{
			float[] speeds = { 0, 0.05f, 0.15f, 0.5f, 1.0f };
			string slewRateComdChar = "_GCMS";

			MaxMotorSpeed = speeds[newRate] * 2.5f; // Can't go much quicker than 2.5 degs/sec

			if (MountConnected)
			{
				var slewChange = string.Format(":R{0}#", slewRateComdChar[newRate]);
				await RunCustomOATCommandAsync(slewChange);
			}
		}

		private void SlewSpeedChanged(float arg1, float arg2)
		{
			UpdateMotorSpeeds(-_slewXSpeed * MaxMotorSpeed, -_slewYSpeed * MaxMotorSpeed * 1.5f);
		}

		private void UpdateMotorSpeeds(float v1, float v2)
		{
			lock (_speedUpdateLock)
			{
				_updatedSpeeds = (RASpeed != v1) || (DECSpeed != v2);
				RASpeed = v1;
				DECSpeed = v2;
			}
		}

		private async Task OnFineSlewTimer(object s, EventArgs e)
		{
			_timerFineSlew.Stop();
			if (MountConnected)
			{
				if (!_isCoarseSlewing)
				{
					double raSpeed;
					double decSpeed;
					bool doUpdate = false;
					lock (_speedUpdateLock)
					{
						raSpeed = RASpeed;
						decSpeed = DECSpeed;
						doUpdate = _updatedSpeeds;
						_updatedSpeeds = false;
					}

					if (doUpdate)
					{
						var ras = string.Format(_oatCulture, ":XSX{0:0.000000}#", raSpeed);
						var decs = string.Format(_oatCulture, ":XSY{0:0.000000}#", decSpeed);
						await RunCustomOATCommandAsync(ras);
						await RunCustomOATCommandAsync(decs);
					}
				}
			}
			_timerFineSlew.Start();
		}

		public int SelectedPointOfInterest
		{
			get { return _selectedPointOfInterest; }
			set { SetPropertyValue(ref _selectedPointOfInterest, value, (oldV, newV) => SetTargetFromPOI(newV)); }
		}

		private void SetTargetFromPOI(int newV)
		{
			int h, m, s;
			if ((newV >= 0) && (newV <= _pointsOfInterest.Count))
			{
				var poi = _pointsOfInterest[newV];
				FloatToHMS(poi.DEC, out h, out m, out s);
				TargetDECDegree = h;
				TargetDECMinute = m;
				TargetDECSecond = s;

				FloatToHMS(poi.RA, out h, out m, out s);
				TargetRAHour = h;
				TargetRAMinute = m;
				TargetRASecond = s;
			}
		}

		public List<PointOfInterest> AvailablePointsOfInterest
		{
			get { return _pointsOfInterest; }
		}

		/// <summary>
		/// Gets the string for the connect button
		/// </summary>
		public string ConnectCommandString
		{
			get { return MountConnected ? "Disconnect" : "Connect"; }
		}

		public string ParkCommandString
		{
			get { return _parkString; }
			set { SetPropertyValue(ref _parkString, value); }
		}

		public Version Version { get; private set; }
	}
}

