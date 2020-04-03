bool movingToTarget = false;

void processRAKeys(int key)
{
  switch (key)
  {
    case btnUP:
      {
        if (RAselect == 0)
          RATime.addHours(1);
        if (RAselect == 1)
          RATime.addMinutes(1);
        if (RAselect == 2)
          RATime.addSeconds(1);

        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnDOWN:
      {
        if (RAselect == 0)
          RATime.addHours(-1);
        if (RAselect == 1)
          RATime.addMinutes(-1);
        if (RAselect == 2)
          RATime.addSeconds(-1);

        // slow down key repetitions
        delay(150);
        waitForButtonRelease = false;
      }
      break;

    case btnLEFT:
      {
        RAselect = adjustWrap(RAselect, 1, 0, 2);
      }
      break;

    case btnSELECT:
      {
        if (movingToTarget) {
          stopSteppers();
        }
        else {
          startMoveSteppersToTargetAsync();
        }
        movingToTarget = !movingToTarget ;
      }
      break;

    case btnRIGHT:
      {
        lcdMenu.setNextActive();
      }
      break;
  }

  if (movingToTarget) {
    if (!moveSteppersToTargetAsync()) {
      movingToTarget = false;
    }
  }
}

void printRASubmenu() {
  if (!movingToTarget) {
    lcdMenu.printMenu(formatRA(&RADisplayTime, RAselect));
  }
}


/*
  WIP for complete OO-ification of code.

  class Processor
  {
    Processor *_nextProcessor;
    int _numSubMenuItems;
    int _currentSubMenu;

  public:
    Processor(int numSubMenuItems)
    {
      _numSubMenuItems = numSubMenuItems;
      _currentSubMenu = 0;
      _nextProcessor = nullptr;
    }

    void setNext(Processor *nextProcessor) {
      _nextProcessor = nextProcessor;
    }
    virtual const char *display() = 0;
    virtual void onDownKeyPressed() {}
    virtual void onUpKeyPressed() {}
    virtual void onLeftKeyPressed() {
      adjustSubMenu();
    }
    virtual void onRightKeyPressed() { }
    virtual void onSelectKeyPressed() {}
    virtual void onKeyReleased(int key) {}
    virtual void onIdle() {}
    void adjustSubMenu(int by = 1)
    {
      _currentSubMenu = adjustWrap(_currentSubMenu, by, 0, _numSubMenuItems);
    }
  };

  class ProcessRA : Processor
  {
  public:
    ProcessRA() : Processor(3)
    {
    }
    const char *display()
    {
      return "RA";
    }

    void onDownKeyPressed()
    {
    }

    void onUpKeyPressed()
    {
    }

    void onLeftKeyPressed()
    {
    }

    void onRightKeyPressed()
    {
    }

    void onSelectKeyPressed()
    {
    }

    void onIdle()
    {
    }
  };

  class MenuProcessor
  {
    Processor *_firstProcessor;
    Processor *_lastProcessor;
    Processor *_activeProcessor;
    int _lastKeyState;

  public:
    MenuProcessor()
    {
      _firstProcessor = nullptr;
      _lastProcessor = nullptr;
      _activeProcessor = nullptr;
      _lastKeyState = btnNONE;
    }

    void loop()
    {
      int keyState = read_LCD_buttons();
      if ((_lastKeyState == btnNONE) && (keyState != btnNONE))
      {
        switch (keyState)
        {
          case btnUP:
            _activeProcessor->onUpKeyPressed();
            break;
          case btnDOWN:
            _activeProcessor->onDownKeyPressed();
            break;
          case btnLEFT:
            _activeProcessor->onLeftKeyPressed();
            break;
          case btnRIGHT:
            _activeProcessor->onRightKeyPressed();
            break;
          case btnSELECT:
            _activeProcessor->onSelectKeyPressed();
            break;
        }
        _lastKeyState = keyState;
      }
      else if ((_lastKeyState != btnNONE) && (keyState == btnNONE))
      {
        _activeProcessor->onKeyReleased(_lastKeyState);
      }
      else if ((_lastKeyState == btnNONE) && (keyState == btnNONE))
      {
        _activeProcessor->onIdle();
      }
    }

    void addProcessor(Processor *processor)
    {
      if (_firstProcessor == nullptr)
      {
        _firstProcessor = processor;
      }
      else
      {
        _firstProcessor->setNext(processor);
      }
    }
  };
*/
