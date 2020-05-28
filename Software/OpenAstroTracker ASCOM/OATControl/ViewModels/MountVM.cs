using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;
using System.Xml.Linq;
using ASCOM.Astrometry.AstroUtils;
using ASCOM.Utilities;
using MahApps.Metro.Controls;
using OATCommunications;
using OATCommunications.CommunicationHandlers;
using OATCommunications.Model;
using OATControl.Properties;

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
		bool _isTracking = false;
		bool _isGuiding = false;
		bool _isSlewingNorth = false;
		bool _isSlewingSouth = false;
		bool _isSlewingWest = false;
		bool _isSlewingEast = false;
		bool _tryConnect = false;
		bool _driftAlignRunning = false;
		int _runningOATCommand = 0;
		SemaphoreSlim exclusiveAccess = new SemaphoreSlim(1, 1);
		string _driftAlignStatus = "Drift Alignment";
		float _driftPhase = 0;

		double _speed = 1.0;
		string _scopeName = string.Empty;
		string _mountStatus = string.Empty;
		string _currentHA = string.Empty;

		Util _util;
		ASCOM.Astrometry.Transform.Transform _transform;

		DelegateCommand _arrowCommand;
		DelegateCommand _chooseScopeCommand;
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

		DispatcherTimer _timerStatus;
		DispatcherTimer _timerFineSlew;

		private OatmealTelescopeCommandHandlers _oatMount;
		private List<PointOfInterest> _pointsOfInterest;
		int _selectedPointOfInterest;
		private float _slewYSpeed;
		private float _slewXSpeed;
		private float _maxMotorSpeed = 400;
		private object _speedUpdateLock = new object();
		private bool _updatedSpeeds;
		private bool _isCoarseSlewing = true;
		DateTime _startTime;
		private string _parkString = "Park";

		public float RASpeed { get; private set; }
		public float DECSpeed { get; private set; }

		public MountVM()
		{
			_startTime = DateTime.UtcNow;
			_timerStatus = new DispatcherTimer(TimeSpan.FromMilliseconds(500), DispatcherPriority.Normal, (s, e) => OnTimer(s, e), Application.Current.Dispatcher);
			_timerFineSlew = new DispatcherTimer(TimeSpan.FromMilliseconds(200), DispatcherPriority.Normal, (s, e) => OnFineSlewTimer(s, e), Application.Current.Dispatcher);
			_arrowCommand = new DelegateCommand(s => OnAdjustTarget(s.ToString()));
			_chooseScopeCommand = new DelegateCommand(() => OnChooseTelescope());
			_connectScopeCommand = new DelegateCommand(() => OnConnectToTelescope(), () => _oatMount != null);
			_slewToTargetCommand = new DelegateCommand(async () => await OnSlewToTarget(), () => MountConnected);
			_syncToTargetCommand = new DelegateCommand(async () => await OnSyncToTarget(), () => MountConnected);
			_syncToCurrentCommand = new DelegateCommand(() => OnSyncToCurrent(), () => MountConnected);
			_startSlewingCommand = new DelegateCommand(async s => await OnStartSlewing(s.ToString()), () => MountConnected);
			_stopSlewingCommand = new DelegateCommand(async () => await OnStopSlewing('a'), () => MountConnected);
			_homeCommand = new DelegateCommand(async () => await OnHome(), () => MountConnected);
			_setHomeCommand = new DelegateCommand(async () => await OnSetHome(), () => MountConnected);
			_parkCommand = new DelegateCommand(async () => await OnPark(), () => MountConnected);
			_driftAlignCommand = new DelegateCommand(async dur => await OnRunDriftAlignment(int.Parse(dur.ToString())), () => MountConnected);

			_util = new Util();
			_transform = new ASCOM.Astrometry.Transform.Transform();

			_tryConnect = true;

			var poiFile = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "PointsOfInterest.xml");
			if (File.Exists(poiFile))
			{
				XDocument doc = XDocument.Load(poiFile);
				_pointsOfInterest = doc.Element("PointsOfInterest").Elements("Object").Select(e => new PointOfInterest(e)).ToList();
				_pointsOfInterest.Insert(0, new PointOfInterest("--- Select Target Object ---"));
				_selectedPointOfInterest = 0;
			}
		}

		private void Log(string format, params object[] p)
		{
			var now = DateTime.UtcNow;
			var elapsed = now - _startTime;
			var timestamp = string.Format("[{0:00}:{1:00}:{2:00}.{3:00}] <{4}>:", elapsed.Hours, elapsed.Minutes, elapsed.Seconds, elapsed.Milliseconds / 10, Thread.CurrentThread.ManagedThreadId);
			Console.WriteLine(String.Format(timestamp + format, p));
		}

		private async Task OnSetHome()
		{
			await RunCustomOATCommandAsync(":SHP#,#");
			await ReadHA();
		}

		private async Task ReadHA()
		{
			var ha = await RunCustomOATCommandAsync(":XGH#,#");
			CurrentHA = String.Format("{0}h {1}m {2}s", ha.Substring(0, 2), ha.Substring(2, 2), ha.Substring(4, 2));
		}

		//private string RunCustomOATCommand(string command)
		//{
		//	var t = RunCustomOATCommandAsync(command);
		//	return t.GetAwaiter().GetResult();
		//}

		async private Task<string> RunCustomOATCommandAsync(string command)
		{
			string result = string.Empty;
			Log("OATCustom:  StartRunning [{0}], checking access", command);
			if (Interlocked.CompareExchange(ref _runningOATCommand, 1, 0) == 0)
			{
				try
				{
					await exclusiveAccess.WaitAsync();
					Log("OATCustom:  Access granted. -> Sending {0} ", command);
					var commandResult = await _oatMount.SendCommand(command);
					if (!string.IsNullOrEmpty(commandResult.Data))
					{
						Log("OATCustomResult [{0}]: '{1}'", command, commandResult.Data);
						result = commandResult.Data;
					}
					Interlocked.Decrement(ref _runningOATCommand);
				}
				finally
				{
					Log("OATCustom:  StopRunning [{0}]", command);
					exclusiveAccess.Release();
				}
			}
			else
			{
				Log("OATCustom:  Already running, skipping '{0}'", command);
			}

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
					var parts = status.Split(",".ToCharArray());
					MountStatus = parts[0];

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
			}
		}

		private async Task OnHome()
		{
			await RunCustomOATCommandAsync(":hF#");

			// The next call actually blocks because Homeing is synchronous
			await UpdateStatus();

			await ReadHA();
		}

		private async Task OnPark()
		{
			if (ParkCommandString == "Park")
			{
				await RunCustomOATCommandAsync(":hP#");
				ParkCommandString = "Unpark";
			}
			else
			{
				await RunCustomOATCommandAsync(":hU#,#");
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
			char dir = char.ToLower(direction[0]);
			if (IsSlewing(dir))
			{
				await OnStopSlewing(dir);
			}
			else
			{
				await OnStartSlewing(dir);
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
			//_oatMount.Connected = !_oatMount.Connected;
			RequeryCommands();

			//if (_oatMount.Connected)
			{
				try
				{
					//await +.WaitAsync();

					var result = await _oatMount.SendCommand("GVP#,#");
					var resultNr = await _oatMount.SendCommand("GVN#,#");
					ScopeName = $"{result.Data} {resultNr.Data}";

					_transform.SiteElevation = 100;// _oatMount.SiteElevation;
					_transform.SiteLatitude = 47.74; //_oatMount.SiteLatitude;
					_transform.SiteLongitude = -121.96;// _oatMount.SiteLongitude;
					_transform.SetAzimuthElevation(0, 90);
					var lst = _transform.RATopocentric;
					var lstS = _util.HoursToHMS(lst, "", "", "");

					Log("LST: {0}", _util.HoursToHMS(lst, "h", "m", "s"));
					var stringResult = await RunCustomOATCommandAsync(string.Format(":SHL{0}#,#", _util.HoursToHMS(lst, "", "", "")));
					lst -= _util.HMSToHours("02:58:04");
					Log("HA: {0}", _util.HoursToHMS(lst, "h", "m", "s"));
					await UpdateCurrentCoordinates();
					TargetDECDegree = CurrentDECDegree;
					TargetDECMinute = CurrentDECMinute;
					TargetDECSecond = CurrentDECSecond;

					TargetRAHour = CurrentRAHour;
					TargetRAMinute = CurrentRAMinute;
					TargetRASecond = CurrentRASecond;

					string steps = await RunCustomOATCommandAsync(string.Format(":XGR#,#"));
					_raStepsPerDegree = int.Parse(steps);
					steps = await RunCustomOATCommandAsync(string.Format(":XGD#,#"));
					_decStepsPerDegree = int.Parse(steps);
					OnPropertyChanged("RAStepsPerDegree");
					OnPropertyChanged("DECStepsPerDegree");

					steps = await RunCustomOATCommandAsync(string.Format(":XGS#,#"));
					SpeedCalibrationFactor = float.Parse(steps);

					await ReadHA();
					MountConnected = true;
				}
				catch (Exception ex)
				{
					MessageBox.Show("Error trying to connect to OpenAstroTracker. " + ex.Message, "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
				}
				finally
				{
					//exclusiveAccess.Release();
				}
			}
		}

		private async Task OnRunDriftAlignment(int driftDuration)
		{
			_timerStatus.Stop();

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

			OnPropertyChanged("ConnectCommandString");
		}

		private void OnChooseTelescope()
		{
			//var chooser = new Chooser();
			//chooser.DeviceType = "Telescope";
			//var name = chooser.Choose("ASCOM.OpenAstroTracker.Telescope");
			//if (!string.IsNullOrEmpty(name))
			//{
			//	ScopeName = name;
			_oatMount = new OatmealTelescopeCommandHandlers(
			  new TcpCommunicationHandler(new IPAddress(new byte[] { 192, 168, 86, 21 }), 4030));

			try
			{
				OnConnectToTelescope();
			}
			catch (Exception)
			{
			}


			//	Settings.Default.Scope = name;

			//	Settings.Default.Save();
			//}

			RequeryCommands();
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

		private void OnAdjustTarget(string command)
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
		public ICommand ChooseScopeCommand { get { return _chooseScopeCommand; } }
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
			set { SetPropertyValue(ref _trkStepper, value); }
		}

		/// <summary>
		/// Gets or sets the RA Stepper position
		/// </summary>
		public int RAStepper
		{
			get { return _raStepper; }
			set { SetPropertyValue(ref _raStepper, value); }
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
			Task.Run(async () => await RunCustomOATCommandAsync(string.Format(":XSS{0:0.0000}#", newVal)));
		}

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

		public float MaxMotorSpeed
		{
			get { return _maxMotorSpeed; }
			set { SetPropertyValue(ref _maxMotorSpeed, value, SlewSpeedChanged); }
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

		private void SlewSpeedChanged(float arg1, float arg2)
		{
			UpdateMotorSpeeds(-_slewXSpeed * MaxMotorSpeed, -_slewYSpeed * MaxMotorSpeed * 2);
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
					var ras = string.Format(":XSX{0:0.000000}#", raSpeed);
					var decs = string.Format(":XSY{0:0.000000}#", decSpeed);
					await RunCustomOATCommandAsync(ras);
					await RunCustomOATCommandAsync(decs);
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
	}
}

