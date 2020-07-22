using MahApps.Metro.Controls;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace OATControl
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : MetroWindow
	{
		MountVM mountVm;
		public MainWindow()
		{
			mountVm = new MountVM();
			InitializeComponent();
			this.DataContext = mountVm;
		}

		private void TextBox_GotKeyboardFocus(object sender, KeyboardFocusChangedEventArgs e)
		{
			TextBox tb = sender as TextBox;
			if (tb != null)
			{
				tb.SelectAll();
			}
		}

		private void TextBox_KeyUp(object sender, KeyEventArgs e)
		{
			TextBox tb = sender as TextBox;
			if (tb != null)
			{
				int newNum;
				if (int.TryParse(tb.Text, out newNum))
				{
					string command = tb.Tag as string;
					if (e.Key == Key.Up)
					{
						command += "+";
					}
					else if (e.Key == Key.Down)
					{
						command += "-";
					}
					else
					{
						return;
					}

					mountVm.OnAdjustTarget(command);
				}
			}
		}
	}
}
