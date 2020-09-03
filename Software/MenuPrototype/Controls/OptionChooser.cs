using System;
using System.Linq;

namespace MenuPrototype
{
	partial class Program
	{
		/*
		ModalScreen - Takes over display
		ActionList - Select from list of items to invoke an action(used in GO)
		LabelList - Scroll through list of items(used in INFO)
		SubmenuList - Choose items(CTRL / CAL)
		ModalSubmenuList - Only for SubMenu, takes over 2nd line
		*/

		////////////////////////////////////////////////////////////////////
		public class OptionChooser : MenuItem
		{ // 2 (Yes/No) or 3 (Yes/No/Cancl)
			private int _activeOption;
			string[] _choices;
			Action<EventArgs> _chosenFunc;
			public OptionChooser(string[] choices, int activeItemIndex, Action<EventArgs> chosenFunc) : base("")
			{
				_activeOption = activeItemIndex;
				_chosenFunc = chosenFunc;
				_choices = choices.ToArray();
			}

			public override void onDisplay(bool modal = false)
			{
				string line = string.Empty;
				for (int i = 0; i < _choices.Length; i++)
				{
					line += ((i == _activeOption) ? '>' : ' ') + _choices[i] + ((i == _activeOption) ? '<' : ' ') + ' ';
				}
				Console.WriteLine(line);
			}

			public override void onSelect()
			{
				_chosenFunc(new SelectEventArgs(this, _choices[_activeOption]));
			}

			public string getSelected()
			{
				return _choices[_activeOption];
			}

			public override bool onKeypressed(int key)
			{
				if (base.onKeypressed(key)) { return true; }
				if (key == btnLEFT)
				{
					_activeOption = (_activeOption + 1) % _choices.Length;
					return true;
				}
				return false;
			}
		}
	}
}
