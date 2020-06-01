using MahApps.Metro.Controls;
using OATCommunications.WPF.CommunicationHandlers;
using OATControl.ViewModels;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace OATControl
{
	/// <summary>
	/// Interaction logic for DlgChooseOat.xaml
	/// </summary>
	public partial class DlgChooseOat : MetroWindow
	{
		private DelegateCommand _okCommand;
		private DelegateCommand _rescanCommand;
		public DlgChooseOat()
		{
			_okCommand = new DelegateCommand(() => { this.DialogResult = true; Close(); });
			_rescanCommand = new DelegateCommand(() => { CommunicationHandlerFactory.DiscoverDevices(); });

			this.DataContext = this;
			InitializeComponent();
			
		}

		public ICommand OKCommand { get { return _okCommand; } }
		public ICommand RescanCommand { get { return _rescanCommand; } }

		public IList<string> AvailableDevices
		{
			get { return CommunicationHandlerFactory.AvailableDevices; }
		}

		public string SelectedDevice { get; set; }
	}
}