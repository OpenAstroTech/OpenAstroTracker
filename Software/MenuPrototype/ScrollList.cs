namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class ScrollList : MenuItem
		{
			public ScrollList() : base(string.Empty)
			{
			}

			public override void onSelect()
			{
				if (_subMenuList.Count > 0)
				{
					_subMenuList[_activeSubMenu].onSelect();
				}
			}

			public override bool onKeypressed(int key)
			{
				if (key == btnDOWN)
				{
					_activeSubMenu = adjustWrap(_activeSubMenu, 1, 0, _subMenuList.Count - 1);
				}
				else if (key == btnUP)
				{
					_activeSubMenu = adjustWrap(_activeSubMenu, -1, 0, _subMenuList.Count - 1);
				}
				else
				{
					if (_subMenuList.Count > 0)
					{
						_subMenuList[_activeSubMenu].onKeypressed(key);
					}
				}
				return true;
			}
		}
	}
}
