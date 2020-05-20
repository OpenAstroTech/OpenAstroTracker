using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
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
		bool _isTracking = false;
		bool _isGuiding = false;
		bool _isSlewingNorth = false;
		bool _isSlewingSouth = false;
		bool _isSlewingWest = false;
		bool _isSlewingEast = false;
		bool _tryConnect = false;
		bool _driftAlignRunning = false;
		bool _runningOATCommand = false;
		object exclusiveAccess = new object();
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

		public float RASpeed { get; private set; }
		public float DECSpeed { get; private set; }

		public MountVM()
		{
			_startTime = DateTime.UtcNow;
			_timerStatus = new DispatcherTimer(TimeSpan.FromMilliseconds(50), DispatcherPriority.Normal, (s, e) => OnTimer(s, e), Application.Current.Dispatcher);
			_timerFineSlew = new DispatcherTimer(TimeSpan.FromMilliseconds(200), DispatcherPriority.Normal, (s, e) => OnFineSlewTimer(s, e), Application.Current.Dispatcher);
			_arrowCommand = new DelegateCommand(s => OnAdjustTarget(s.ToString()));
			_chooseScopeCommand = new DelegateCommand(() => OnChooseTelescope());
			_connectScopeCommand = new DelegateCommand(() => OnConnectToTelescope(), () => _oatMount != null);
			_slewToTargetCommand = new DelegateCommand(() => OnSlewToTarget(), () => MountConnected);
			_startSlewingCommand = new DelegateCommand(s => OnStartSlewing(s.ToString()), () => MountConnected);
			_stopSlewingCommand = new DelegateCommand(() => OnStopSlewing('a'), () => MountConnected);
			_homeCommand = new DelegateCommand(() => OnHome(), () => MountConnected);
			_setHomeCommand = new DelegateCommand(() => OnSetHome(), () => MountConnected);
			_parkCommand = new DelegateCommand(() => OnPark(), () => MountConnected);
			_driftAlignCommand = new DelegateCommand(dur => OnRunDriftAlignment(int.Parse(dur.ToString())), () => MountConnected);

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
			var timestamp = string.Format("[{0:00}:{1:00}:{2:00}.{3:00}] ", elapsed.Hours, elapsed.Minutes, elapsed.Seconds, elapsed.Milliseconds / 10);
			Console.WriteLine(String.Format(timestamp + format, p));
		}

		private void OnSetHome()
		{
			RunCustomOATCommand(":SHP#,#");
			CurrentHA = RunCustomOATCommand(":XGH#,#").TrimEnd("#".ToCharArray()); ;
		}

		private void ReadHA()
		{
			var ha = RunCustomOATCommand(":XGH#,#");
			CurrentHA = String.Format("{0}h {1}m {2}s", ha.Substring(0, 2), ha.Substring(2, 2), ha.Substring(4, 2));
		}

		private string RunCustomOATCommand(string command)
		{
			var t = RunCustomOATCommandAsync(command);
			t.Wait();
			return t.Result;
		}

		async private Task<string> RunCustomOATCommandAsync(string command)
		{
			string result = string.Empty;
			try
			{
				_runningOATCommand = true;
				Log("OATCustom:  StartRunning [{0}]", command);
				Monitor.Enter(exclusiveAccess);
				{
					Log("OATCustom:  -> {0} ", command);
					var commamdResult = await _oatMount.SendCommand(command);
					if (!string.IsNullOrEmpty(commamdResult.Data))
					{
						Log("OATCustomResult [{0}]: '{1}'", command, commamdResult.Data);
						result = commamdResult.Data;
					}
				}
				Monitor.Exit(exclusiveAccess);
			}
			finally
			{
				Log("OATCustom:  StopRunning [{0}]", command);
				_runningOATCommand = false;
			}

			return result;
		}

		private void OnTimer(object s, EventArgs e)
		{
			//if (_tryConnect)
			//{
			//	_tryConnect = false;

			//	MountStatus = "Connecting...";

			//	ScopeName = Settings.Default.Scope;
			//	if (!string.IsNullOrEmpty(ScopeName))
			//	{
			//		_oatMount = new OatmealTelescopeCommandHandlers(ScopeName);
			//		try
			//		{
			//			OnConnectToTelescope();
			//		}
			//		catch (Exception)
			//		{
			//		}

			//		RequeryCommands();
			//	}
			//}

			if (!_runningOATCommand)
			{
				Log("OATTimer:  Not Running");
				if (MountConnected)
				{
					//UpdateCurrentCoordinates();
					UpdateStatus();
				}
			}
			else
			{
				Log("OATTimer:  Running -> Skip");

			}
		}

		private void UpdateStatus()
		{
			//   0   1  2 3 4    5      6
			// Idle,--T,0,0,31,080300,+900000,#
			string status;

			status = RunCustomOATCommand(":GX#,#");

			//Monitor.Enter(exclusiveAccess);
			//status = _oatMount.Action("Serial:PassThroughCommand", ":GX#,#");
			//Monitor.Exit(exclusiveAccess);

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

				IsTracking = parts[1][2] == 'T';

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


		private void OnHome()
		{
			RunCustomOATCommand("hF");
			ReadHA();
		}

		private void OnPark()
		{
			RunCustomOATCommand("hP");
		}

		private bool IsSlewing(char dir)
		{
			if (dir == 'n') return IsSlewingNorth;
			if (dir == 'e') return IsSlewingEast;
			if (dir == 'w') return IsSlewingWest;
			if (dir == 's') return IsSlewingSouth;
			return false;
		}

		private void OnStopSlewing(char dir)
		{
			RunCustomOATCommand(string.Format(":Q{0}#", dir));
		}

		private void OnStartSlewing(char dir)
		{
			RunCustomOATCommand(string.Format(":M{0}#", dir));
		}

		private void OnStartSlewing(string direction)
		{
			char dir = char.ToLower(direction[0]);
			if (IsSlewing(dir))
			{
				OnStopSlewing(dir);
			}
			else
			{
				OnStartSlewing(dir);
			}
		}

		public bool MountConnected
		{
			get
			{
				try
				{
					return _oatMount != null && _oatMount.Connected && !IsDriftAligning;
				}
				catch
				{
					return false;
				}
			}
		}

		private void OnSlewToTarget()
		{
			_oatMount.Slew(new TelescopePosition( 1.0 * TargetRASecond / 3600.0 + 1.0 * TargetRAMinute / 60.0 + TargetRAHour, 1.0 * TargetDECSecond / 3600.0 + 1.0 * TargetDECMinute / 60.0 + TargetDECDegree, Epoch.JNOW));
		}

		private void FloatToHMS(double val, out int h, out int m, out int s)
		{
			h = (int)Math.Floor(val);
			val = (val - h) * 60;
			m = (int)Math.Floor(val);
			val = (val - m) * 60;
			s = (int)Math.Round(val);
		}

		private async void UpdateCurrentCoordinates()
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

		private void OnConnectToTelescope()
		{
			//_oatMount.Connected = !_oatMount.Connected;
			RequeryCommands();

			//if (_oatMount.Connected)
			{
				try
				{
					ScopeName = _oatMount.SendCommand("GVP").Result.Data;

					_transform.SiteElevation = 100;// _oatMount.SiteElevation;
					_transform.SiteLatitude = 47; //_oatMount.SiteLatitude;
					_transform.SiteLongitude = -121;// _oatMount.SiteLongitude;
					_transform.SetAzimuthElevation(0, 90);
					var lst = _transform.RATopocentric;
					var lstS = _util.HoursToHMS(lst, "", "", "");

					Log("LST: {0}", _util.HoursToHMS(lst, "h", "m", "s"));
					RunCustomOATCommand(string.Format(":SHL{0}#,#", _util.HoursToHMS(lst, "", "", "")));
					lst -= _util.HMSToHours("02:58:04");
					Log("HA: {0}", _util.HoursToHMS(lst, "h", "m", "s"));
					UpdateCurrentCoordinates();
					TargetDECDegree = CurrentDECDegree;
					TargetDECMinute = CurrentDECMinute;
					TargetDECSecond = CurrentDECSecond;

					TargetRAHour = CurrentRAHour;
					TargetRAMinute = CurrentRAMinute;
					TargetRASecond = CurrentRASecond;

					string steps = RunCustomOATCommand(string.Format(":XGR#,#")).TrimEnd("#".ToCharArray());
					_raStepsPerDegree = int.Parse(steps);
					steps = RunCustomOATCommand(string.Format(":XGD#,#")).TrimEnd("#".ToCharArray());
					_decStepsPerDegree = int.Parse(steps);
					OnPropertyChanged("RAStepsPerDegree");
					OnPropertyChanged("DECStepsPerDegree");

					steps = RunCustomOATCommand(string.Format(":XGS#,#")).TrimEnd("#".ToCharArray());
					SpeedCalibrationFactor = float.Parse(steps);

					ReadHA();
				}
				catch (Exception ex)
				{
					MessageBox.Show("Error trying to connect to OpenAstroTracker. " + ex.Message, "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
				}
			}
		}

		private async void OnRunDriftAlignment(int driftDuration)
		{
			DriftAlignStatus = "Drift Alignment running...";
			bool wasTracking = _oatMount.SendCommand("GIT").Result.Data == "1";
			IsTracking = false;
			DateTime startTime = DateTime.UtcNow;
			TimeSpan duration = TimeSpan.FromSeconds(2 * driftDuration + 2);
			_timerStatus.Stop();
			await Task.Delay(200);

			try
			{
				RunCustomOATCommand(string.Format(":XD{0:000}#,", driftDuration));
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
			RequeryCommands();
			_timerStatus.Start();
		}

		private void RequeryCommands()
		{
			_connectScopeCommand.Requery();
			_slewToTargetCommand.Requery();
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
			RunCustomOATCommand(string.Format(":XSS{0:0.0000}#,", newVal));
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
			RunCustomOATCommand(string.Format(":XSR{0:0}#,", newVal));
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
			var result = RunCustomOATCommand(string.Format(":XSD{0:0}#,", newVal));
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
				if (MountConnected)
				{
					try
					{
						RunCustomOATCommand(string.Format(":XSM{0}#,", value ? 0 : 1));
					}
					catch
					{

					}
				}

				SetPropertyValue(ref _isCoarseSlewing, value);
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
				if (MountConnected)
				{
					try
					{
						_oatMount.SetTracking(value);
					}
					catch
					{

					}
				}

				SetPropertyValue(ref _isTracking, value);
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

		private void OnFineSlewTimer(object s, EventArgs e)
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
					var ras = string.Format(":XSX{0:0.000000}#,", raSpeed);
					var decs = string.Format(":XSY{0:0.000000}#,", decSpeed);
					RunCustomOATCommand(ras);
					RunCustomOATCommand(decs);
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
			get { return _oatMount == null ? string.Empty : (_oatMount.Connected ? "Disconnect" : "Connect"); }
		}
	}
}

