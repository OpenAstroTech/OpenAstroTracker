using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace OATControl.Converters
{
    public class BoolToResourceConverter : IValueConverter
    {
        public string TrueString { get; set; }
        public string FalseString { get; set; }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var uriSource = "pack://application:,,,/OATControl;component/" + FalseString;
            if (value is bool)
            {
                if ((bool)value == true)
                {
                    uriSource = "pack://application:,,,/OATControl;component/" + TrueString;
                }
            }
            
            return new ImageSourceConverter().ConvertFromString(uriSource) as ImageSource;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    public class BoolToStringConverter : IValueConverter
    {
        public string TrueString { get; set; }
        public string FalseString { get; set; }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool)
            {
                if ((bool)value == true)
                {
                    return TrueString;
                }
            }

            return FalseString;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
