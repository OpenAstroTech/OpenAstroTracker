using MahApps.Metro.Converters;
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
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace OATControl.Controls
{
	/// <summary>
	/// Interaction logic for PushButton.xaml
	/// </summary>
	public partial class PushButton : UserControl
	{
		/// <summary>
		/// The dependency property for the Minimum property
		/// </summary>

		public static readonly DependencyProperty DirectionProperty = DependencyProperty.Register(
			"Direction",
			typeof(char),
			typeof(PushButton),
			new PropertyMetadata('A', PushButton.DirectionPropertyChanged));

		public static readonly DependencyProperty IsPressedProperty = DependencyProperty.Register(
			"IsPressed",
			typeof(bool),
			typeof(PushButton),
			new PropertyMetadata(false, PushButton.AnyPropertyChanged));

		public static readonly DependencyProperty RotAngleProperty = DependencyProperty.Register(
			"RotAngle",
			typeof(double),
			typeof(PushButton),
			new PropertyMetadata(0.0, PushButton.AnyPropertyChanged));

		public static readonly DependencyProperty CommandProperty = DependencyProperty.Register(
			"Command",
			typeof(ICommand),
			typeof(PushButton),
			new PropertyMetadata(null, PushButton.AnyPropertyChanged));

		public static readonly DependencyProperty CommandParameterProperty = DependencyProperty.Register(
			"CommandParameter",
			typeof(object),
			typeof(PushButton),
			new PropertyMetadata(null, PushButton.AnyPropertyChanged));

		//
		// Summary:
		//     Gets or sets the parameter to pass to the System.Windows.Controls.Primitives.ButtonBase.Command
		//     property.
		//
		// Returns:
		//     Parameter to pass to the System.Windows.Controls.Primitives.ButtonBase.Command
		//     property.
		[Bindable(true)]
		[Category("Action")]
		[Localizability(LocalizationCategory.NeverLocalize)]
		public object CommandParameter 
		{
			get
			{
				return this.GetValue(PushButton.CommandParameterProperty);
			}
			set
			{
				this.SetValue(PushButton.CommandParameterProperty, value);
			}
		}
		//
		// Summary:
		//     Gets or sets the command to invoke when this button is pressed.
		//
		// Returns:
		//     A command to invoke when this button is pressed. The default value is null.
		[Bindable(true)]
		[Category("Action")]
		[Localizability(LocalizationCategory.NeverLocalize)]
		public ICommand Command 
		{
			get
			{
				return (ICommand)this.GetValue(PushButton.CommandProperty);
			}
			set
			{
				this.SetValue(PushButton.CommandProperty, value);
			}
		}

		private static void DirectionPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			var pushButton = obj as PushButton;
			switch (char.ToUpper((char)e.NewValue))
			{
				case 'N': pushButton.RotAngle = 270.0;break;
				case 'E': pushButton.RotAngle = 0.0; break;
				case 'W': pushButton.RotAngle = 180.0; break;
				case 'S': pushButton.RotAngle = 90.0; break;
			}
		}
		
		private static void AnyPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			var pushButton = obj as PushButton;
		}

		public PushButton()
		{
			InitializeComponent();
			//this.DataContext = this;
		}

		[Bindable(true)]
		public char Direction
		{
			get
			{
				return (char)this.GetValue(PushButton.DirectionProperty);
			}
			set
			{
				this.SetValue(PushButton.DirectionProperty, value);
			}
		}

		[Bindable(true)]
		public double RotAngle
		{
			get
			{
				return (double)this.GetValue(PushButton.RotAngleProperty);
			}
			set
			{
				this.SetValue(PushButton.RotAngleProperty, value);
			}
		}

		[Bindable(true)]
		public bool IsPressed
		{
			get
			{
				return (bool)this.GetValue(PushButton.IsPressedProperty);
			}
			set
			{
				this.SetValue(PushButton.IsPressedProperty, value);
			}
		}


		/// <summary>
		/// Handles the OnMouseButtonDown event of the MainGrid control. Captures the mouse and sets the current 
		/// value to the click point. If the textbox currently is active, it is made inactive.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="MouseButtonEventArgs"/> instance containing the event data.</param>
		private void MainGrid_OnMouseButtonDown(object sender, MouseButtonEventArgs e)
		{
			if (e.LeftButton == MouseButtonState.Pressed)
			{
				IsPressed = true;
				this.MainGrid.CaptureMouse();
				this.Command.Execute("+" + CommandParameter.ToString());
			}
		}

		/// <summary>
		/// Handles the OnMouseButtonUp event of the MainGrid control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="MouseButtonEventArgs"/> instance containing the event data.</param>
		private void MainGrid_OnMouseButtonUp(object sender, MouseButtonEventArgs e)
		{
			this.MainGrid.ReleaseMouseCapture();
			IsPressed = false;
			this.Command.Execute("-" + CommandParameter.ToString());
		}
	}
}
