void handleDECandRACalculations()
{
    float hourPos = RATime.getTotalHours();
  // Map [0 to 24] range to [-12 to +12] range
  if (hourPos > 12) {
    hourPos = hourPos - 24;
  }

  // How many steps moves the RA ring one hour along (15degrees?)
  stepsPerHour = (float(RAsteps) / 288.0) * RevSteps;

  // Where do ww want to move RA to?
  float moveRA = hourPos * stepsPerHour / 2;

  // Where do we want to move DEC to?
  moveDEC = (degreeDEC * float(DECsteps) + minDEC * (float(DECsteps) / 60.0f) + secDEC * (float(DECsteps) / 3600.0f));

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

  // Since the Control menu does it's own stepping, don't move the axes.
  if (lcdMenu.getActive() != Control_Menu) {
    stepperRA.moveTo(-moveRA);
    stepperDEC.moveTo(moveDEC);
  }
}
