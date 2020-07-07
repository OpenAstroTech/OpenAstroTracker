using Xamarin.Forms;
using OATMobile.Services;
using OATMobile.Views;

using OATCommunications.CommunicationHandlers;

namespace OATMobile
{
    public partial class App : Application
    {

        public App()
        {
            InitializeComponent();

            //DependencyService.Register<MockDataStore>();
            MainPage = new MainPage();
        }

        protected override void OnStart()
        {
            CommunicationHandlerFactory.DiscoverDevices();
        }

        protected override void OnSleep()
        {
        }

        protected override void OnResume()
        {
        }
    }
}
