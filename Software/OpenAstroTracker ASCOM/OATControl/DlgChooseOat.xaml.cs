using MahApps.Metro.Controls;
using OATCommunications.CommunicationHandlers;
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
		public DlgChooseOat()
		{
			InitializeComponent();
		}


		public IList<string> AvailableDevices
		{
			get { return CommunicationHandlerFactory.AvailableDevices; }
		}

		public string SelectedDevice { get; set; }
	}
}