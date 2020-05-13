using System;
using OATCommunications;
using OATCommunications.TelescopeCommandHandlers;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using OATMobile.Services;
using OATMobile.Views;

namespace OATMobile
{
    public partial class App : Application
    {

        public App()
        {
            InitializeComponent();

            DependencyService.Register<MockDataStore>();
            MainPage = new MainPage();
        }

        protected override void OnStart()
        {
        }

        protected override void OnSleep()
        {
        }

        protected override void OnResume()
        {
        }
    }
}
