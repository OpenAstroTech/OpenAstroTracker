using MahApps.Metro.Controls;
using OATCommunications.WPF.CommunicationHandlers;
using OATControl.Properties;
using OATControl.ViewModels;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace OATControl
{
	/// <summary>
	/// Interaction logic for DlgChooseOat.xaml
	/// </summary>
	public partial class DlgChooseOat : MetroWindow, INotifyPropertyChanged
	{
		public enum Steps
		{
			Idle,
			WaitForConnect,
			CheckHardware,
			WaitForLevel,
			WaitForGPS,
			ConfirmLocation,
			Completed
		};
		private DelegateCommand _rescanCommand;
		private DelegateCommand _connectAndNextCommand;
		private float _latitude = 45;
		private float _longitude = -75;
		private float _altitude = 100;
		private string _device;
		private bool _showGPSStatus = false;
		private bool _showManualLocation = false;
		private bool _showNextButton = false;
		private bool _showLevelDisplay = false;
		private double _rollOffset;
		private List<double> _rollOffsetHistory;
		private float _rollReference;
		private MountVM _mountViewModel;
		private Steps _currentStep = Steps.Idle;
		private string _gpsStatus = string.Empty;
		private Func<string, Task<string>> _sendCommand;
		private DispatcherTimer stateTimer;
		const float MaxWaitForGPS = 30;
		DateTime _startedGPSWaitAt;

		public DlgChooseOat(MountVM mountViewModel, Func<string, Task<string>> sendCommand)
		{
			stateTimer = new DispatcherTimer();
			stateTimer.Tick += new EventHandler(ProcessStateMachine);
			stateTimer.Interval = TimeSpan.FromMilliseconds(100);

			_sendCommand = sendCommand;
			_mountViewModel = mountViewModel;
			_rollOffsetHistory = new List<double>(5);
			_latitude = Settings.Default.SiteLatitude;
			_longitude = Settings.Default.SiteLongitude;

			CurrentStep = Steps.Idle;
			_rescanCommand = new DelegateCommand(() => { CommunicationHandlerFactory.DiscoverDevices(); }, () => (_currentStep == Steps.Idle) || (_currentStep == Steps.WaitForConnect) );
			_connectAndNextCommand = new DelegateCommand((o) => AdvanceStateMachine(), () => IsNextEnabled);

			this.DataContext = this;
			InitializeComponent();

			stateTimer.Start();
		}

		public ICommand RescanCommand { get { return _rescanCommand; } }
		public ICommand ConnectAndNextCommand { get { return _connectAndNextCommand; } }

		public IList<string> AvailableDevices
		{
			get { return CommunicationHandlerFactory.AvailableDevices; }
		}

		public string SelectedDevice
		{
			get { return _device; }
			set
			{
				if (value != _device)
				{
					_device = value;
					if (CurrentStep== Steps.Idle)
					{
						AdvanceStateMachine();
					}
					OnPropertyChanged("SelectedDevice");
					
				}
			}
		}

		public string GPSStatus
		{
			get { return _gpsStatus; }
			set
			{
				if (value != _gpsStatus)
				{
					_gpsStatus = value;
					OnPropertyChanged("GPSStatus");
				}
			}
		}

		public bool ShowNextButton
		{
			get { return _showNextButton; }
			set
			{
				if (value != _showNextButton)
				{
					_showNextButton = value;
					OnPropertyChanged("ShowNextButton");
				}
			}
		}

		public bool ShowManualLocation
		{
			get { return _showManualLocation; }
			set
			{
				if (value != _showManualLocation)
				{
					_showManualLocation = value;
					OnPropertyChanged("ShowManualLocation");
				}
			}
		}

		public bool ShowGPSStatus
		{
			get { return _showGPSStatus; }
			set
			{
				if (value != _showGPSStatus)
				{
					_showGPSStatus = value;
					OnPropertyChanged("ShowGPSStatus");
				}
			}
		}

		public bool ShowLevelDisplay
		{
			get { return _showLevelDisplay; }
			set
			{
				if (value != _showLevelDisplay)
				{
					_showLevelDisplay = value;
					OnPropertyChanged("ShowLevelDisplay");
				}
			}
		}

		public double RollOffset
		{
			get { return _rollOffset; }
			set
			{
				if (value != _rollOffset)
				{
					_rollOffset = value;
					OnPropertyChanged("RollOffset");
				}
			}
		}

		public Steps CurrentStep
		{
			get { return _currentStep; }
			set
			{
				if (value != _currentStep)
				{
					_currentStep = value;
					OnPropertyChanged("CurrentStep");
				}
			}
		}

		public event PropertyChangedEventHandler PropertyChanged;

		public float Latitude
		{
			get { return _latitude; }
			set
			{
				if (value != _latitude)
				{
					_latitude = value;
					OnPropertyChanged("Latitude");
				}
			}
		}

		public float Longitude
		{
			get { return _longitude; }
			set
			{
				if (value != _longitude)
				{
					_longitude = value;
					OnPropertyChanged("Longitude");
				}
			}
		}

		public float Altitude
		{
			get { return _altitude; }
			set
			{
				if (value != _altitude)
				{
					_altitude = value;
					OnPropertyChanged("Altitude");
				}
			}
		}

		private void OnPropertyChanged(string field)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, new PropertyChangedEventArgs(field));
				RequeryCommands();
			}
		}
		private void RequeryCommands()
		{
			_rescanCommand.Requery();
			_connectAndNextCommand.Requery();
		}

		protected override void OnClosed(EventArgs e)
		{
			stateTimer.Stop();
			base.OnClosed(e);
		}

		private void AdvanceStateMachine()
		{
			switch (CurrentStep)
			{
				case Steps.Idle:
					CurrentStep = Steps.WaitForConnect;
					ShowNextButton = true;
					break;

				case Steps.WaitForConnect:
					GPSStatus = string.Format("Connecting to OAT on {0}", SelectedDevice);
					ShowGPSStatus = true;
					CurrentStep = Steps.CheckHardware;
					break;

				case Steps.CheckHardware:
					// NOP
					break;

				case Steps.WaitForLevel:
					// Turn off the Level
					Task.Run(() => _sendCommand(":XL0#,#")).Wait();

					if (_mountViewModel.IsAddonSupported("GPS"))
					{
						CurrentStep = Steps.WaitForGPS;
						ShowGPSStatus = true;
						_startedGPSWaitAt = DateTime.UtcNow;
					}
					else
					{
						ShowManualLocation = true;
						CurrentStep = Steps.ConfirmLocation;
					}
					break;

				case Steps.WaitForGPS:
					ShowManualLocation = true;
					CurrentStep = Steps.ConfirmLocation;
					GPSStatus = "GPS acquisition cancelled, please enter location:";
					break;

				case Steps.ConfirmLocation:
					GPSStatus = "Sync'd! Setting OAT location...";
					Task.Run(() => _mountViewModel.SetSiteLatitude(Latitude)).Wait();
					Task.Run(() => _mountViewModel.SetSiteLongitude(Longitude)).Wait();
					Settings.Default.SiteLatitude = Latitude;
					Settings.Default.SiteLongitude = Longitude;
					Settings.Default.Save();
					CurrentStep = Steps.Completed;
					break;

				case Steps.Completed:
					this.DialogResult = true;
					this.Close();
					break;

				default: break;
			}
		}

		private async void ProcessStateMachine(object sender, EventArgs e)
		{
			stateTimer.Stop();
			switch (CurrentStep)
			{
				case Steps.WaitForConnect:
					break;

				case Steps.CheckHardware:
					await _mountViewModel.ConnectToOat(SelectedDevice);

					ShowGPSStatus = false;
					GPSStatus = string.Empty;

					if (_mountViewModel.IsAddonSupported("GYRO"))
					{
						CurrentStep = Steps.WaitForLevel;

						// Turn on the Digital Level
						await _sendCommand(":XL1#,#");

						// Get the reference angles from the level.
						string referenceAngles = await _sendCommand(":XLGR#,#");
						_rollReference = float.Parse(referenceAngles.Split(",".ToCharArray())[1]);
						ShowLevelDisplay = true;
					}
					else if (_mountViewModel.IsAddonSupported("GPS"))
					{
						CurrentStep = Steps.WaitForGPS;
						ShowGPSStatus = true;
						_startedGPSWaitAt = DateTime.UtcNow;
					}
					else
					{
						ShowManualLocation = true;
						CurrentStep = Steps.ConfirmLocation;
					}
					break;

				case Steps.WaitForLevel:
					// Get the current digital level angles.
					string currentAngles = await _sendCommand(":XLGC#,#");
					float currentRoll = float.Parse(currentAngles.Split(",".ToCharArray())[1]);

					// Keep a rolling average of the last 6 values.
					if (_rollOffsetHistory.Count > 5)
					{
						_rollOffsetHistory.RemoveAt(0);
					}

					_rollOffsetHistory.Add(currentRoll - _rollReference);
					RollOffset = _rollOffsetHistory.Average();
					break;

				case Steps.WaitForGPS:
					TimeSpan elapsed;
					elapsed = DateTime.UtcNow - _startedGPSWaitAt;
					if (elapsed.TotalSeconds >= MaxWaitForGPS)
					{
						GPSStatus = string.Format("GPS could not get a location lock. Please enter location manually:");
						ShowManualLocation = true;
						CurrentStep = Steps.ConfirmLocation;
						break;
					}

					GPSStatus = string.Format("Waiting {0:0}s for GPS to find satellites and sync...", MaxWaitForGPS - elapsed.TotalSeconds);
					await Task.Delay(150);

					var result = await _sendCommand(":gT100#,n");
					if (result == "1")
					{
						GPSStatus = "Sync'd! Retrieving current location...";
						result = await _sendCommand(":Gt#,#");
						float latitude = float.Parse(result.Substring(0, 3)) + (float.Parse(result.Substring(4)) / 60.0f);
						await Task.Delay(250);
						result = await _sendCommand(":Gg#,#");
						float longitude = float.Parse(result.Substring(0, 3)) + (float.Parse(result.Substring(4)) / 60.0f);
						if (longitude > 180) longitude -= 360;
						await Task.Delay(250);

						GPSStatus = "Sync'd! Setting OAT location...";
						await _mountViewModel.SetSiteLatitude(latitude);
						await _mountViewModel.SetSiteLongitude(longitude);
						await Task.Delay(250);

						Settings.Default.SiteLatitude = latitude;
						Settings.Default.SiteLongitude = longitude;
						Settings.Default.Save();

						CurrentStep = Steps.Completed;
					}
					break;

				case Steps.ConfirmLocation:
					break;

				case Steps.Completed:
					this.DialogResult = true;
					this.Close();
					return;

				default: break;
			}

			stateTimer.Start();
		}

		public bool IsNextEnabled
		{
			get
			{
				switch (CurrentStep)
				{
					case Steps.Idle:
						return false;

					case Steps.WaitForConnect:
						return !string.IsNullOrEmpty(SelectedDevice);

					case Steps.CheckHardware:
						return false;

					case Steps.WaitForLevel:
						return true;

					case Steps.WaitForGPS:
						return true;

					case Steps.ConfirmLocation:
						return true;

					case Steps.Completed:
						return true;

					default: break;
				}
				return false;
			}
		}

	}
}