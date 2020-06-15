using System.ComponentModel;
using OATCommunications.Model;
using OATCommunications.TelescopeCommandHandlers;
using Xamarin.Forms;
using Xamarin.Essentials;

namespace OATMobile.ViewModels {
    public class MountControlViewModel : ViewModelBase {

        private ITelescopeCommandHandler _cmdHandler;

        public string OppositeParkStateText { get; private set; } = "Unpark";

        public MountControlViewModel(ITelescopeCommandHandler handler) {
            _cmdHandler = handler;
            Title = "Control";

            _cmdHandler.MountState.PropertyChanged += MountStateOnPropertyChanged;

            Commands.Add("GoHome", new Command(async () => {
                await _cmdHandler.GoHome();
                await _cmdHandler.RefreshMountState();
            }));

            Commands.Add("StartMoveDirection", new Command<string>(x => { _cmdHandler.StartMoving(x); }));

            Commands.Add("StopMoveDirection", new Command<string>(x => { _cmdHandler.StopMoving(x); }));

            Commands.Add("SetTracking", new Command<bool>(x => { _cmdHandler.SetTracking(x); }));

            Commands.Add("SetLocation", new Command(async () => {
                var location = await Geolocation.GetLocationAsync(new GeolocationRequest(GeolocationAccuracy.Default));
                await _cmdHandler.SetLocation(location.Latitude, location.Longitude, 0, 0);
            }));

            Commands.Add("ToggleParkedState", new Command(async () => {
                if (_cmdHandler.MountState.IsTracking) {
                    await _cmdHandler.SetTracking(false);
                }
                else {
                    await _cmdHandler.SetTracking(true);
                }
            }));

            _cmdHandler.RefreshMountState();
        }

        private void MountStateOnPropertyChanged(object sender, PropertyChangedEventArgs e) {
            switch (e.PropertyName) {
                case nameof(MountState.IsTracking):
                    OppositeParkStateText = _cmdHandler.MountState.IsTracking ? "Park" : "Unpark";
                    break;
            }
        }

        public bool IsConnected { get; private set; }
    }
}