using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace OATControl.Converters
{
    public class StretchModeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            Stretch mode;
            if (Enum.TryParse<System.Windows.Media.Stretch>((string)value, out mode))
            {
                return mode;
            }

            return Stretch.None;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
