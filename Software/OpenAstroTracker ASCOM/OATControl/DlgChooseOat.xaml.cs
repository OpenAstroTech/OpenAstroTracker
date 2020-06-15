using MahApps.Metro.Controls;
using OATCommunications.WPF.CommunicationHandlers;
using OATControl.Properties;
using OATControl.ViewModels;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
	public partial class DlgChooseOat : MetroWindow, INotifyPropertyChanged
	{
		private DelegateCommand _okCommand;
		private DelegateCommand _rescanCommand;
		private float _latitude = 45;
		private float _longitude = -75;
		private float _altitude = 100;
		private string _device;

		public DlgChooseOat()
		{
			_latitude = Settings.Default.SiteLatitude;
			_longitude = Settings.Default.SiteLongitude;
			_okCommand = new DelegateCommand(() =>
			{
				this.DialogResult = true;
				Settings.Default.SiteLatitude = Latitude;
				Settings.Default.SiteLongitude = Longitude;
				Settings.Default.Save();
				Close();
			}, () => SelectedDevice != null);
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

		public string SelectedDevice 
		{
			get { return _device; }
			set
			{
				if (value != _device)
				{
					_device = value;
					OnPropertyChanged("SelectedDevice");
					_okCommand.Requery();
				}
			}
		}

		public event PropertyChangedEventHandler PropertyChanged;

		public float Latitude
		{
			get { return _latitude; }
			set
			{
				if (value != _latitude)
				{
					_latitude = value;
					OnPropertyChanged("Latitude");
				}
			}
		}

		public float Longitude
		{
			get { return _longitude; }
			set
			{
				if (value != _longitude)
				{
					_longitude = value;
					OnPropertyChanged("Longitude");
				}
			}
		}

		public float Altitude
		{
			get { return _altitude; }
			set
			{
				if (value != _altitude)
				{
					_altitude = value;
					OnPropertyChanged("Altitude");
				}
			}
		}

		private void OnPropertyChanged(string field)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, new PropertyChangedEventArgs(field));
			}
		}
	}
}