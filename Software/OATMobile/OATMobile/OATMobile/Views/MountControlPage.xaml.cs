using System;
using System.ComponentModel;
using System.Net;
using OATCommunications;
using OATCommunications.CommunicationHandlers;
using Xamarin.Forms;
using OATMobile.ViewModels;

namespace OATMobile.Views {
    // Learn more about making custom code visible in the Xamarin.Forms previewer
    // by visiting https://aka.ms/xamarinforms-previewer
    [DesignTimeVisible(false)]
    public partial class MountControlPage : ContentPage {
        MountControlViewModel viewModel;

        public MountControlPage() {
            InitializeComponent();
            var handler =
                new OatmealTelescopeCommandHandlers(
                    new TcpCommunicationHandler(new IPAddress(new byte[] {192, 168, 86, 57}), 4030));
            BindingContext = viewModel = new MountControlViewModel(handler);
        }

        private void Move_Button_Pressed(object sender, EventArgs e) {
            var dir = (sender as Button).CommandParameter.ToString();
            viewModel.Commands["StartMoveDirection"].Execute(dir);
        }

        private void Move_Button_Released(object sender, EventArgs e) {
            var dir = (sender as Button).CommandParameter.ToString();
            viewModel.Commands["StopMoveDirection"].Execute(dir);
        }
    }
}