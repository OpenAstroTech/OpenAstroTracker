using System;

namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class Button : MenuItem
		{
			Action<EventArgs> _chosenFunc;
			MenuItem _subMenu;
			bool _subMenuActivated;
			public Button(string choice, MenuItem subMenu) : base(choice)
			{
				_chosenFunc = null;
				_subMenu = subMenu;
				_subMenuActivated = false;
			}

			public Button(string choice, Action<EventArgs> chosenFunc) : base(choice)
			{
				_chosenFunc = chosenFunc;
				_subMenu = null;
				_subMenuActivated = false;
			}

			public override void closeMenuItem(MenuItem closeMe)
			{
				if (_subMenuActivated)
				{
					if (_subMenu == closeMe)
					{
						_subMenuActivated = false;
					}
					else
					{
						_subMenu.closeMenuItem(closeMe);
					}
				}
			}

			public override bool onKeypressed(int key)
			{
				if (_subMenuActivated)
				{
					return _subMenu.onKeypressed(key);
				}
				return base.onKeypressed(key);
			}

			public override bool onPreviewKey(int keyState)
			{
				if (_subMenuActivated)
				{
					return _subMenu.onPreviewKey(keyState);
				}
				return base.onPreviewKey(keyState);
			}

			public override void setMainMenu(MainMenu mainMenu)
			{
				base.setMainMenu(mainMenu);
				if (_subMenu != null)
				{
					_subMenu.setMainMenu(mainMenu);
				}
			}

			public override void onDisplay(bool modal = false)
			{
				if ((_subMenu != null) && _subMenuActivated)
				{
					_subMenu.onDisplay(modal);
				}
				else
				{
					string line = string.Empty;
					line = ">" + _displayName + "             ";
					Console.WriteLine(line);
				}
			}

			public override void onSelect()
			{
				if (_subMenu != null)
				{
					if (_subMenuActivated)
					{
						_subMenu.onSelect();
						_subMenuActivated = false;
					}
					else
					{
						_subMenuActivated = true;
					}
				}
				else
				{
					_chosenFunc(new SelectEventArgs(this, _displayName));
				}
			}
		}
	}
}
