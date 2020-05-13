using System;
using System.ComponentModel;
using Xamarin.Forms;
using OATMobile.Models;
using OATMobile.ViewModels;

namespace OATMobile.Views
{
    // Learn more about making custom code visible in the Xamarin.Forms previewer
    // by visiting https://aka.ms/xamarinforms-previewer
    [DesignTimeVisible(false)]
    public partial class MountSettingsPage : ContentPage
    {
        MountSettingsViewModel viewModel;

        public MountSettingsPage()
        {
            InitializeComponent();

            BindingContext = viewModel = new MountSettingsViewModel();
        }

        async void OnItemSelected(object sender, EventArgs args)
        {
            var layout = (BindableObject)sender;
            var item = (Item)layout.BindingContext;
            await Navigation.PushAsync(new ItemDetailPage(new ItemDetailViewModel(item)));
        }

        async void AddItem_Clicked(object sender, EventArgs e)
        {
            await Navigation.PushModalAsync(new NavigationPage(new NewItemPage()));
        }

        protected override void OnAppearing()
        {
            base.OnAppearing();

            if (viewModel.Items.Count == 0)
                viewModel.IsBusy = true;
        }

        public double SquareSize => 100;

        private async void Button_OnClicked(object sender, EventArgs e) {
            await viewModel.GoHome();
        }

        private void Button_OnPressed(object sender, EventArgs e) {
            var dir = (sender as Button).CommandParameter.ToString();
            viewModel.StartMoving(dir);
        }

        private void Button_OnReleased(object sender, EventArgs e) {
            var dir = (sender as Button).CommandParameter.ToString();
            viewModel.StopMoving(dir);
        }

        private void TrackingSwitch_OnToggled(object sender, ToggledEventArgs e) {
            viewModel.SetTracking(e.Value);
        }

        private void Location_ButtonClicked(object sender, EventArgs e) {
            viewModel.SetLocation();
        }
    }
}