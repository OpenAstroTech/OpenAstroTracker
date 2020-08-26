using System;

namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class TextInfoMulti : MenuItem
		{
			String _prompt;
			Func<int, string> _displayFunc;
			int _currentSubItem;
			int _subItems;

			public TextInfoMulti(int subItems, Func<int, string> displayFunc) : base("")
			{
				_subItems = subItems;
				_currentSubItem = 0;
				_displayFunc = displayFunc;
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnLEFT)
				{
					_currentSubItem = (_currentSubItem + 1) % _subItems;
					return true;
				}
				return base.onKeypressed(key);
			}
			public override void onDisplay(bool modal = false)
			{
				Console.WriteLine(_displayFunc(_currentSubItem) + "            ");
			}
		}
	}
}
