using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Net;
using System.Threading.Tasks;
using System.Windows.Input;
using OATCommunications;
using OATCommunications.CommunicationHandlers;
using OATCommunications.TelescopeCommandHandlers;
using Xamarin.Forms;

using OATMobile.Models;
using OATMobile.Views;
using Xamarin.Essentials;

namespace OATMobile.ViewModels
{
    public class MountSettingsViewModel : BaseViewModel
    {
        public ObservableCollection<Item> Items { get; set; }
        public Command LoadItemsCommand { get; set; }

        public IObservable<bool> IsTracking { get; set; }

        private ITelescopeCommandHandler _cmdHandler;

        public MountSettingsViewModel()
        {
            _cmdHandler = new OatmealTelescopeCommandHandlers(new TcpCommunicationHandler(new IPAddress(new byte[] { 192, 168, 86, 250 }), 4030));
            Title = "Mount Settings";
            Items = new ObservableCollection<Item>();
            LoadItemsCommand = new Command(async () => await ExecuteLoadItemsCommand());

            MessagingCenter.Subscribe<NewItemPage, Item>(this, "AddItem", async (obj, item) =>
            {
                var newItem = item as Item;
                Items.Add(newItem);
                await DataStore.AddItemAsync(newItem);
            });
        }

        public async Task<bool> GoHome() {
            return await _cmdHandler.GoHome();
        }

        public async Task<bool> StartMoving(string dir) {
            return await _cmdHandler.StartMoving(dir);
        }

        public async Task<bool> StopMoving(string dir) {
            return await _cmdHandler.StopMoving(dir);
        }

        public async Task<bool> SetTracking(bool enabled) {
            return await _cmdHandler.SetTracking(enabled);
        }

        public async Task<bool> SetLocation() {
            var location = await Geolocation.GetLocationAsync(new GeolocationRequest(GeolocationAccuracy.Default));
            return await _cmdHandler.SetLocation(location.Latitude, location.Longitude, 0, 0);
        }

        async Task ExecuteLoadItemsCommand()
        {
            IsBusy = true;

            try
            {
                Items.Clear();
                var items = await DataStore.GetItemsAsync(true);
                foreach (var item in items)
                {
                    Items.Add(item);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
            }
            finally
            {
                IsBusy = false;
            }
        }
    }
}