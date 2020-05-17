using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using OATCommunications.Model;
using Xamarin.Forms;
using OATMobile.Services;

namespace OATMobile.ViewModels
{
    public class ViewModelBase : ObservableClass
    {
        public Dictionary<string, ICommand> Commands { get; protected set; }

        public ViewModelBase() {
            Commands = new Dictionary<string, ICommand>();
        }

        public string Title { get; set; } = string.Empty;
    }
}
