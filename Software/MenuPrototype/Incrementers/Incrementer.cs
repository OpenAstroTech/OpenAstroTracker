namespace MenuPrototype
{
	partial class Program
	{

		const int INCREMENT_NO_LIMITS = 0;
		const int INCREMENT_WRAP = 1;
		const int INCREMENT_CLAMP = 2;

		public class Incrementer
		{
			int _mode;
			int[] _maxVals;
			int[] _minVals;

			public Incrementer(int mode = INCREMENT_NO_LIMITS, int[] maxVals = null, int[] minVals = null)
			{
				_mode = mode;
				_maxVals = maxVals;
				_minVals = minVals;
			}

			protected void setMinVals(int[] minVals) { _minVals = minVals; }
			protected void setMaxVals(int[] maxVals) { _maxVals = maxVals; }

			public virtual void onChange(string tag, int[] numbers, int index, int val)
			{
				if (_mode == INCREMENT_CLAMP)
				{
					numbers[index] = adjustClamp(numbers[index], val, (_minVals != null) ? _minVals[index] : 0, _maxVals[index]);
				}
				else if (_mode == INCREMENT_WRAP)
				{
					numbers[index] = adjustWrap(numbers[index], val, (_minVals != null) ? _minVals[index] : 0, _maxVals[index]);
				}
				else if (_mode == INCREMENT_NO_LIMITS)
				{
					numbers[index] += val;
				}
			}

			public virtual string getDisplay(string tag, int index, int val, string formatString = null)
			{
				return val.ToString();
			}

			public virtual void getNumbers(string tag, int[] nums) { }

			public virtual void onSelect(EventArgs args) { }
		}
	}
}
