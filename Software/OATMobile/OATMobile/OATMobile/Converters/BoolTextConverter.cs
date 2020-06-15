using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;
using Xamarin.Forms;

namespace OATMobile.Converters
{
    class BoolTextConverter : IValueConverter {
        public string TrueValue {get;set; } = "True";
        public string FalseValue { get; set; } = "False";
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture) {
            if (value == null) {
                return FalseValue;
            }

            return ((bool) value) ? TrueValue : FalseValue;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture) {
            throw new NotImplementedException();
        }
    }
}
