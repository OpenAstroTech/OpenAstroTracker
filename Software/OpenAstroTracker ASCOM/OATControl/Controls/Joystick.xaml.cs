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
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace OATControl.Controls
{
	/// <summary>
	/// Interaction logic for Joystick.xaml
	/// </summary>
	public partial class Joystick : UserControl
	{
		/// <summary>
		/// The dependency property for the Minimum property
		/// </summary>

		public Joystick()
		{
			InitializeComponent();
			//this.DataContext = this;
		}

		public static readonly DependencyProperty CursorXProperty = DependencyProperty.Register(
			"CursorX",
			typeof(double),
			typeof(Joystick),
			new PropertyMetadata(0.0, Joystick.CursorPosPropertyChanged));

		public static readonly DependencyProperty CursorYProperty = DependencyProperty.Register(
			"CursorY",
			typeof(double),
			typeof(Joystick),
			new PropertyMetadata(0.0, Joystick.CursorPosPropertyChanged));

		public static readonly DependencyProperty RadiusProperty = DependencyProperty.Register(
			"Radius",
			typeof(double),
			typeof(Joystick),
			new PropertyMetadata(1.0, Joystick.CursorPosPropertyChanged));
		private Point _startPoint;
		private double _startX;
		private double _startY;

		private static void CursorPosPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			var joystick = obj as Joystick;
		}

		public double CursorX
		{
			get
			{
				return (double)this.GetValue(Joystick.CursorXProperty);
			}
			set
			{
				this.SetValue(Joystick.CursorXProperty, value);
			}
		}

		public double CursorY
		{
			get
			{
				return (double)this.GetValue(Joystick.CursorYProperty);
			}
			set
			{
				this.SetValue(Joystick.CursorYProperty, value);
			}
		}

		public double Radius
		{
			get
			{
				return (double)this.GetValue(Joystick.RadiusProperty);
			}
			set
			{
				this.SetValue(Joystick.RadiusProperty, value);
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
				this.MainGrid.CaptureMouse();
				_startPoint = e.GetPosition(MainGrid);
				_startX = CursorX;
				_startY = CursorY;
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
		}


		/// <summary>
		/// Handles the OnMouseMove event of the MainGrid control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="MouseEventArgs"/> instance containing the event data.</param>
		private void MainGrid_OnMouseMove(object sender, MouseEventArgs e)
		{
			if (this.MainGrid.IsMouseCaptured)
			{
				this.SetValueFromMouse(e);
			}
		}

		/// <summary>
		/// Sets the value from mouse location.
		/// </summary>
		/// <param name="e">The <see cref="MouseEventArgs"/> instance containing the event data.</param>
		private void SetValueFromMouse(MouseEventArgs e)
		{
			Point pt = e.GetPosition(this.MainGrid);
			var delta = pt - _startPoint;

			Radius = (int)Math.Floor(Math.Min(RenderSize.Width / 2, RenderSize.Height / 2));

			var x = pt.X - this.RenderSize.Width / 2;
			var y = pt.Y - this.RenderSize.Height / 2;
			var len = Math.Sqrt(x * x + y * y);
			if (len != 0)
			{
				x = x / Radius;
				y = y / Radius;
			}


			CursorX = Math.Min(Math.Max(-1.0, x), 1.0);
			CursorY = Math.Min(Math.Max(-1.0, y), 1.0);
		}
	}
}
