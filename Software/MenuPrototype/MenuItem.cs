using System;
using System.Collections.Generic;

namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class MenuItem
		{
			protected int _activeSubMenu;
			protected string _displayName;
			protected string _tag;
			protected List<MenuItem> _subMenuList;
			protected MainMenu _mainMenu;

			public MenuItem(string displayName, string tag = "")
			{
				_tag = tag;
				_displayName = displayName;
				_subMenuList = new List<MenuItem>();
				_activeSubMenu = 0;
			}

			public void addMenuItem(MenuItem item)
			{
				_subMenuList.Add(item);
			}

			public virtual void closeMenuItem(MenuItem closeMe)
			{
				if (_subMenuList.Count > 0)
				{
					_subMenuList[_activeSubMenu].closeMenuItem(closeMe);
				}
			}

			public virtual MainMenu getMainMenu()
			{
				return _mainMenu;
			}

			public virtual void setMainMenu(MainMenu mainMenu)
			{
				_mainMenu = mainMenu;
				for (int i = 0; i < _subMenuList.Count; i++)
				{
					_subMenuList[i].setMainMenu(mainMenu);
				}
			}

			public string getTag() { return _tag; }
			public void setTag(string tag) { _tag = tag; }
			public string getDisplayName() { return _displayName; }

			public virtual void onDisplay(bool modal = false)
			{
				if (modal)
				{
					Console.WriteLine(_displayName);
				}
				if (_subMenuList.Count > 0)
				{
					_subMenuList[_activeSubMenu].onDisplay(modal);
				}
			}

			public virtual void onSelect() { }

			public virtual bool onPreviewKey(int keyState)
			{
				if (_subMenuList.Count > 0)
				{
					return _subMenuList[_activeSubMenu].onPreviewKey(keyState);
				}
				return false;
			}

			public virtual bool onKeypressed(int key)
			{ // True if should wait for release
				if ((key == btnRIGHT) && (_subMenuList.Count > 0))
				{
					_activeSubMenu = (_activeSubMenu + 1) % _subMenuList.Count;
					return true;
				}
				if (_subMenuList.Count > 0)
				{
					if (key == btnSELECT)
					{
						_subMenuList[_activeSubMenu].onSelect();
						return true;
					}
					return _subMenuList[_activeSubMenu].onKeypressed(key); ;
				}
				return false;
			}
		}
	}
}
