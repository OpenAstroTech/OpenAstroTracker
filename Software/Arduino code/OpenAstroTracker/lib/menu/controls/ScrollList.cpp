#include "../../input/LcdButtons.hpp"
#include "../../util/debug.hpp"
#include "../../util/utils.hpp"
#include "ScrollList.hpp"

ScrollList::ScrollList() : MenuItem("S","SCRL")
{
}

void ScrollList::onSelect()
{
    if (_subMenuList->count() > 0)
    {
        _subMenuList->getItem(_activeSubMenu)->onSelect();
    }
}

bool ScrollList::onKeypressed(int key)
{
    if (key == btnDOWN)
    {
        //LOGV1(255, "SCROLL: Down pressed");
        _activeSubMenu = adjustWrap(_activeSubMenu, 1, 0, _subMenuList->count() - 1);
    }
    else if (key == btnUP)
    {
        //LOGV1(255, "SCROLL: Up pressed");
        _activeSubMenu = adjustWrap(_activeSubMenu, -1, 0, _subMenuList->count() - 1);
    }
    else 
    {
        if (_subMenuList->count() > 0)
        {
            return _subMenuList->getItem(_activeSubMenu)->onKeypressed(key);
        }
    }
    return true;
}
