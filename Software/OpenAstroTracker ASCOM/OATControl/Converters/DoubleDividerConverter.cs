using System;
using System.Globalization;
using System.Windows.Data;

namespace OATControl.Converters
{
    public class DoubleDividerConverter : IValueConverter
    {
        public DoubleDividerConverter()
        {
            Divisor = 2.0;
            Addition = 0.0;
        }

        public double Divisor { get; set; }

        public double Addition { get; set; }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is double)
            {
                return ((double)value) / Divisor;
            }
            return 0;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
