using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace OATControl.Controls
{
	class ScopePointer : FrameworkElement
	{
		private Pen _pen;
		private Brush _brush;
		private double _x;
		private double _y;

		public Brush Foreground
		{
			get { return _brush; }
			set
			{
				_brush = value;
				_pen = new Pen(_brush, 1.5);
			}
		}

		public static readonly DependencyProperty XProperty = DependencyProperty.Register(
			"X",
			typeof(double),
			typeof(ScopePointer),
			new PropertyMetadata(0.0, ScopePointer.SomePropertyChanged));

		public static readonly DependencyProperty YProperty = DependencyProperty.Register(
			"Y",
			typeof(double),
			typeof(ScopePointer),
			new PropertyMetadata(0.0, ScopePointer.SomePropertyChanged));

		public static readonly DependencyProperty ScaleProperty = DependencyProperty.Register(
			"Scale",
			typeof(double),
			typeof(ScopePointer),
			new PropertyMetadata(1.0, ScopePointer.SomePropertyChanged));

		private static void SomePropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			var pointer = obj as ScopePointer;
			pointer.InvalidateVisual();
		}


		public double X
		{
			get
			{
				return (double)this.GetValue(ScopePointer.XProperty);
			}
			set
			{
				this.SetValue(ScopePointer.XProperty, value);
			}
		}


		public double Y
		{
			get
			{
				return (double)this.GetValue(ScopePointer.YProperty);
			}
			set
			{
				this.SetValue(ScopePointer.YProperty, value);
			}
		}

		public double Scale
		{
			get
			{
				return (double)this.GetValue(ScopePointer.ScaleProperty);
			}
			set
			{
				this.SetValue(ScopePointer.ScaleProperty, value);
			}
		}

		protected override Size MeasureOverride(Size availableSize)
		{
			return new Size(20.0, 20.0);
		}

		protected override void OnRender(DrawingContext dc)
		{
			Point center = new Point(RenderSize.Width / 2, RenderSize.Height / 2);
			Point cursorPos = new Point(center.X + Scale * X, center.Y + Scale * Y);

			dc.DrawEllipse(null, _pen, cursorPos, 10, 10);
			dc.DrawEllipse(null, _pen, cursorPos, 5, 5);

			Point p1 = new Point(cursorPos.X - 12, cursorPos.Y);
			Point p2 = new Point(cursorPos.X + 12, cursorPos.Y);
			dc.DrawLine(_pen, p1, p2);

			p1 = new Point(cursorPos.X, cursorPos.Y - 12);
			p2 = new Point(cursorPos.X, cursorPos.Y + 12);
			dc.DrawLine(_pen, p1, p2);
		}
	}
}
