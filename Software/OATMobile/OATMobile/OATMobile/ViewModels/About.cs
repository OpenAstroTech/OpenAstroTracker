using System;
using System.Windows.Input;
using Xamarin.Essentials;
using Xamarin.Forms;

namespace OATMobile.ViewModels
{
    public class About : ViewModelBase
    {
        public About()
        {
            Title = "About";
            OpenWebCommand = new Command(async () => await Browser.OpenAsync("https://xamarin.com"));
        }

        public ICommand OpenWebCommand { get; }
    }
}