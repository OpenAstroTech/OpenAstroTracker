using System;
using System.ComponentModel;
using System.Net;
using OATCommunications;
using OATCommunications.CommunicationHandlers;
using Xamarin.Forms;
using OATMobile.ViewModels;
using System.Linq;

namespace OATMobile.Views {
    // Learn more about making custom code visible in the Xamarin.Forms previewer
    // by visiting https://aka.ms/xamarinforms-previewer
    [DesignTimeVisible(false)]
    public partial class MountControlView : ContentPage {
        MountControlViewModel viewModel;

        public MountControlView() {
            InitializeComponent();
            BindingContext = viewModel = new MountControlViewModel();
        }

        private void Move_Button_Pressed(object sender, EventArgs e) {
            var dir = (sender as ImageButton).CommandParameter.ToString();
            if (viewModel.Commands.Any())
            {
                viewModel.Commands["StartMoveDirection"].Execute(dir);
            }
        }

        private void Move_Button_Released(object sender, EventArgs e) {
            var dir = (sender as ImageButton).CommandParameter.ToString();
            if (viewModel.Commands.Any())
            {
                viewModel.Commands["StopMoveDirection"].Execute(dir);
            }
        }
    }
}