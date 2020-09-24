#pragma once

#include "EventArgs.hpp"

class MenuItem;

class ChangedEventArgs : public EventArgs
{
private:
	int _index;
	int _newVal;
	int _oldVal;
	int *_newNumbers;

public:
	ChangedEventArgs(MenuItem *source);
	ChangedEventArgs(MenuItem *source, int index, int newVal, int oldVal, int *_newNumbers = nullptr);
	int getIndex();
	int getNewVal();
	int getOldVal();
	int* getNumbers();
};
