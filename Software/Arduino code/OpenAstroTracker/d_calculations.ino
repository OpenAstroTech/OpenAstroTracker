
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
