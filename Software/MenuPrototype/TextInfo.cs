using System;

namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class TextInfo : MenuItem
		{
			String _prompt;
			Func<string> _displayFunc;

			public TextInfo(String prompt, Func<string> displayFunc) : base("")
			{
				_prompt = prompt;
				_displayFunc = displayFunc;
			}

			public override void onDisplay(bool modal = false)
			{
				Console.WriteLine(_prompt + _displayFunc() + "            ");
			}
		}
	}
}
