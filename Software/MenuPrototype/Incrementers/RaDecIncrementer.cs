namespace MenuPrototype
{
	partial class Program
	{
		public class RaDecIncrementer : Incrementer
		{
			bool _isRA;
			public RaDecIncrementer(string which) : base(INCREMENT_WRAP, new[] { 90, 59, 59, 1 }, new[] { -90, 0, 0, 0 })
			{
				_isRA = which == "RA";
				if (_isRA)
				{
					setMinVals(new int[] { 0, 0, 0, 0 });
					setMaxVals(new int[] { 23, 59, 59, 1 });
				}
			}

			public override void onChange(string tag, int[] numbers, int index, int val)
			{
				if ((index == 0) && !_isRA)
				{
					numbers[0] = adjustClamp(numbers[0], val, -90, 90);
				}
				else
				{
					base.onChange(tag, numbers, index, val);
				}
			}
			public override string getDisplay(string tag, int index, int val, string formatString)
			{
				if (index == 3)
				{
					return val == 0 ? "Ta" : "Cu";
				}

				return val.ToString();
			}

			public override void getNumbers(string tag, int[] numbers)
			{
				if (numbers[3] == 0)
				{
					numbers[0] = 50; // mount get target ra/dec
				}
				else if (numbers[3] == 1)
				{
					numbers[0] = 80;// mount get current ra/dec
				}
			}
		}
	}
}
