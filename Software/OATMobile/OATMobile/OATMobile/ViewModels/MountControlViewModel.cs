using System.ComponentModel;
using OATCommunications.Model;
using OATCommunications.TelescopeCommandHandlers;
using Xamarin.Forms;
using Xamarin.Essentials;
using System.Linq;
using OATCommunications;
using OATCommunications.CommunicationHandlers;
using System.Net;
using System.Timers;

namespace OATMobile.ViewModels
{
	public class MountControlViewModel : ViewModelBase
	{
		private ITelescopeCommandHandler _cmdHandler;

		public string OppositeParkStateText { get; private set; } = "Unpark";
		private Timer discoveryTimer = new Timer(500);

		public MountControlViewModel()
		{
			Title = "Control";

			if (CommunicationHandlerFactory.AvailableDevices.Any())
			{
				ConnectToDevice();
			}
			else
			{
				discoveryTimer.Elapsed += (s, e) =>
				{
					if (CommunicationHandlerFactory.AvailableDevices.Any())
					{
						discoveryTimer.Stop();

						// This is probably needed for Windows. Seems to work OK on Android. 
						// RunOnUiThread() is platform specific :-(

						//Activity.RunOnUiThread(() =>
						//{
							ConnectToDevice();
							discoveryTimer.Dispose();
							discoveryTimer = null;
						//});
					}
				};

				discoveryTimer.Start();
			}
		}

		public void ConnectToDevice()
		{
			var device = CommunicationHandlerFactory.ConnectToDevice(CommunicationHandlerFactory.AvailableDevices.First());
			_cmdHandler = new OatmealTelescopeCommandHandlers(device);

			_cmdHandler.MountState.PropertyChanged += MountStateOnPropertyChanged;

			Commands.Add("GoHome", new Command(async () =>
			{
				await _cmdHandler?.GoHome();
				await _cmdHandler?.RefreshMountState();
			}));

			Commands.Add("StartMoveDirection", new Command<string>(x => { _cmdHandler?.StartMoving(x); }));

			Commands.Add("StopMoveDirection", new Command<string>(x => { _cmdHandler?.StopMoving(x); }));

			Commands.Add("SetTracking", new Command<bool>(x => { _cmdHandler?.SetTracking(x); }));

			Commands.Add("SetLocation", new Command(async () =>
			{
				var location = await Geolocation.GetLocationAsync(new GeolocationRequest(GeolocationAccuracy.Default));
				await _cmdHandler?.SetLocation(location.Latitude, location.Longitude, 0, 0);
			}));

			Commands.Add("ToggleParkedState", new Command(async () =>
			{
				if (_cmdHandler != null)
				{
					if (_cmdHandler.MountState.IsTracking)
					{
						await _cmdHandler?.SetTracking(false);
					}
					else
					{
						await _cmdHandler?.SetTracking(true);
					}
				}
			}));

			_cmdHandler?.RefreshMountState();
		}

		private void MountStateOnPropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			if (_cmdHandler != null)
			{
				switch (e.PropertyName)
				{
					case nameof(MountState.IsTracking):
						OppositeParkStateText = _cmdHandler.MountState.IsTracking ? "Park" : "Unpark";
						break;
				}
			}
		}

		public bool IsConnected { get; private set; }
	}
}