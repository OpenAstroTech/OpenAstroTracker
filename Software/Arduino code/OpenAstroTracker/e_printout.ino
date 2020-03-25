  if (!pcControl) {
  
    switch (lcdMenu.getActive()) {
      case RA_Menu: printRASubmenu(); break;
      case DEC_Menu: printDECSubmenu(); break;
      case HA_Menu: printHASubmenu(); break;
      case Home_Menu: printHomeSubmenu(); break;
      case Polaris_Menu: printPolarisSubmenu(); break;
      case Heat_Menu: printHeatSubmenu(); break;
      case Control_Menu: printControlSubmenu(); break;
      case Calibration_Menu: printCalibrationSubmenu(); break;
    }
  
    //tracking menu  ----------------------
    /*if (lcdMenu.getActive() == TRK_Menu) {
      lcd.print("Tracking ON OFF");
      lcd.print("         ");
      }*/
  }
  BTin();
}
