using System;
using System.Windows.Data;
using System.Windows.Markup;

namespace OATControl.Converters
{

    /// <summary>
    /// Data binding helper.
    /// Converts a binding from boolean to an integer value.
    /// Returns FalseValue when bound value is false and TrueValue when value is ture.
    /// </summary>
    public class IntToBoolConverter : MarkupExtension, IValueConverter
    {
        /// <summary>
        /// Gets or sets the integer value to return on the given boolean value.
        /// </summary>
        public int TrueValue { get; set; }
        public string Operator { get; set; } = "";

        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value produced by the binding source.</param>
        /// <param name="targetType">The type of the binding target property.</param>
        /// <param name="parameter">The converter parameter to use.</param>
        /// <param name="culture">The culture to use in the converter.</param>
        /// <returns>
        /// A converted value. If the method returns null, the valid null value is used.
        /// </returns>
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (Operator.Equals(">"))
            {
                return ((int)value > this.TrueValue);
            }
            if (Operator.Equals(">="))
            {
                return ((int)value >= this.TrueValue);
            }
            if (Operator.Equals("<"))
            {
                return ((int)value < this.TrueValue);
            }
            if (Operator.Equals("<="))
            {
                return ((int)value <= this.TrueValue);
            }
            if (Operator.Equals("=="))
            {
                return ((int)value == this.TrueValue);
            }
            if (Operator.Equals("!="))
            {
                return ((int)value != this.TrueValue);
            }
            return false;
        }

        /// <summary>
        /// Converts a value back. Not implemented
        /// </summary>
        /// <param name="value">The value that is produced by the binding target.</param>
        /// <param name="targetType">The type to convert to.</param>
        /// <param name="parameter">The converter parameter to use.</param>
        /// <param name="culture">The culture to use in the converter.</param>
        /// <returns>
        /// A converted value. If the method returns null, the valid null value is used.
        /// </returns>
        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (Operator.Equals(">"))
            {
                return this.TrueValue+1;
            }
            if (Operator.Equals(">="))
            {
                return this.TrueValue + 1;
            }
            if (Operator.Equals("<"))
            {
                return this.TrueValue - 1;
            }
            if (Operator.Equals("<="))
            {
                return this.TrueValue - 1;
            }
            if (Operator.Equals("=="))
            {
                return this.TrueValue ;
            }
            if (Operator.Equals("!="))
            {
                return this.TrueValue + 21;
            }
            return 0;
        }

        /// <summary>
        /// When implemented in a derived class, returns an object that is provided as the value of the target property for this markup extension.
        /// </summary>
        /// <param name="serviceProvider">A service provider helper that can provide services for the markup extension.</param>
        /// <returns>
        /// The object value to set on the property where the extension is applied.
        /// </returns>
        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this;
        }
    }
}