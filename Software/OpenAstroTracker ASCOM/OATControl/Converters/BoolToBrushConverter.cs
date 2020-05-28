using System;
using System.Windows;
using System.Windows.Data;
using System.Windows.Markup;
using System.Windows.Media;

namespace OATControl.Converters
{
    /// <summary>
    /// Data binding helper.
    /// Converts a binding from a nullable boolean to a Brush value.
    /// </summary>
    public class BoolToBrushConverter : MarkupExtension, IValueConverter
    {
        /// <summary>
        /// Gets or sets the brush to use when the value is true.
        /// </summary>
        public Brush TrueBrush { get; set; }

        /// <summary>
        /// Gets or sets the brush to use when the value is false.
        /// </summary>
        public Brush FalseBrush { get; set; }

        /// <summary>
        /// Gets or sets the brush to use when the value is null.
        /// </summary>
        public Brush NullBrush { get; set; }

        public BoolToBrushConverter()
        {
            this.NullBrush = Brushes.Transparent;
        }

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            bool? v = value as bool?;
            if (v == null)
            {
                return this.NullBrush;
            }

            return (bool)value ? TrueBrush : FalseBrush;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this;
        }
    }
}