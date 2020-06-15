using System;
using System.Globalization;
using System.Windows;
using System.Windows.Media;

namespace OATControl.Controls
{
	public class ScopeCircles : FrameworkElement
	{
		private Pen _pen;
		private Brush _brush;

		public Brush Foreground
		{
			get { return _brush; }
			set
			{
				_brush = value;
				_pen = new Pen(_brush, 1);
			}
		}

		public bool DrawLabels { get; set; }

		public double DeltaAngle { get; set; }

		public int NumCircles { get; set; }

		protected override void OnRender(DrawingContext dc)
		{
			int steps = (int)(360 / DeltaAngle);
			Point center = new Point(RenderSize.Width / 2, RenderSize.Height / 2);

			int maxRadius = (int)Math.Floor(Math.Min(RenderSize.Width / 2, RenderSize.Height / 2));

			//dc.DrawEllipse(null, _pen, center, maxRadius, maxRadius);

			for (int i = 0; i < steps; i++)
			{
				double angle = i * DeltaAngle;
				Point p1 = GetPoint(angle, null, maxRadius);

				// Lines 
				dc.DrawLine(_pen, center, p1);
				double radius = RenderSize.Width * 0.5 * i / steps;

				if (DrawLabels)
				{
					// Text
					FormattedText text = new FormattedText("" + i * DeltaAngle, CultureInfo.InvariantCulture,  FlowDirection.LeftToRight, new Typeface("Verdana"), 12, Foreground);
					Point p2 = GetPoint(angle, text, maxRadius-16);
					p2.X -= text.Width / 2;
					p2.Y -= text.Height / 2;
					dc.DrawText(text, p2);
				}
			}

			// Circles
			for (int i = 1; i <= NumCircles ; i++)
			{
				double radius = maxRadius * i / NumCircles;
				dc.DrawEllipse(null, _pen, center, radius, radius);
			}
		}

		private Point GetPoint(double angle, FormattedText text, double radius)
		{
			Point center = new Point(RenderSize.Width / 2, RenderSize.Height / 2);
			double radAngle = angle * Math.PI / 180;

			if (text != null)
			{
				radius += Math.Max(text.Width/2, text.Height/2);
			}

			double x = center.X + radius * Math.Cos(radAngle);
			double y = center.Y - radius * Math.Sin(radAngle);
			return new Point(x, y);
		}
	}
}
