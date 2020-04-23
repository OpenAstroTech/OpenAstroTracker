var scope = "ASCOM.OpenAstroTracker.Telescope";
var T = new ActiveXObject(scope);
T.Connected = true;

printInfo("Start Up");

WScript.StdOut.Write("\nHit Any Key To Begin...(Ctrl+C to exit)");
var option = WScript.Stdin.ReadLine();
WScript.StdOut.WriteLine()

slewRelativeMeridian(-0.00001, T.SiteLatitude);
printInfo("Post Slew");

T.SyncToCoordinates(5, -5);
WScript.Sleep(1000);
printInfo("Post Sync");

WScript.StdOut.WriteLine("Script Complete");

function slewRelativeMeridian(degreesFromMeridian, degreesFromEquator){
    if( T.AtPark )
        T.Unpark();
    if (T.CanSetTracking && !T.Tracking)
        T.Tracking = true;
    var hoursFromMeridian = -degreesFromMeridian / 4;
    var ra = T.SiderealTime + hoursFromMeridian;
    var dec = degreesFromEquator;
    WScript.StdOut.WriteLine("Slewing to " + hoursFromMeridian + " from meridian.");
    WScript.StdOut.WriteLine("   RA:  " + ra );
    WScript.StdOut.WriteLine("   Dec: " + dec);
    T.SlewToCoordinates(ra, dec);
    WScript.StdOut.WriteLine("... slew complete");
}

function printInfo(title){
    WScript.StdOut.WriteLine("---------------------------- " + title);
    WScript.StdOut.WriteLine("RA:  " + T.RightAscension);
    WScript.StdOut.WriteLine("Dec: " + T.Declination);
    WScript.StdOut.WriteLine("Pier Side: " + (T.SideOfPier == 0 ? "East" : "West"));
    WScript.StdOut.WriteLine();
}