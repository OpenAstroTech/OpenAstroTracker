using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace OATControl.Controls
{
	public class Arrow : FrameworkElement
	{
		private Pen _pen;
		private double _angle;

		public static readonly DependencyProperty AngleProperty = DependencyProperty.Register(
			"Angle",
			typeof(double),
			typeof(Arrow),
			new PropertyMetadata(0.0, Arrow.SomePropertyChanged));

		public static readonly DependencyProperty BackgroundProperty = DependencyProperty.Register(
			"Background",
			typeof(Brush),
			typeof(Arrow),
			new PropertyMetadata(Brushes.White, Arrow.SomePropertyChanged));

		public static readonly DependencyProperty ForegroundProperty = DependencyProperty.Register(
			"Foreground",
			typeof(Brush),
			typeof(Arrow),
			new PropertyMetadata(Brushes.Black, Arrow.SomePropertyChanged));


		//public static readonly DependencyProperty ScaleProperty = DependencyProperty.Register(
		//	"Scale",
		//	typeof(double),
		//	typeof(Arrow),
		//	new PropertyMetadata(1.0, Arrow.SomePropertyChanged));

		private static void SomePropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			var pointer = obj as Arrow;
			pointer.InvalidateVisual();
		}


		public double Angle
		{
			get
			{
				return (double)this.GetValue(Arrow.AngleProperty);
			}
			set
			{
				this.SetValue(Arrow.AngleProperty, value);
			}
		}

		public Brush Foreground
		{
			get
			{
				return (Brush)this.GetValue(Arrow.ForegroundProperty);
			}
			set
			{
				this.SetValue(Arrow.ForegroundProperty, value);
			}
		}

		public Brush Background
		{
			get
			{
				return (Brush)this.GetValue(Arrow.BackgroundProperty);
			}
			set
			{
				this.SetValue(Arrow.BackgroundProperty, value);
			}
		}

		//public double Scale
		//{
		//	get
		//	{
		//		return (double)this.GetValue(Arrow.ScaleProperty);
		//	}
		//	set
		//	{
		//		this.SetValue(Arrow.ScaleProperty, value);
		//	}
		//}

		protected override Size MeasureOverride(Size availableSize)
		{
			return availableSize;
		}

		protected override void OnRender(DrawingContext dc)
		{
			//double Scale = 1;

			dc.PushTransform(new RotateTransform(Angle,RenderSize.Width/2, RenderSize.Height/2));
			Point p1 = new Point(0.2 * RenderSize.Width, 0.05 * RenderSize.Height);
			Point p2 = new Point(0.8 * RenderSize.Width, 0.50 * RenderSize.Height);
			Point p3 = new Point(0.2 * RenderSize.Width, 0.95 * RenderSize.Height);
			StreamGeometry streamGeometry = new StreamGeometry();
			using (StreamGeometryContext geometryContext = streamGeometry.Open())
			{
				geometryContext.BeginFigure(p1, true, true);
				PointCollection points = new PointCollection { p2, p3 };
				geometryContext.PolyLineTo(points, true, true);
			}

			streamGeometry.Freeze();
			Pen pen = new Pen(Foreground, 1.5);
			dc.DrawGeometry(Background, pen, streamGeometry);
			//dc.DrawLine(_pen, p1, p2);
			//dc.DrawLine(_pen, p2, p3);
			//dc.DrawLine(_pen, p3, p1);

			//Geometry
			//dc.DrawRectangle
			////Point center = new Point(RenderSize.Width / 2, RenderSize.Height / 2);
			//Point cursorPos = new Point(center.X,  center.Y );

			//dc.DrawEllipse(null, _pen, cursorPos, 10, 10);
			//dc.DrawEllipse(null, _pen, cursorPos, 5, 5);

			//Point p1 = new Point(cursorPos.X - 12, cursorPos.Y);
			//Point p2 = new Point(cursorPos.X + 12, cursorPos.Y);
			//dc.DrawLine(_pen, p1, p2);

			//p1 = new Point(cursorPos.X, cursorPos.Y - 12);
			//p2 = new Point(cursorPos.X, cursorPos.Y + 12);
			//dc.DrawLine(_pen, p1, p2);
		}
	}
}
