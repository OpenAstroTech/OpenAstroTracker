using System;

namespace MenuPrototype
{
	partial class Program
	{
		public class FindGPSDisplay : MenuItem
		{
			public FindGPSDisplay(string pitchOrRoll) : base("GPS", pitchOrRoll)
			{
			}

			public override void onDisplay(bool modal = false)
			{
				Console.WriteLine("* Found 0 sats              ");
				//if (foundEnoughSats)
				{
					getMainMenu().closeDialog();
				}
			}

			//public override void onSelect()
			//{
			//	Console.CursorTop = 4;
			//	Console.WriteLine("Store offset for " + _tag);
			//}

			public override bool onKeypressed(int key)
			{
				if (key == btnLEFT)
				{
					getMainMenu().activateDialog("StartHAChooser");
				}
				return base.onKeypressed(key);
			}
		}
	}
}
