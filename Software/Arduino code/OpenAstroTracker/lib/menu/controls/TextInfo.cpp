#include "MainMenu.hpp"
#include "TextInfo.hpp"

TextInfo::TextInfo(String prompt, displayFunc displayFunc) : MenuItem("", "")
{
    _prompt = prompt;
    _displayFunc = displayFunc;
}

void TextInfo::onDisplay(bool modal)
{
    getMainMenu()->writeToLCD(0, 1, _prompt + _displayFunc());
}
