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
	/// Interaction logic for LevelDisplay.xaml
	/// </summary>
	public partial class LevelDisplay : UserControl
	{
		/// <summary>
		/// The dependency property for the Minimum property
		/// </summary>
		public static readonly DependencyProperty RangeProperty = DependencyProperty.Register(
			"Range",
			typeof(double),
			typeof(LevelDisplay),
			new PropertyMetadata(4.0, LevelDisplay.LevelDisplayPropertyChanged));

		/// <summary>
		/// The dependency property for the Value property
		/// </summary>
		public static readonly DependencyProperty ValueProperty = DependencyProperty.Register(
			"Value",
			typeof(double),
			typeof(LevelDisplay),
			new FrameworkPropertyMetadata(
				0.0,
				FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
				LevelDisplay.ValueChanged,
				LevelDisplay.CoerceValueCallback));

		/// <summary>
		/// The dependency property for the Labels property
		/// </summary>
		public static readonly DependencyProperty LabelsProperty = DependencyProperty.Register(
			"Labels",
			typeof(string),
			typeof(LevelDisplay),
			new PropertyMetadata(string.Empty, LevelDisplay.LevelDisplayPropertyChanged));

		/// <summary>
		/// The set part width property key (read-only dependancy property)
		/// </summary>
		private static readonly DependencyPropertyKey PixelValuePropertyKey = DependencyProperty.RegisterReadOnly(
			"PixelValue",
			typeof(double),
			typeof(LevelDisplay),
			new PropertyMetadata(0.0));

		private readonly List<Tuple<double, string>> labelList = new List<Tuple<double, string>>();
		private double rounder = 100.0;
		private string stringFormat = "0.00";

		public LevelDisplay()
		{
			InitializeComponent();
		}

		private void ParseLabels()
		{
			var labelPairs = this.Labels.Split("|".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
			this.labelList.Clear();
			foreach (var pair in labelPairs)
			{
				var label = pair.Split(",".ToCharArray());
				var stringIndex = (label.Length == 1) ? 0 : 1;
				double location;
				if (double.TryParse(label[0], out location))
				{
					this.labelList.Add(Tuple.Create(location, label[stringIndex]));
				}
			}

		}

		/// <summary>
		/// Gets or sets the minimum value of the slider range.
		/// </summary>
		public double Range
		{
			get
			{
				return (double)this.GetValue(LevelDisplay.RangeProperty);
			}
			set
			{
				this.SetValue(LevelDisplay.RangeProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the current value of the slider.
		/// </summary>
		public double Value
		{
			get
			{
				return (double)this.GetValue(LevelDisplay.ValueProperty);
			}
			set
			{
				this.SetValue(LevelDisplay.ValueProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the current value of the slider.
		/// </summary>
		public string Labels
		{
			get
			{
				return (string)this.GetValue(LevelDisplay.LabelsProperty);
			}
			set
			{
				this.SetValue(LevelDisplay.LabelsProperty, value);
			}
		}

		/// <summary>
		/// Called by the dependency property when any of the slider properties have changed.
		/// </summary>
		/// <param name="obj">The slider object that caused the change.</param>
		/// <param name="e">The <see cref="DependencyPropertyChangedEventArgs"/> instance containing the event data.</param>
		private static void LevelDisplayPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			LevelDisplay level = obj as LevelDisplay;

			switch (e.Property.Name)
			{
				case "Labels":
					level.ParseLabels();
					level.RecalculateTicks();
					break;
			}
		}

		/// <summary>
		/// Called by the dependency property when the Value has changed.
		/// </summary>
		/// <param name="obj">The object.</param>
		/// <param name="e">The <see cref="DependencyPropertyChangedEventArgs"/> instance containing the event data.</param>
		private static void ValueChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			LevelDisplay level = obj as LevelDisplay;
			if (level == null)
			{
				return;
			}

			level.ValueTextBox.Text = Math.Abs(level.Value).ToString(level.stringFormat);
			level.SetValue(LevelDisplay.PixelValuePropertyKey, level.PixelValue);
		}

		public bool GoodRange
		{
			get
			{
				return Math.Abs(Value) < 0.2;
			}
		}

		/// <summary>
		/// Coerces the given value to a valid range.
		/// </summary>
		/// <param name="dependencyObject">The dependency object.</param>
		/// <param name="baseValue">The base value.</param>
		/// <returns></returns>
		private static object CoerceValueCallback(DependencyObject dependencyObject, object baseValue)
		{
			LevelDisplay level = dependencyObject as LevelDisplay;
			if (level == null)
			{
				return null;
			}

			var newVal = (double)baseValue;

			// Check if we need to round it
			if (level.rounder != 0.0)
			{
				newVal = Math.Round(level.rounder * newVal) / level.rounder;
			}

			// Make sure it's within our defined range
			newVal = Math.Min(Math.Max(-level.Range, newVal), level.Range);

			return newVal;
		}

		void RecalculateTicks()
		{
			this.TickCanvas.Children.Clear();
			this.TickLabelCanvas.Children.Clear();

			var lines = new Dictionary<string, Line>();

			this.InsertMinorTicks(lines);
			this.InsertMajorTicks(lines);

			// Now add the resulting list to the canvas.
			foreach (Line line in lines.Values)
			{
				this.TickCanvas.Children.Add(line);
			}

			this.InsertLabels();
		}


		/// <summary>
		/// Called when the <see cref="E:System.Windows.FrameworkElement.SizeChanged" /> event is raised. Used to regenerate the ticks and labels.
		/// </summary>
		/// <param name="sizeInfo">Details of the old and new size involved in the change.</param>
		protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
		{
			base.OnRenderSizeChanged(sizeInfo);
			this.RecalculateTicks();
		}

		public double PixelValue
		{
			get
			{
				double location = 0.5 + Value / (this.Range * 2);
				location = Math.Min(1, Math.Max(0, location));
				return 1.5 + location * (this.MainGrid.ActualWidth - 3.0);
			}
		}

		/// <summary>
		/// Inserts the major ticks into the given dictionary. The key for this dictionary is the string representation
		/// of the location on the axis. This is used to make sure major ticks overwrite minor ticks and don't
		/// introduce floating point errors when comparing.
		/// </summary>
		/// <param name="lines">The dictionary containing the minor and major tick lines.</param>
		private void InsertMajorTicks(Dictionary<string, Line> lines)
		{
			const int MajorTickLength = 6;

			for (double start = 0; start <= this.Range; start += 1.0)
			{
				var key = start.ToString(this.stringFormat);
				double location = 0.5 + start / (this.Range * 2);
				if ((location >= 0.0) && (location <= 1.0))
				{
					// We scale the axis so that the minimum and maximum are just inside the control.
					var pixelLocation = 1.5 + location * (this.MainGrid.ActualWidth - 3.0);
					Line line = new Line
					{
						X1 = pixelLocation,
						X2 = pixelLocation,
						Y1 = this.MainGrid.RowDefinitions[0].ActualHeight - 1,
						Y2 = this.MainGrid.RowDefinitions[0].ActualHeight - MajorTickLength,
						Stroke = Brushes.Red,
						SnapsToDevicePixels = true
					};

					lines[key] = line;

					location = 0.5 - start / (this.Range * 2);
					pixelLocation = 1.5 + location * (this.MainGrid.ActualWidth - 3.0);


					Line line2 = new Line
					{
						X1 = pixelLocation,
						X2 = pixelLocation,
						Y1 = this.MainGrid.RowDefinitions[0].ActualHeight - 1,
						Y2 = this.MainGrid.RowDefinitions[0].ActualHeight - MajorTickLength,
						Stroke = Brushes.Red,
						SnapsToDevicePixels = true
					};
					lines["-" + key] = line2;
				}
			}
		}

		/// <summary>
		/// Inserts the minor ticks into the given dictionary. The key for this dictionary is the string representation
		/// of the location on the axis. This is used to make sure major ticks overwrite minor ticks and don't
		/// introduce floating point errors when comparing.
		/// </summary>
		/// <param name="lines">The dictionary containing the minor and major tick lines.</param>
		private void InsertMinorTicks(Dictionary<string, Line> lines)
		{
			const int MinorTickLength = 3;

			// Minor tick lines
			for (double start = 0; start <= this.Range; start += 0.1)
			{
				var key = start.ToString(this.stringFormat);
				double location = 0.5 + start / (this.Range * 2);
				if ((location >= 0.0) && (location <= 1.0))
				{
					// We scale the axis so that the minimum and maximum are just inside the control.
					var pixelLocation = 1.5 + location * (this.MainGrid.ActualWidth - 3.0);
					Line line = new Line
					{
						X1 = pixelLocation,
						X2 = pixelLocation,
						Y1 = this.MainGrid.RowDefinitions[0].ActualHeight - 1,
						Y2 = this.MainGrid.RowDefinitions[0].ActualHeight - MinorTickLength,
						Stroke = Brushes.DarkRed,
						SnapsToDevicePixels = true
					};
					lines[key] = line;

					location = 0.5 - start / (this.Range * 2);
					pixelLocation = 1.5 + location * (this.MainGrid.ActualWidth - 3.0);

					Line line2 = new Line
					{
						X1 = pixelLocation,
						X2 = pixelLocation,
						Y1 = this.MainGrid.RowDefinitions[0].ActualHeight - 1,
						Y2 = this.MainGrid.RowDefinitions[0].ActualHeight - MinorTickLength,
						Stroke = Brushes.DarkRed,
						SnapsToDevicePixels = true
					};
					lines["-" + key] = line2;
				}
			}
		}

		private void InsertLabels()
		{
			const double LabelFontSize = 12;
			foreach (var label in this.labelList)
			{
				var textBlock = new TextBlock
				{
					Text = label.Item2,
					FontSize = LabelFontSize,
					Foreground = Brushes.DarkRed,
					Background = Brushes.Transparent,
					HorizontalAlignment = HorizontalAlignment.Center,
					VerticalAlignment = VerticalAlignment.Top,
					Margin = new Thickness(0.0)
				};

				// Calculate the normalized (0..1) horizontal location of the label (which is centered below this)
				double location = 0.5 + label.Item1 / (this.Range * 2);

				// Scale to the actual width of the grid, but place it 1.5pixels inside the range..
				location = 1.5 + location * (this.MainGrid.ActualWidth - 3.0);

				this.TickLabelCanvas.Children.Add(textBlock);

				// Assume an average 0.7 width-to-height ratio.
				const double AverageWidthPerCharacter = LabelFontSize * 0.7;

				// Now position the label accordingly
				textBlock.SetValue(Canvas.LeftProperty, location - (AverageWidthPerCharacter * label.Item2.Length) / 2.0);
				textBlock.SetValue(Canvas.TopProperty, 0.0);
			}
		}
	}
}
