// This code tells the steppers to what location to move to, given the select right ascension and declination
void handleDECandRACalculations()
{
  float hourPos = RATime.getTotalHours();
  // Map [0 to 24] range to [-12 to +12] range
  if (hourPos > 12) {
    hourPos = hourPos - 24;
  }

  // How many steps moves the RA ring one hour along (15degrees?) No idea what 288 is?
  stepsPerHour = (float(RAStepsPerDegree) / 288.0) * StepsPerRevolution;

  // Where do we want to move RA to? Why divided by 2?
  float moveRA = hourPos * stepsPerHour / 2;

  // Where do we want to move DEC to?
  // the variable degreeDEC is 0deg for the celestial pole (90deg), and goes negative only.
  moveDEC = (degreeDEC * float(DECStepsPerDegree) + minDEC * (float(DECStepsPerDegree) / 60.0f) + secDEC * (float(DECStepsPerDegree) / 3600.0f));

  // We can move 6 hours in either direction. Outside of that we need to flip directions.
  float RALimit = (6.0f * stepsPerHour / 2);

  // If we reach the limit in the positive direction ...
  if (moveRA > RALimit) {
    // ... turn both RA and DEC axis around
    float oldRA = moveRA;
    moveRA -= long(12.0f * stepsPerHour / 2);
    moveDEC = -moveDEC;
    //Serial.println(format("> %s HRPos: %f  MoveRA: %f   newMoveRA: %f    RALimit: %f", RATime.ToString().c_str(), hourPos, oldRA, moveRA, RALimit));
  }
  // If we reach the limit in the negative direction...
  else if (moveRA < -RALimit) {
    // ... turn both RA and DEC axis around
    float oldRA = moveRA;
    moveRA += long(12.0f * stepsPerHour / 2);
    moveDEC = -moveDEC;
    //Serial.println(format("< %s HRPos: %f  MoveRA: %f   newMoveRA: %f    RALimit: %f", RATime.ToString().c_str(), hourPos, oldRA, moveRA, RALimit));
  } else {
    //Serial.println(format("= %s HRPos: %f  MoveRA: %f   newMoveRA: %f    RALimit: %f", RATime.ToString().c_str(), hourPos, moveRA, moveRA, RALimit));
  }

  float targetRA = clamp(-moveRA, -RAStepperLimit, RAStepperLimit);
  float targetDEC = clamp(moveDEC, DECStepperUpLimit, DECStepperDownLimit);

  // Can we get there without physical issues? (not doing anything with this yet)
  isUnreachable = ((targetRA != -moveRA) || (targetDEC != moveDEC));

  // Show time: tell the steppers where to go!
  stepperRA.moveTo(targetRA);
  stepperDEC.moveTo(targetDEC);
}

void runTracker()
{
  if (tracking == 1) {
    stepperTRK.runSpeed();
  }
}

void  doCalculations() {
  currentSecs = millis() / 1000;

  RADisplayTime.set(RATime);
  DayTime HACorrection(HATime);
  HACorrection.addTime(-h, -m, -s);
  RADisplayTime.addTime(HACorrection);

  if (!north) {
    printdegDEC = degreeDEC - 90;
    if (printdegDEC < -90) degreeDEC = 0;
  }
  else {
    printdegDEC = degreeDEC + 90;
    if (printdegDEC > 90) degreeDEC = 0;
  }
}
