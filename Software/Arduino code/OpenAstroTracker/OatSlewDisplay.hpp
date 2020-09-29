#include <Arduino.h>

#include "lib/menu/controls/ActionRunnerModal.hpp"

class SlewDisplay : public ActionRunnerModal {
public:
    SlewDisplay(String displayname, String tag);
    bool isComplete() override;
    void onDisplay(bool modal = false) override;
};