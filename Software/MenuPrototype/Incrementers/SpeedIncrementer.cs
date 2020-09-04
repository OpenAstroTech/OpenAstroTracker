using System;

namespace MenuPrototype
{
	partial class Program
	{
		public class SpeedIncrementer : Incrementer
		{
			int _speed = 0; // Come from mount
			int _ra = 0;
			int _dec = 0;
			int _back = 0;

			public override void onChange(string tag, int[] numbers, int index, int val)
			{
				base.onChange(tag, numbers, index, val);
				if (tag == "SPD")
				{
					_speed = numbers[0];
				}
				else if (tag == "RA")
				{
					_ra = numbers[0];
				}
				else if (tag == "DEC")
				{
					_dec = numbers[0];
				}
				else if (tag == "BACK")
				{
					_back = numbers[0];
				}
			}

			public override string getDisplay(string tag, int index, int val, string formatString = null)
			{
				if (tag == "SPD")
				{
					return string.Format("{0:0.0000}", 1.0 + 1.0 * val / 10000.0);
				}
				return val.ToString();
			}

			public override void getNumbers(string tag, int[] numbers)
			{
				if (tag == "SPD")
				{
					numbers[0] = _speed;
				}
				else if (tag == "RA")
				{
					numbers[0] = _ra;
				}
				else if (tag == "DEC")
				{
					numbers[0] = _dec;
				}
				else if (tag == "BACK")
				{
					numbers[0] = _back;
				}
			}

			public override void onSelect(EventArgs args)
			{
				Console.CursorTop = 4;
				if (args.source.getTag() == "SPD")
				{
					Console.WriteLine("Set speed factor to {0} ({1:0.0000})", _speed, 1.0 + 1.0 * _speed / 10000.0);
				}
				else if (args.source.getTag() == "RA")
				{
					Console.WriteLine("Set RA Steps to {0}", _ra);
				}
				else if (args.source.getTag() == "DEC")
				{
					Console.WriteLine("Set DEC Steps to {0}", _dec);
				}
				else if (args.source.getTag() == "BACK")
				{
					Console.WriteLine("Set Backlash asdjust to {0}", _back);
				}
			}
		}
	}
}
