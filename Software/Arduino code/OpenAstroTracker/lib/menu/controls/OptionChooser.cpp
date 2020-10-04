
#include "MainMenu.hpp"
#include "SelectEventArgs.hpp"
#include "OptionChooser.hpp"
#include "../../input/LcdButtons.hpp"

OptionChooser::OptionChooser(String choice1, String choice2, int activeItemIndex, eventAction chosenFunc) : MenuItem("", "OPT")
{
    _activeOption = activeItemIndex;
    _chosenFunc = chosenFunc;
    _choices[0] = choice1;
    _choices[1] = choice2;
    _numChoices = 2;
}

OptionChooser::OptionChooser(String choice1, String choice2, String choice3, int activeItemIndex, eventAction chosenFunc) : MenuItem("", "OPT")
{
    _activeOption = activeItemIndex;
    _chosenFunc = chosenFunc;
    _choices[0] = choice1;
    _choices[1] = choice2;
    _choices[2] = choice3;
    _numChoices = 3;
}

OptionChooser::OptionChooser(String choice1, String choice2, String choice3, String choice4, int activeItemIndex, eventAction chosenFunc) : MenuItem("", "OPT")
{
    _activeOption = activeItemIndex;
    _chosenFunc = chosenFunc;
    _choices[0] = choice1;
    _choices[1] = choice2;
    _choices[2] = choice3;
    _choices[3] = choice4;
    _numChoices = 4;
}

void OptionChooser::onDisplay(bool modal)
{
    String line = "";
    for (int i = 0; i < _numChoices; i++)
    {
        line += ((i == _activeOption) ? '>' : ' ') + _choices[i] + ((i == _activeOption) ? '<' : ' ') + ' ';
    }
    if (line.length() > 16)
    {
        line = "";
        for (int i = 0; i < _numChoices; i++)
        {
            line += ((i == _activeOption) ? '>' : ' ') + _choices[i] + ((i == _activeOption) ? '<' : ' ');
        }
    }

    getMainMenu()->writeToLCD(0, 1, line);
}

void OptionChooser::onSelect()
{
    SelectEventArgs chosen(this, _choices[_activeOption]);
    _chosenFunc(&chosen);
}

String OptionChooser::getSelected()
{
    return _choices[_activeOption];
}

int OptionChooser::getSelectedIndex()
{
    return _activeOption;
}

bool OptionChooser::onKeypressed(int key)
{
    if (MenuItem::onKeypressed(key))
    {
        return true;
    }

    if (key == btnLEFT)
    {
        _activeOption = (_activeOption + 1) % _numChoices;
        return true;
    }
    return false;
}
