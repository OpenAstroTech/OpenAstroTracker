using System;

namespace MenuPrototype
{
	partial class Program
	{
		public class PitchRollDisplay : MenuItem
		{
			public PitchRollDisplay(string pitchOrRoll) : base("Roll Offset", pitchOrRoll)
			{
			}

			public override void onDisplay(bool modal = false)
			{
				if (_tag == "ROLL")
				{
					Console.WriteLine("R ------0.4-<----            ");
				}
				else
				{
					Console.WriteLine("P --->--0.6------            ");
				}
			}

			public override void onSelect()
			{
				Console.CursorTop = 4;
				Console.WriteLine("Store offset for " + _tag);
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnLEFT)
				{
					getMainMenu().closeMenuItem(this);
				}
				return base.onKeypressed(key);
			}

			public override bool onPreviewKey(int keyState)
			{
				// Ignore Up and Down buttons.
				if (keyState == btnUP) return true;
				if (keyState == btnDOWN) return true;
				if (keyState == btnRIGHT) return true;

				return base.onPreviewKey(keyState);
			}
		}
	}
}
