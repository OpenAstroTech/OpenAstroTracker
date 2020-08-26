using System;
using System.Collections.Generic;

namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class MainMenu
		{
			private MenuItem _activeDialog;
			private int _activeItem;
			private List<MenuItem> _topMenuList;
			private List<MenuItem> _dialogs;

			public MainMenu()
			{
				_topMenuList = new List<MenuItem>();
				_dialogs = new List<MenuItem>();
				_activeItem = 0;
				_activeDialog = null;
			}


			public void closeDialog()
			{
				_activeDialog = null;
			}

			public void activateDialog(string tag)
			{
				for (int i = 0; i < _dialogs.Count; i++)
				{
					if (_dialogs[i].getTag() == tag)
					{
						_activeDialog = _dialogs[i];
						break;
					}
				}
			}

			public void addMenuItem(MenuItem item)
			{
				_topMenuList.Add(item);
				item.setMainMenu(this);
			}

			public void addModalDialog(MenuItem dialog)
			{
				dialog.setMainMenu(this);
				_dialogs.Add(dialog);
			}

			public bool onPreviewKey(int keyState)
			{
				if (_activeDialog != null)
				{
					return _activeDialog.onPreviewKey(keyState);
				}
				return _topMenuList[_activeItem].onPreviewKey(keyState);
			}

			public bool processKeys(int key)
			{
				if (_activeDialog != null)
				{
					bool waitForRelease = _activeDialog.onKeypressed(key);
					return waitForRelease;
				}

				if (key == btnRIGHT)
				{
					_activeItem = (_activeItem + 1) % _topMenuList.Count;
					return true;
				}

				return _topMenuList[_activeItem].onKeypressed(key);
			}

			public void updateDisplay()
			{
				string menu = string.Empty;
				if (_activeDialog != null)
				{
					_activeDialog.onDisplay(true);
					return;
				}

				for (int i = 0; i < _topMenuList.Count; i++)
				{
					menu += string.Format("{0}{1}{2} ", _activeItem == i ? '>' : ' ', _topMenuList[i].getDisplayName(), _activeItem == i ? '<' : ' ');
				}
				Console.WriteLine(menu);

				_topMenuList[_activeItem].onDisplay();
			}

			public virtual void closeMenuItem(MenuItem closeMe)
			{
				if (_activeDialog != null)
				{
					_activeDialog.closeMenuItem(closeMe);
				}
				else
				{
					_topMenuList[_activeItem].closeMenuItem(closeMe);
				}
			}
		}
	}
}
