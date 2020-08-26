using System;

namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class ManualControlMenuItem : MenuItem
		{
			Action<EventArgs> _onClose;
			public ManualControlMenuItem(Action<EventArgs> onClose, string tag = "Control") : base("", tag)
			{
				_onClose = onClose;
			}

			public override void onDisplay(bool modal = false)
			{
				Console.CursorTop = 1;
				Console.CursorLeft = 0;
				Console.WriteLine("Use UDLR to home         ");
				Console.WriteLine("R031504 D+897322    ");
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnSELECT)
				{
					_onClose(new SelectEventArgs(this));
					return true;
				}
				return false;
			}

		}
	}
}
