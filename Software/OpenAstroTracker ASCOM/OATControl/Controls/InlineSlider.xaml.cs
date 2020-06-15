
namespace OATControl.Controls
{
	using System;
	using System.Collections.Generic;
	using System.Linq;
	using System.Windows;
	using System.Windows.Controls;
	using System.Windows.Input;
	using System.Windows.Media;
	using System.Windows.Shapes;

	/// <summary>
	/// Interaction logic for InlineSlider.xaml
	/// This is a slider that has a label as well as the current value display inside the slider area. There is 
	/// an Edit button to the right that can be used to enter a value with the keyboard. It also allows the
	/// range to be edited (this can be disabled, too).
	/// 
	/// The typical usage in a XAML file is:
	///           InlineSlider Padding="3" 
	///                 Prompt="Diffuse" 
	///                 Minimum="0" 
	///                 Maximum="1" 
	///                 Value="{Binding Diffuse, Mode=TwoWay}" 
	///                 DecimalPlaces="3" 
	///                 TickStart="0" 
	///                 TickEnd="1" 
	///                 TickMinorSpacing="0.1" 
	///                 TickMajorSpacing="0.5" 
	///                 TickLabels="0|0.25|0.50|0.75|1" 
	///                 IsRangeVisible="False"
	///               
	/// 
	/// - DecimalPlaces determines the granularity of the values as well as the displayed value. A value
	///   of zero indicates an integer slider, negative values indicate powers of ten that the slider
	///   should honor (so e.g. -1 will only allow values that are multiples of 10).
	/// - TickLabels are optional; if they are left off, no labels will be displayed below the slider.
	/// 
	/// </summary>
	public partial class InlineSlider : UserControl
	{
		/// <summary>
		/// The dependency property for the Minimum property
		/// </summary>
		public static readonly DependencyProperty MinimumProperty = DependencyProperty.Register(
			"Minimum",
			typeof(double),
			typeof(InlineSlider),
			new PropertyMetadata(0.0, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the Maximum property
		/// </summary>
		public static readonly DependencyProperty MaximumProperty = DependencyProperty.Register(
			"Maximum",
			typeof(double),
			typeof(InlineSlider),
			new PropertyMetadata(1.0, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the Value property
		/// </summary>
		public static readonly DependencyProperty ValueProperty = DependencyProperty.Register(
			"Value",
			typeof(double),
			typeof(InlineSlider),
			new FrameworkPropertyMetadata(
				0.0,
				FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
				InlineSlider.ValueChanged,
				InlineSlider.CoerceValueCallback));

		/// <summary>
		/// The dependency property for the TickStart property
		/// </summary>
		public static readonly DependencyProperty TickStartProperty = DependencyProperty.Register(
			"TickStart",
			typeof(double),
			typeof(InlineSlider),
			new PropertyMetadata(0.0, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the TickEnd property
		/// </summary>
		public static readonly DependencyProperty TickEndProperty = DependencyProperty.Register(
			"TickEnd",
			typeof(double),
			typeof(InlineSlider),
			new PropertyMetadata(0.0, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the TickMinorSpacing property
		/// </summary>
		public static readonly DependencyProperty TickMinorSpacingProperty = DependencyProperty.Register(
			"TickMinorSpacing",
			typeof(double),
			typeof(InlineSlider),
			new PropertyMetadata(0.0, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the TickMajorSpacing property
		/// </summary>
		public static readonly DependencyProperty TickMajorSpacingProperty = DependencyProperty.Register(
			"TickMajorSpacing",
			typeof(double),
			typeof(InlineSlider),
			new PropertyMetadata(0.0, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the TickLabels property
		/// </summary>
		public static readonly DependencyProperty TickLabelsProperty = DependencyProperty.Register(
			"TickLabels",
			typeof(string),
			typeof(InlineSlider),
			new PropertyMetadata(string.Empty, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the DecimalPlaces property
		/// </summary>
		public static readonly DependencyProperty DecimalPlacesProperty = DependencyProperty.Register(
			"DecimalPlaces",
			typeof(int),
			typeof(InlineSlider),
			new PropertyMetadata(0, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the DecimalPlaces property
		/// </summary>
		public static readonly DependencyProperty IsRangeVisibleProperty = DependencyProperty.Register(
			"IsRangeVisible",
			typeof(bool),
			typeof(InlineSlider),
			new PropertyMetadata(false, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the Locked property
		/// </summary>
		public static readonly DependencyProperty IsLockedProperty = DependencyProperty.Register(
			"IsLocked",
			typeof(bool),
			typeof(InlineSlider),
			new PropertyMetadata(false, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the ShowLock property
		/// </summary>
		public static readonly DependencyProperty ShowLockProperty = DependencyProperty.Register(
			"ShowLock",
			typeof(bool),
			typeof(InlineSlider),
			new PropertyMetadata(false, InlineSlider.SliderPropertyChanged));

		/// <summary>
		/// The dependency property for the Prompt property
		/// </summary>
		public static readonly DependencyProperty PromptProperty = DependencyProperty.Register(
			"Prompt",
			typeof(string),
			typeof(InlineSlider),
			new PropertyMetadata(string.Empty));

		/// <summary>
		/// The set part width property key (read-only dependancy property)
		/// </summary>
		private static readonly DependencyPropertyKey SetPartWidthPropertyKey = DependencyProperty.RegisterReadOnly(
			"SetPartWidth",
			typeof(string),
			typeof(InlineSlider),
			new PropertyMetadata("0"));

		/// <summary>
		/// The set part width property (read-only dependancy property)
		/// </summary>
		public static readonly DependencyProperty SetPartWidthProperty = InlineSlider.SetPartWidthPropertyKey.DependencyProperty;

		/// <summary>
		/// The unset part width property key (read-only dependancy property)
		/// </summary>
		private static readonly DependencyPropertyKey UnsetPartWidthPropertyKey = DependencyProperty.RegisterReadOnly(
			"UnsetPartWidth",
			typeof(string),
			typeof(InlineSlider),
			new PropertyMetadata("*"));

		/// <summary>
		/// The unset part width property (read-only dependancy property)
		/// </summary>
		public static readonly DependencyProperty UnsetPartWidthProperty = InlineSlider.UnsetPartWidthPropertyKey.DependencyProperty;

		/// <summary>
		/// The label row height property key (read-only dependancy property)
		/// </summary>
		private static readonly DependencyPropertyKey LabelRowHeightPropertyKey = DependencyProperty.RegisterReadOnly(
			"LabelRowHeight",
			typeof(string),
			typeof(InlineSlider),
			new PropertyMetadata("0"));

		/// <summary>
		/// The label row height property (read-only dependancy property)
		/// </summary>
		public static readonly DependencyProperty LabelRowHeightProperty = InlineSlider.LabelRowHeightPropertyKey.DependencyProperty;

		private readonly List<Tuple<double, string>> labelList = new List<Tuple<double, string>>();
		private double rounder = 1.0;
		private string stringFormat = "0";

		/// <summary>
		/// Initializes a new instance of the <see cref="InlineSlider"/> class.
		/// </summary>
		public InlineSlider()
		{
			this.InitializeComponent();
			this.IsRangeVisible = false;
		}

		/// <summary>
		/// Gets or sets the tick start location. 
		/// Minor ticks are drawn starting at this value, increasing by TickMinorSpacing until we are past TickEnd. 
		/// Major ticks are drawn starting at this value, increasing by TickMajorSpacing until we are past TickEnd. 
		/// Major ticks take precedence over minor ticks.
		/// Only ticks that are within Minimum and Maximum (inclusive) are drawn.
		/// </summary>
		public double TickStart
		{
			get
			{
				return (double)this.GetValue(InlineSlider.TickStartProperty);
			}
			set
			{
				this.SetValue(InlineSlider.TickStartProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the tick end location. 
		/// Minor ticks are drawn starting at TickStart, increasing by TickMinorSpacing until we are past this value. 
		/// Major ticks are drawn starting at TickStart, increasing by TickMajorSpacing until we are past this value. 
		/// Major ticks take precedence over minor ticks.
		/// Only ticks that are within Minimum and Maximum (inclusive) are drawn.
		/// </summary>
		public double TickEnd
		{
			get
			{
				return (double)this.GetValue(InlineSlider.TickEndProperty);
			}
			set
			{
				this.SetValue(InlineSlider.TickEndProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the minor tick spacing. 
		/// Minor ticks are drawn starting at TickStart, increasing by this value until we are past TickEnd. 
		/// Only ticks that are within Minimum and Maximum (inclusive) are drawn.
		/// </summary>
		public double TickMinorSpacing
		{
			get
			{
				return (double)this.GetValue(InlineSlider.TickMinorSpacingProperty);
			}
			set
			{
				this.SetValue(InlineSlider.TickMinorSpacingProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the major tick spacing. 
		/// Major ticks are drawn starting at TickStart, increasing by this value until we are past TickEnd. 
		/// Only ticks that are within Minimum and Maximum (inclusive) are drawn.
		/// </summary>
		public double TickMajorSpacing
		{
			get
			{
				return (double)this.GetValue(InlineSlider.TickMajorSpacingProperty);
			}
			set
			{
				this.SetValue(InlineSlider.TickMajorSpacingProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the tick labels. This is a string with pipe-delimited labels. 
		/// Each label should be either a number, or a number followed by a comma and a label.
		/// If only a number is found the number is rendered at the location indicated by it.
		/// If a label is also present, the label is rendered at the location indicated by the number.
		/// </summary>
		public string TickLabels
		{
			get
			{
				return (string)this.GetValue(InlineSlider.TickLabelsProperty);
			}
			set
			{
				this.SetValue(InlineSlider.TickLabelsProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the current value of the slider.
		/// </summary>
		public double Value
		{
			get
			{
				return (double)this.GetValue(InlineSlider.ValueProperty);
			}
			set
			{
				this.SetValue(InlineSlider.ValueProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the minimum value of the slider range.
		/// </summary>
		public double Minimum
		{
			get
			{
				return (double)this.GetValue(InlineSlider.MinimumProperty);
			}
			set
			{
				this.SetValue(InlineSlider.MinimumProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the maximum value of the slider range.
		/// </summary>
		public double Maximum
		{
			get
			{
				return (double)this.GetValue(InlineSlider.MaximumProperty);
			}
			set
			{
				this.SetValue(InlineSlider.MaximumProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the text of the prompt displayed inside slider on the left.
		/// </summary>
		public string Prompt
		{
			get
			{
				return (string)this.GetValue(InlineSlider.PromptProperty);
			}
			set
			{
				this.SetValue(InlineSlider.PromptProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets a flag indicating whether the Range textboxes are visible.
		/// </summary>
		public bool IsRangeVisible
		{
			get
			{
				return (bool)this.GetValue(InlineSlider.IsRangeVisibleProperty);
			}
			set
			{
				this.SetValue(InlineSlider.IsRangeVisibleProperty, value);
			}
		}

		/// <summary>
		/// Gets or sets the number of decimal places the slider displays. This is used 
		/// to round the value of the slider and to determine the format of the string to display on the right.
		/// </summary>
		public int DecimalPlaces
		{
			get
			{
				return (int)this.GetValue(InlineSlider.DecimalPlacesProperty);
			}
			set
			{
				this.SetValue(InlineSlider.DecimalPlacesProperty, value);
			}
		}
		SolidColorBrush _tickBrush;
		/// <summary>
		/// Called when the <see cref="E:System.Windows.FrameworkElement.SizeChanged" /> event is raised. Used to regenerate the ticks and labels.
		/// </summary>
		/// <param name="sizeInfo">Details of the old and new size involved in the change.</param>
		protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
		{
			_tickBrush = new SolidColorBrush(Color.FromArgb(70, 255, 255, 255)); // Lighten
			base.OnRenderSizeChanged(sizeInfo);
			this.RecalculateLabelsAndTicks();
		}

		/// <summary>
		/// Parses the labels property for the list of labels to render.
		/// </summary>
		private void ParseLabels()
		{
			var labelPairs = this.TickLabels.Split("|".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
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

			this.SetValue(InlineSlider.LabelRowHeightPropertyKey, this.labelList.Any() ? "9" : "0");
		}

		/// <summary>
		/// Calculates the widths of the set and unset parts of the slider.
		/// </summary>
		private void CalculatePartWidths()
		{
			string setPart, unsetPart;
			if ((this.ActualWidth == 0) || (this.ValueTextBox == null) || !this.ValueTextBox.IsReadOnly)
			{
				setPart = "0";
				unsetPart = "*";
			}
			else
			{
				setPart = string.Format("{0}*", Math.Max(0, (int)(this.ActualWidth * (this.Value - this.Minimum) / (this.Maximum - this.Minimum))));
				unsetPart = string.Format("{0}*", Math.Max(0, (int)(this.ActualWidth * (this.Maximum - this.Value) / (this.Maximum - this.Minimum))));
			}

			this.SetValue(InlineSlider.SetPartWidthPropertyKey, setPart);
			this.SetValue(InlineSlider.UnsetPartWidthPropertyKey, unsetPart);
		}

		/// <summary>
		/// Recalculates the label and tick lists.
		/// </summary>
		private void RecalculateLabelsAndTicks()
		{
			this.TickCanvas.Children.Clear();
			this.TickLabelCanvas.Children.Clear();


			if ((this.TickStart != this.TickEnd) && (this.Minimum != this.Maximum))
			{
				var lines = new Dictionary<string, Line>();

				this.InsertMinorTicks(lines);
				this.InsertMajorTicks(lines);

				// Now add the resulting list to the canvas.
				foreach (Line line in lines.Values)
				{
					this.TickCanvas.Children.Add(line);
				}
			}

			// Check whether there are any labels to add to this control
			if (this.labelList.Any() && (this.Minimum != this.Maximum))
			{
				this.InsertTickLabels();
			}

			// Update the slider position.
			this.CalculatePartWidths();
		}

		/// <summary>
		/// Adds the tick labels to the canvas of the control.
		/// </summary>
		private void InsertTickLabels()
		{
			const double LabelFontSize = 8;
			foreach (var label in this.labelList)
			{
				var textBlock = new TextBlock
				{
					Text = label.Item2,
					FontSize = LabelFontSize,
					Foreground = _tickBrush,
					Background = Brushes.Transparent,
					HorizontalAlignment = HorizontalAlignment.Center,
					VerticalAlignment = VerticalAlignment.Top,
					Margin = new Thickness(0.0)
				};

				// Calculate the normalized (0..1) horizontal location of the label (which is centered below this)
				double location = (label.Item1 - this.Minimum) / (this.Maximum - this.Minimum);

				// Scale to the actual width of the grid, but place it 1.5pixels inside the range..
				location = 1.5 + location * (this.MainGrid.ColumnDefinitions[0].ActualWidth - 3.0);

				this.TickLabelCanvas.Children.Add(textBlock);

				// Assume an average 0.7 width-to-height ratio.
				const double AverageWidthPerCharacter = LabelFontSize * 0.7;

				// Now position the label accordingly
				textBlock.SetValue(Canvas.LeftProperty, location - (AverageWidthPerCharacter * label.Item2.Length) / 2.0);
				textBlock.SetValue(Canvas.TopProperty, 0.0);
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
			const int MajorTickLength = 4;

			if (this.TickMajorSpacing > 0)
			{
				for (double start = this.TickStart; start <= this.TickEnd; start += this.TickMajorSpacing)
				{
					var key = start.ToString(this.stringFormat);
					double location = (start - this.Minimum) / (this.Maximum - this.Minimum);
					if ((location >= 0.0) && (location <= 1.0))
					{
						// We scale the axis so that the minimum and maximum are just inside the control.
						location = 1.5 + location * (this.MainGrid.ColumnDefinitions[0].ActualWidth - 3.0);
						Line line = new Line
						{
							X1 = location,
							X2 = location,
							Y1 = this.MainGrid.RowDefinitions[0].ActualHeight - 1,
							Y2 = this.MainGrid.RowDefinitions[0].ActualHeight - MajorTickLength,
							Stroke = _tickBrush,
							SnapsToDevicePixels = true
						};

						lines[key] = line;
					}
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
			const int MinorTickLength = 2;

			// Minor tick lines
			if (this.TickMinorSpacing > 0)
			{
				for (double start = this.TickStart; start <= this.TickEnd; start += this.TickMinorSpacing)
				{
					var key = start.ToString(this.stringFormat);
					var location = (start - this.Minimum) / (this.Maximum - this.Minimum);
					if ((location >= 0.0) && (location <= 1.0))
					{
						// We scale the axis so that the minimum and maximum are just inside the control.
						location = 1.5 + location * (this.MainGrid.ColumnDefinitions[0].ActualWidth - 3.0);
						Line line = new Line
						{
							X1 = location,
							X2 = location,
							Y1 = this.MainGrid.RowDefinitions[0].ActualHeight - 1,
							Y2 = this.MainGrid.RowDefinitions[0].ActualHeight - MinorTickLength,
							Stroke = _tickBrush,
							SnapsToDevicePixels = true
						};
						lines[key] = line;
					}
				}
			}
		}

		/// <summary>
		/// Called by the dependency property when any of the slider properties have changed.
		/// </summary>
		/// <param name="obj">The slider object that caused the change.</param>
		/// <param name="e">The <see cref="DependencyPropertyChangedEventArgs"/> instance containing the event data.</param>
		private static void SliderPropertyChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			bool callLabelRecalc = true;
			InlineSlider slider = obj as InlineSlider;

			switch (e.Property.Name)
			{
				case "Maximum":
				case "Minimum":
				case "TickStart":
				case "TickEnd":
				case "TickMinorSpacing":
				case "TickMajorSpacing":
					// No extra processing neccessary
					break;

				case "TickLabels":
					slider.ParseLabels();
					break;

				case "DecimalPlaces":
					int places = slider.DecimalPlaces;
					slider.stringFormat = "0";
					slider.rounder = (float)Math.Pow(10, places);
					if (places > 0)
					{
						slider.stringFormat += "." + new string('0', places);
					}

					slider.ValueTextBox.Text = slider.Value.ToString(slider.stringFormat);
					break;

				case "IsRangeVisible":
				case "IsLocked":
				case "ShowLock":
					callLabelRecalc = false;
					break;


				default:
					callLabelRecalc = false;
					break;
			}

			if (callLabelRecalc)
			{
				slider.RecalculateLabelsAndTicks();
			}
		}

		/// <summary>
		/// Called by the dependency property when the Value has changed.
		/// </summary>
		/// <param name="obj">The object.</param>
		/// <param name="e">The <see cref="DependencyPropertyChangedEventArgs"/> instance containing the event data.</param>
		private static void ValueChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
		{
			InlineSlider slider = obj as InlineSlider;
			if (slider == null)
			{
				return;
			}

			slider.ValueTextBox.Text = slider.Value.ToString(slider.stringFormat);
			slider.CalculatePartWidths();
		}

		/// <summary>
		/// Coerces the given value to a valid range.
		/// </summary>
		/// <param name="dependencyObject">The dependency object.</param>
		/// <param name="baseValue">The base value.</param>
		/// <returns></returns>
		private static object CoerceValueCallback(DependencyObject dependencyObject, object baseValue)
		{
			InlineSlider slider = dependencyObject as InlineSlider;
			if (slider == null)
			{
				return null;
			}

			var newVal = (double)baseValue;

			// Check if we need to round it
			if (slider.rounder != 0.0)
			{
				newVal = Math.Round(slider.rounder * newVal) / slider.rounder;
			}

			// Make sure it's within our defined range
			newVal = Math.Min(Math.Max(slider.Minimum, newVal), slider.Maximum);

			return newVal;
		}

		/// <summary>
		/// Handles the OnClick event of the Edit and Lock Button control. 
		/// For a click on the Edit button, it enables the textbox, sets the focus to it and selects all the text.
		/// For a click on the Lock button, it toggles the locked range state. 
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="RoutedEventArgs"/> instance containing the event data.</param>
		private void ButtonBase_OnClick(object sender, RoutedEventArgs e)
		{
			if (sender == this.ButtonEdit)
			{
				this.ValueTextBox.IsReadOnly = false;
				this.ValueTextBox.Focusable = true;
				this.ValueTextBox.Focus();
				this.ValueTextBox.SelectAll();
				this.CalculatePartWidths();
			}
		}

		/// <summary>
		/// Handles the OnKeyDown event of the ValueTextBox control. It looks for return in order to set the value textbox back to read only.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="KeyEventArgs"/> instance containing the event data.</param>
		private void ValueTextBox_OnKeyDown(object sender, KeyEventArgs e)
		{
			if ((e.Key == Key.Return) || (e.Key == Key.Tab))
			{
				this.ReleaseTextboxFocus();
			}
		}

		/// <summary>
		/// Called when the textbox should lose focus.
		/// </summary>
		private void ReleaseTextboxFocus()
		{
			// Move to a parent that can take focus
			FrameworkElement parent = (FrameworkElement)this.ValueTextBox.Parent;
			while (parent != null && parent is IInputElement && !((IInputElement)parent).Focusable)
			{
				parent = (FrameworkElement)parent.Parent;
			}

			DependencyObject scope = FocusManager.GetFocusScope(this.ValueTextBox);
			FocusManager.SetFocusedElement(scope, parent);
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
				if (!this.ValueTextBox.IsReadOnly)
				{
					this.ReleaseTextboxFocus();
				}

				this.MainGrid.CaptureMouse();
				//this.SetValueFromMouse(e);
			}
		}

		/// <summary>
		/// Sets the value from mouse location.
		/// </summary>
		/// <param name="e">The <see cref="MouseEventArgs"/> instance containing the event data.</param>
		private void SetValueFromMouse(MouseEventArgs e)
		{
			Point pt = e.GetPosition(this.MainGrid);

			double normalized = 1.0 * pt.X / this.MainGrid.ColumnDefinitions[0].ActualWidth;
			double clickValue = this.Minimum + (float)Math.Min(1.0, Math.Max(0, normalized)) * (this.Maximum - this.Minimum);

			// If slider is configure for integers, support increment and decrement when Shit is pressed.
			if (DecimalPlaces == 0 && (Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift)))
			{
				this.Value = Math.Min(Maximum, Math.Max(Minimum, (clickValue > Value) ? this.Value + 1 : this.Value - 1));
			}
			else
			{
				this.Value = clickValue;
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
		/// Handles the OnLostFocus event of the ValueTextBox control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="RoutedEventArgs"/> instance containing the event data.</param>
		private void ValueTextBox_OnLostFocus(object sender, RoutedEventArgs e)
		{
			this.ValueTextBox.IsReadOnly = true;
			this.ValueTextBox.Focusable = false;
			float newVal;
			if (float.TryParse(this.ValueTextBox.Text, out newVal))
			{
				this.Value = newVal;
			}

			this.CalculatePartWidths();
		}

		/// <summary>
		/// Handles the OnSizeChanged event of the control.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">The <see cref="SizeChangedEventArgs"/> instance containing the event data.</param>
		private void TrackPart_OnSizeChanged(object sender, SizeChangedEventArgs e)
		{
			this.RecalculateLabelsAndTicks();
		}

		static byte ParseHexByte(string s)
		{
			return (byte)((s[0] <= '9' ? s[0] - '0' : 10 + s[0] - 'A') * 16 + (s[1] <= '9' ? s[1] - '0' : 10 + s[1] - 'A') * 16);
		}

		public static Color ParseColor(string s)
		{
			string a = s.Substring(1, 2);
			string r = s.Substring(3, 2);
			string g = s.Substring(5, 2);
			string b = s.Substring(7, 2);
			return Color.FromArgb(ParseHexByte(a), ParseHexByte(r), ParseHexByte(g), ParseHexByte(b));
		}

		public Color[] Colors
		{
			get
			{
				return new Color[] {
					ParseColor("#FFF8F818"),
					ParseColor("#FF202020"),
					ParseColor("#FF505050"),
					ParseColor("#FF404040"),
					ParseColor("#FF404040"),
					ParseColor("#FF303030"),
					ParseColor("#FF707070"),
					ParseColor("#FF404040"),
					ParseColor("#FF808080"),
					ParseColor("#FF606060"),
					ParseColor("#FFA0FFA0"),
					ParseColor("#FF70FF70"),
					ParseColor("#FF181818"),
					ParseColor("#FFFFFFFF"),
					ParseColor("#FF909090"),
					ParseColor("#FF90FF90"),
				};
			}
		}
	}
}