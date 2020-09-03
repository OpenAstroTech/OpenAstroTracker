using System;

namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class NumberInput : MenuItem
		{
			public enum BehaviorFlags { ConstantRepetition, AcceleratingRepetition }
			String _mask;
			int _activeNumber;
			int[] _numbers;
			Action<EventArgs> _selectCallback;
			ulong _nextKeyOk;
			ulong _waitFor;
			BehaviorFlags _flags;
			bool _useIncrForDisplay;
			Incrementer _incr;

			public NumberInput(string tag, int numNumbers, String mask, Action<EventArgs> selectCallback, Incrementer incr, BehaviorFlags flags = BehaviorFlags.ConstantRepetition) : base("", tag)
			{
				_activeNumber = 0;
				_mask = mask;
				_numbers = new int[numNumbers];
				_selectCallback = selectCallback;
				_flags = flags;
				_incr = incr ?? new Incrementer();
				_useIncrForDisplay = true;
			}

			public NumberInput(string tag, int[] numbers, String mask, Action<EventArgs> selectCallback, Incrementer incr = null, BehaviorFlags flags = BehaviorFlags.ConstantRepetition) : base("", tag)
			{
				_activeNumber = 0;
				_mask = mask;
				_numbers = numbers;
				_selectCallback = selectCallback;
				_flags = flags;
				_incr = incr ?? new Incrementer();
				_useIncrForDisplay = false;
			}
			// ("RASteps : %d")  ("^%02dh^%02dm") ("^%02d*^%02d'^%02d\"") ("^+%02d*^%02d'^%02d\"")

			public override void onDisplay(bool modal = false)
			{
				int cursor = _activeNumber;
				int numberCursor = 0;
				string line = string.Empty;
				if (_useIncrForDisplay)
				{
					_incr.getNumbers(_tag, _numbers);
				}
				for (int i = 0; i < _mask.Length; i++)
				{
					if (_mask[i] == '^')
					{
						line += cursor == 0 ? '>' : ' ';
						cursor--;
					}
					else if (_mask[i] == '@')
					{
						line += _incr.getDisplay(_tag, numberCursor, _numbers[numberCursor]);
						numberCursor++; ;
					}
					else if (_mask[i] == '%')
					{
						int start = i;
						i++;
						while (char.IsDigit(_mask[i]))
						{
							i++;
						}

						line += _incr.getDisplay(_tag, numberCursor, _numbers[numberCursor], _mask.Substring(start, i - start));
						numberCursor++;
					}
					else
					{
						line += _mask[i];
					}
				}

				Console.WriteLine(line + "              ");
			}

			public override void onSelect()
			{
				if (_selectCallback != null)
				{
					_selectCallback(new SelectEventArgs(this));
				}
				else
				{
					_incr.onSelect(new SelectEventArgs(this));
				}
			}

			public int getValue(int index) { return _numbers[index]; }

			public override bool onPreviewKey(int keyState)
			{
				if (keyState == btnDOWN)
				{
					if (millis() > _nextKeyOk)
					{
						_incr.onChange(_tag, _numbers, _activeNumber, -1);
						_waitFor = (_flags == NumberInput.BehaviorFlags.AcceleratingRepetition) ? (ulong)Math.Max(10, 0.95 * _waitFor) : 200;
						_nextKeyOk = millis() + _waitFor;
					}
				}
				else if (keyState == btnUP)
				{
					if (millis() > _nextKeyOk)
					{
						_incr.onChange(_tag, _numbers, _activeNumber, 1);
						_waitFor = (_flags == NumberInput.BehaviorFlags.AcceleratingRepetition) ? (ulong)Math.Max(10, 0.95 * _waitFor) : 200;
						_nextKeyOk = millis() + _waitFor;
					}
				}
				else
				{
					_nextKeyOk = 0;
					_waitFor = 200;
					return false;
				}
				return true;
			}

			public override bool onKeypressed(int key)
			{
				if (!base.onKeypressed(key))
				{
					if (key == btnLEFT)
					{
						_activeNumber = (_activeNumber + 1) % _numbers.Length;
						return true;
					}
				}
				return false;
			}

		}
	}
}
