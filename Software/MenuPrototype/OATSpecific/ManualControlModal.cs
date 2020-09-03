using System;

namespace MenuPrototype
{
	partial class Program
	{
		public class ManualControlEventArgs : EventArgs
		{
			private int _oldState;
			private int _newState;
			public enum EventType { Close, ChangeState }
			public ManualControlEventArgs(MenuItem source, EventType evt, int oldState = 0, int newState = 0) : base(source)
			{
				Event = evt;
				_oldState = oldState;
				_newState = newState;
			}

			public EventType Event { get; set; }

			public int getOldState() { return _oldState; }
			public int getNewState() { return _newState; }
		}

		////////////////////////////////////////////////////////////////////
		public class ManualControlModal : MenuItem
		{
			Action<ManualControlEventArgs> _handler;
			int _keyState = btnNONE;
			public ManualControlModal(Action<ManualControlEventArgs> handler, string tag) : base("", tag)
			{
				_handler= handler;
			}

			public override void onDisplay(bool modal = false)
			{
				Console.CursorTop = 1;
				Console.CursorLeft = 0;
				Console.WriteLine("Use UDLR to home         ");
				Console.WriteLine("R031504 D+897322    ");
			}

			public override bool onPreviewKey(int keyState)
			{
				int top = Console.CursorTop;
				Console.CursorTop = 4;
				if (_keyState != keyState)
				{
					var stateChange = new ManualControlEventArgs(this, ManualControlEventArgs.EventType.ChangeState, _keyState, keyState);
					_handler(stateChange);
					_keyState = keyState;
				}
				else
				{
					Console.CursorTop = top;
					return base.onPreviewKey(keyState);
				}

				return true;
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnSELECT)
				{
					_handler(new ManualControlEventArgs(this, ManualControlEventArgs.EventType.Close));
					return true;
				}
				return false;
			}

		}
	}
}
