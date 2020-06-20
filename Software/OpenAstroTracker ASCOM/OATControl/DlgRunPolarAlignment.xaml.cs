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
	public partial class DlgRunPolarAlignment: MetroWindow, INotifyPropertyChanged
	{
		private DelegateCommand _okCommand;
		private DelegateCommand _closeCommand;
		private int _state;
		private Func<string, Task<string>> _sendCommand;

		public DlgRunPolarAlignment(Func<string, Task<string>> sendCommand)
		{
			_sendCommand = sendCommand;
			_closeCommand = new DelegateCommand(() =>
			{
				this.DialogResult = false;
				this.Close();
			});

			_okCommand = new DelegateCommand(async () =>
			{
				this.State++;
				if (_state == 2)
				{
					await _sendCommand($":Sr02:58:15#,n");

					// Now set DEC to move to Home position
					if (Settings.Default.SiteLatitude > 0) // Northern hemisphere
					{
						await _sendCommand($":Sd+90*00:00#,n");
					}
					else
					{
						await _sendCommand($":Sd-90*00:00#,n");
					}
					await _sendCommand($":MS#,n");
				}
				else if (_state == 3)
				{
					if (Settings.Default.SiteLatitude > 0) // Northern hemisphere
					{
						await _sendCommand($":Sd+89*21:03#,n");
					}
					else
					{
						await _sendCommand($":Sd-89*21:03#,n");
					}
					await _sendCommand($":MS#,n");
				}
			});

			this.DataContext = this;
			InitializeComponent();
			State = 1;
		}

		public ICommand OKCommand { get { return _okCommand; } }
		public ICommand CloseCommand { get { return _closeCommand; } }

		public event PropertyChangedEventHandler PropertyChanged;

		public int State
		{
			get { return _state; }
			set
			{
				if (value != _state)
				{ 
					_state= value;
					OnPropertyChanged("State");
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