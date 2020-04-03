/////////////////////////////////////////////
// This code tells the steppers to what location to move to, given the select right ascension and declination
void handleDECandRACalculations()
{
  float hourPos = RATime.getTotalHours();
  // Map [0 to 24] range to [-12 to +12] range
  if (hourPos > 12) {
    hourPos = hourPos - 24;
  }

  // How many steps moves the RA ring one hour along (15degrees?) No idea what 288 is?
  // stepsPerHour = (float(RAStepsPerDegree) / 288.0) * StepsPerRevolution;
  stepsPerHour = RAStepsPerDegree * 15.0;

  // Where do we want to move RA to? Why divided by 2?
  float moveRA = hourPos * stepsPerHour / 2;

  // Where do we want to move DEC to?
  // the variable degreeDEC is 0deg for the celestial pole (90deg), and goes negative only.
  moveDEC = degreeDEC + ( minDEC / 60.0f) + (secDEC / 3600.0f); // Convert to double
  moveDEC *= DECStepsPerDegree;  // How many steps represent that position?

  // We can move 6 hours in either direction. Outside of that we need to flip directions.
  float RALimit = (6.0f * stepsPerHour / 2);

  // If we reach the limit in the positive direction ...
  if (moveRA > RALimit) {
    // ... turn both RA and DEC axis around
    float oldRA = moveRA;
    moveRA -= long(12.0f * stepsPerHour / 2);
    moveDEC = -moveDEC;
  }
  // If we reach the limit in the negative direction...
  else if (moveRA < -RALimit) {
    // ... turn both RA and DEC axis around
    float oldRA = moveRA;
    moveRA += long(12.0f * stepsPerHour / 2);
    moveDEC = -moveDEC;
  }

  float targetRA = clamp(-moveRA, -RAStepperLimit, RAStepperLimit);
  float targetDEC = clamp(moveDEC, DECStepperUpLimit, DECStepperDownLimit);

  // Can we get there without physical issues? (not doing anything with this yet)
  isUnreachable = ((targetRA != -moveRA) || (targetDEC != moveDEC));

//  if (stepperRA.currentPosition() != int(targetRA)) {
//    Serial.println("Moving RA from " + String(stepperRA.currentPosition()) + " to " + targetRA);
//  }
//  if (stepperDEC.currentPosition() != (targetDEC)) {
//    Serial.println("Moving DEC from " + String(stepperDEC.currentPosition()) + " to " + targetDEC);
//  }
  
  // Show time: tell the steppers where to go!
  stepperRA.moveTo(targetRA);
  stepperDEC.moveTo(targetDEC);
}

/////////////////////////////////////////////
// Runs the tracker stepper to compensate for earths rotation.
void runTracker()
{
  if (tracking == 1) {
    stepperTRK.runSpeed();
  }
}

/////////////////////////////////////////////
// Calculate tracking speed, RA and DEC display.
void doCalculations() {

  // Run speed for steppers is steps/second
  // What are these magic numbers??? 335.14? 288? 3590?
  //trackingSpeed = ((((335.1417 / 288.0) * StepsPerRevolution) / 3590)) - 1 + float(speedCalibration);

  // The tracker simply needs to rotate at 15degrees/hour, adjusted for sidereal
  // time (i.e. the 15degrees is per 23h56m04s. 3590/3600 is the same ratio).
  trackingSpeed  = speedCalibration * RAStepsPerDegree * 15.0f / 3590.0f;
  stepperTRK.setSpeed(trackingSpeed);

  RADisplayTime.set(RATime);
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
