using System;
using System.Threading;
using ASCOM.Astrometry.AstroUtils;
using ASCOM.DeviceInterface;
using ASCOM.Utilities;
using ASCOM.Astrometry.Transform;
using System.Collections;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ASCOM.OpenAstroTracker {
    [Guid("be07c02f-8a5e-429f-87b1-23fe9d5f4065")]
    [ProgId("ASCOM.OpenAstroTracker.Telescope")]
    [ServedClassName("OpenAstroTracker")]
    [ClassInterface(ClassInterfaceType.None)]
    public class Telescope : ReferenceCountedObjectBase, ITelescopeV3 {
        // 
        // Driver ID and descriptive string that shows in the Chooser
        // 
        private string Version = "0.2.0.0b";
        private string _driverId;
        private static string driverDescription = "OpenAstroTracker Telescope";

        internal static double PolarisRAJNow = 0.0;
        internal static DriveRates driveRate = DriveRates.driveSidereal;

        private Util _utilities = new Util();
        private AstroUtils _astroUtilities = new AstroUtils();
        private TraceLogger _tl;
        private Transform _transform = new Transform();

        private bool _isParked;
        private bool _isTracking = true;
        private double _targetRa;
        private double _targetDec;
        private bool _targetRaSet;
        private bool _targetDecSet;
        private readonly Mutex _mutexCommand = new Mutex(false, "CommMutex");
        private bool _isConnected = false;

        private ProfileData Profile => SharedResources.ReadProfile();

        // 
        // Constructor - Must be public for COM registration!
        // 
        public Telescope() {
            _driverId = Marshal.GenerateProgIdForType(this.GetType());

            _tl = SharedResources.tl;
            _tl.Enabled = Profile.TraceState;
            LogMessage("Telescope", $"Starting initialization - v{Version}");

            // TODO: Implement your additional construction here
            _transform.SetJ2000(_utilities.HMSToHours("02:31:51.12"), _utilities.DMSToDegrees("89:15:51.4"));
            _transform.SiteElevation = SiteElevation;
            _transform.SiteLatitude = SiteLatitude;
            _transform.SiteLongitude = SiteLongitude;
            PolarisRAJNow = _transform.RATopocentric;
            LogMessage("Telescope", "Completed initialization");
        }

        // 
        // PUBLIC COM INTERFACE ITelescopeV3 IMPLEMENTATION
        // 

        /// <summary>
        ///     ''' Displays the Setup Dialog form.
        ///     ''' If the user clicks the OK button to dismiss the form, then
        ///     ''' the new settings are saved, otherwise the old values are reloaded.
        ///     ''' THIS IS THE ONLY PLACE WHERE SHOWING USER INTERFACE IS ALLOWED!
        ///     ''' </summary>
        public void SetupDialog() {
            // consider only showing the setup dialog if not connected
            // or call a different dialog if connected
            if (IsConnected) {
                MessageBox.Show("Already connected, just press OK");
            }

            using (var f = new SetupDialogForm(Profile)) {
                if (f.ShowDialog() == DialogResult.OK) {
                    SharedResources.WriteProfile(f
                        .GetProfileData()); // Persist device configuration values to the ASCOM Profile store
                }
            }
        }

        public ArrayList SupportedActions {
            get {
                ArrayList actionList = new ArrayList();
                actionList.Add("Telescope:getFirmwareVer");
                actionList.Add("Utility:JNowtoJ2000");
                actionList.Add("Serial:PassThroughCommand");
                LogMessage("SupportedActions Get",
                    "Returning arraylist of " + actionList.Count.ToString() + " item(s)");
                return actionList;
            }
        }

        public string Action(string ActionName, string ActionParameters) {
            if (SupportedActions.Contains(ActionName)) {
                string retVal = "255"; // Default error code
                switch (ActionName) {
                    case "Telescope:getFirmwareVer": {
                        retVal = CommandString(":GVP"); // Get firmware name
                        retVal = retVal + " " + CommandString(":GVN"); // Get firmware version number
                        break;
                    }

                    case "Utility:JNowtoJ2000": {
                        _transform.SetTopocentric(System.Convert.ToDouble(ActionParameters.Split(',')[0]),
                            System.Convert.ToDouble(ActionParameters.Split(',')[1]));
                        retVal = _utilities.HoursToHMS(_transform.RAJ2000, ":", ":", string.Empty) + "&" +
                                 DegreesToDmsWithSign(_transform.DecJ2000, "*", ":", string.Empty);
                        break;
                    }

                    case "Serial:PassThroughCommand": {
                        var s = ActionParameters.Split(',');
                        retVal = SharedResources.SendPassThroughCommand(s[0], s[1][0]);
                        break;
                    }
                }

                LogMessage("Action(" + ActionName + ", " + ActionParameters + ")", retVal);
                return retVal;
            }
            else
                throw new ActionNotImplementedException("Action " + ActionName + " is not supported by this driver");
        }

        public void CommandBlind(string Command, bool Raw = false) {
            _mutexCommand.WaitOne();

            try {
                SharedResources.SendMessageBlind(Command);
                LogMessage("CommandBlind", "Transmitted " + Command);
            }
            catch (Exception ex) {
                LogMessage("CommandBlind(" + Command + ")", "Error : " + ex.Message);
            }
            finally {
                _mutexCommand.ReleaseMutex();
            }
        }

        public bool CommandBool(string Command, bool Raw = false) {
            throw new System.NotImplementedException();
        }

        public string CommandString(string Command, bool Raw = false) {
            _mutexCommand.WaitOne();

            try {
                var response = SharedResources.SendMessageString(Command);
                LogMessage("CommandString", "Received " + response);
                return response;
            }
            catch (Exception ex) {
                LogMessage("CommandString(" + Command + ")", ex.Message);
                return "255";
            }
            finally {
                _mutexCommand.ReleaseMutex();
            }
        }

        public bool Connected {
            get => IsConnected;
            set {
                if (IsConnected != value) {
                    _isConnected = value;
                    SharedResources.Connected = value;
                }
            }
        }

        public string Description {
            get {
                // this pattern seems to be needed to allow a public property to return a private field
                string d = driverDescription;
                LogMessage("Description Get", d);
                return d;
            }
        }

        public string DriverInfo {
            get {
                // Dim m_version As Version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version
                // Dim s_driverInfo As String = "OpenAstroTracker ASCOM driver version: " + m_version.Major.ToString() + "." + m_version.Minor.ToString() + "." + m_version.Build.ToString()
                string s_driverInfo = "OpenAstroTracker ASCOM driver version: " + Version;
                LogMessage("DriverInfo Get", s_driverInfo);
                return s_driverInfo;
            }
        }

        public string DriverVersion {
            get {
                LogMessage("DriverVersion Get", Version);
                return Version;
            }
        }

        public short InterfaceVersion {
            get {
                LogMessage("InterfaceVersion Get", "3");
                return 3;
            }
        }

        public string Name {
            get {
                string s_name = "OAT ASCOM";
                LogMessage("Name Get", s_name);
                return s_name;
            }
        }

        public void Dispose() {
            Connected = false;
        }


        public void AbortSlew() {
            if (!AtPark) {
                CommandString(":Q");
                LogMessage("AbortSlew", ":Q# Sent");
            }
            else
                throw new ASCOM.ParkedException("AbortSlew");
        }

        public AlignmentModes AlignmentMode {
            get {
                LogMessage("AlignmentMode Get", "1");
                return
                    AlignmentModes
                        .algPolar; // 1 is "Polar (equatorial) mount other than German equatorial." from AlignmentModes Enumeration
            }
        }

        public double Altitude {
            get {
                LogMessage("Altitude", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("Altitude", false);
            }
        }

        public double ApertureArea {
            get {
                LogMessage("ApertureArea Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("ApertureArea", false);
            }
        }

        public double ApertureDiameter {
            get {
                LogMessage("ApertureDiameter Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("ApertureDiameter", false);
            }
        }

        public bool AtHome {
            get {
                // This property must be False if the telescope does not support homing.
                // TODO : We'll try to implement homing later.
                LogMessage("AtHome", "Get - " + false.ToString());
                return false;
            }
        }

        public bool AtPark {
            get {
                LogMessage("AtPark", "Get - " + _isParked.ToString());
                return _isParked; // Custom boolean we added to track parked state
            }
        }

        public IAxisRates AxisRates(TelescopeAxes Axis) {
            LogMessage("AxisRates", "Get - " + Axis.ToString());
            return new AxisRates(Axis);
        }

        public double Azimuth {
            get {
                LogMessage("Azimuth Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("Azimuth", false);
            }
        }

        public bool CanFindHome {
            get {
                if (!IsConnected)
                    throw new ASCOM.NotConnectedException("CanFindHome");
                LogMessage("CanFindHome", "Get - " + false);
                return false;
            }
        }

        public bool CanMoveAxis(TelescopeAxes Axis) {
            LogMessage("CanMoveAxis", "Get - " + Axis);
            
            switch (Axis) {
                case TelescopeAxes.axisPrimary:
                case TelescopeAxes.axisSecondary:
                    return true;
                default:
                    return false;
            }
        }

        public bool CanPark {
            get {
                LogMessage("CanPark", "Get - " + true.ToString());
                return true;
            }
        }

        public bool CanPulseGuide {
            get {
                LogMessage("CanPulseGuide", "Get - " + true.ToString());
                return true;
            }
        }

        public bool CanSetDeclinationRate {
            get {
                LogMessage("CanSetDeclinationRate", "Get - " + false.ToString());
                return false;
            }
        }

        public bool CanSetGuideRates {
            get {
                LogMessage("CanSetGuideRates", "Get - " + false.ToString());
                return false;
            }
        }

        public bool CanSetPark {
            // ToDo  We should allow this
            get {
                LogMessage("CanSetPark", "Get - " + false.ToString());
                return false;
            }
        }

        public bool CanSetPierSide {
            get {
                LogMessage("CanSetPierSide", "Get - " + false.ToString());
                return false;
            }
        }

        public bool CanSetRightAscensionRate {
            get {
                LogMessage("CanSetRightAscensionRate", "Get - " + false.ToString());
                return false;
            }
        }

        public bool CanSetTracking {
            get {
                LogMessage("CanSetTracking", "Get - " + true.ToString());
                return true;
            }
        }

        public bool CanSlew {
            get {
                LogMessage("CanSlew", "Get - " + true.ToString());
                return true;
            }
        }

        public bool CanSlewAltAz {
            // TODO - AltAz slewing
            get {
                LogMessage("CanSlewAltAz", "Get - " + false.ToString());
                return false;
            }
        }

        public bool CanSlewAltAzAsync {
            get {
                LogMessage("CanSlewAltAzAsync", "Get - " + false.ToString());
                return false;
            }
        }

        public bool CanSlewAsync {
            get {
                LogMessage("CanSlewAsync", "Get - " + true.ToString());
                return true;
            }
        }

        public bool CanSync {
            get {
                LogMessage("CanSync", "Get - " + true.ToString());
                return true;
            }
        }

        public bool CanSyncAltAz {
            get {
                LogMessage("CanSyncAltAz", "Get - " + false.ToString());
                return false;
            }
        }

        public bool CanUnpark {
            get {
                LogMessage("CanUnpark", "Get - " + true.ToString());
                return true;
            }
        }

        public double Declination {
            get {
                double declination__1 = 0.0;
                string scopeDec = CommandString(":GD");
                LogMessage("Declination", "Get - " + scopeDec);
                declination__1 = _utilities.DMSToDegrees(scopeDec);
                return declination__1;
            }
        }

        public double DeclinationRate {
            get {
                double declination = 0.0;
                LogMessage("DeclinationRate", "Get - " + declination.ToString());
                return declination;
            }
            set {
                LogMessage("DeclinationRate Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("DeclinationRate", true);
            }
        }

        public PierSide DestinationSideOfPier(double RightAscension, double Declination) {
            LogMessage("DestinationSideOfPier Get", "Not implemented");
            throw new ASCOM.MethodNotImplementedException("DestinationSideOfPier");
        }

        public bool DoesRefraction {
            get {
                LogMessage("DoesRefraction Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("DoesRefraction", false);
            }
            set {
                LogMessage("DoesRefraction Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("DoesRefraction", true);
            }
        }

        public EquatorialCoordinateType EquatorialSystem {
            // TODO : Determine if we're using JNow or J2000, or can use both.  Work on this.
            get {
                EquatorialCoordinateType equatorialSystem__1 = EquatorialCoordinateType.equTopocentric;
                LogMessage("DeclinationRate", "Get - " + equatorialSystem__1.ToString());
                return equatorialSystem__1;
            }
        }

        public void FindHome() {
            LogMessage("FindHome", "Not implemented");
            throw new ASCOM.MethodNotImplementedException("FindHome");
        }

        public double FocalLength {
            get {
                LogMessage("FocalLength Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("FocalLength", false);
            }
        }

        public double GuideRateDeclination {
            get {
                LogMessage("GuideRateDeclination Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("GuideRateDeclination", false);
            }
            set {
                LogMessage("GuideRateDeclination Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("GuideRateDeclination", true);
            }
        }

        public double GuideRateRightAscension {
            get {
                LogMessage("GuideRateRightAscension Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("GuideRateRightAscension", false);
            }
            set {
                LogMessage("GuideRateRightAscension Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("GuideRateRightAscension", true);
            }
        }

        public bool IsPulseGuiding {
            get {
                bool retVal = Convert.ToBoolean(System.Convert.ToInt32(CommandString(":GIG")));
                LogMessage("isPulseGuiding Get", retVal.ToString());
                return retVal;
            }
        }


        private bool _trackingPriorToMove;
        public void MoveAxis(TelescopeAxes Axis, double Rate) {
            if (Axis == TelescopeAxes.axisTertiary) {
                throw new ASCOM.NotImplementedException("MoveAxis Tertiary Not Supported.");
            }
            
            if (AtPark) {
                LogMessage("MoveAxis", "Parked");
                throw new ASCOM.ParkedException("MoveAxis");
            }

            if (Rate != 0 && !ValidAxisSpeed(Axis, Rate)) {
                throw new ASCOM.InvalidValueException("Invalid speed for Axis");
            }


            var sAxis = Enum.GetName(typeof(TelescopeAxes), Axis);
            string cmd = "Q";

            LogMessage("MoveAxis", $"{sAxis} Rate {Rate}");


            if (Rate == 0) {
                // if at some point we support multiple tracking rates this should set
                // the value back to the previous rate...
                CommandBlind($":{cmd}");
                Tracking = _trackingPriorToMove;
            }
            else {
                _trackingPriorToMove = Tracking;
                switch (Axis)
                {
                    case TelescopeAxes.axisPrimary:
                        cmd = Rate > 0 ? "Mw" : "Me";
                        break;
                    case TelescopeAxes.axisSecondary:
                        cmd = Rate > 0 ? "Mn" : "Ms";
                        break;
                }
                CommandBlind($":{cmd}");
            }
        }

        private bool ValidAxisSpeed(TelescopeAxes axis, double reqRate) {
            var rates = AxisRates(axis);
            var absRate = Math.Abs(reqRate);
            foreach (Rate rate in rates) {
                if (absRate >= rate.Minimum && absRate <= rate.Maximum) {
                    return true;
                }
            }

            return false;
        }

        public void Park() {
            if (!AtPark) {
                // LogMessage("Park", "Err : Mount already parked")
                // Throw New ASCOM.ParkedException("Already Parked")
                // Else
                CommandString(":hP", false);
                PollUntilZero(":GIS");
                _isParked = true;
                LogMessage("Park", "Parked mount");
            }
        }

        public void PulseGuide(GuideDirections Direction, int Duration) {
            if (!AtPark) {
                var dirString = Enum.GetName(typeof(GuideDirections), Direction);
                var durString = Duration.ToString("0000");
                LogMessage($"PulseGuide", $"{Direction} {durString}");
                var dir = dirString.Substring(5, 1);
                CommandBlind($":MG{dir}{durString}", false);
            }
            else {
                LogMessage("PulseGuide", "Parked");
                throw new ASCOM.ParkedException("PulseGuide");
            }
        }

        public double RightAscension {
            get {
                double rightAscension__1 = 0.0;
                rightAscension__1 = _utilities.HMSToHours(CommandString(":GR"));
                LogMessage("RightAscension", rightAscension__1.ToString());
                return rightAscension__1;
            }
        }

        public double RightAscensionRate {
            get {
                double rightAscensionRate__1 = 0.0;
                LogMessage("RightAscensionRate", "Get - " + rightAscensionRate__1.ToString());
                return rightAscensionRate__1;
            }
            set {
                LogMessage("RightAscensionRate Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("RightAscensionRate", true);
            }
        }

        public void SetPark() {
            LogMessage("SetPark", "Not implemented");
            throw new ASCOM.MethodNotImplementedException("SetPark");
        }

        public PierSide SideOfPier {
            get {
                PierSide retVal;
                if (SiderealTime < 12) {
                    if (RightAscension >= SiderealTime && RightAscension <= SiderealTime + 12)
                        retVal = PierSide.pierWest;
                    else
                        retVal = PierSide.pierEast;
                }
                else if (RightAscension <= SiderealTime && RightAscension >= SiderealTime - 12)
                    retVal = PierSide.pierEast;
                else
                    retVal = PierSide.pierWest;

                LogMessage("SideOfPier Get", Enum.GetName(typeof(PierSide), retVal));
                return retVal;
            }
            set {
                LogMessage("SideOfPier Set", "Not Implemented");
                throw new ASCOM.PropertyNotImplementedException("SideOfPier", true);
            }
        }

        public double SiderealTime {
            get {
                // now using novas 3.1
                double lst = 0.0;
                using (ASCOM.Astrometry.NOVAS.NOVAS31 novas = new ASCOM.Astrometry.NOVAS.NOVAS31()) {
                    double jd = _utilities.DateUTCToJulian(DateTime.UtcNow);
                    novas.SiderealTime(jd, 0, novas.DeltaT(jd), ASCOM.Astrometry.GstType.GreenwichMeanSiderealTime,
                        ASCOM.Astrometry.Method.EquinoxBased, ASCOM.Astrometry.Accuracy.Reduced, ref lst);
                }

                // Allow for the longitude
                lst += SiteLongitude / 360.0 * 24.0;

                // Reduce to the range 0 to 24 hours
                lst = _astroUtilities.ConditionRA(lst);

                LogMessage("SiderealTime", "Get - " + lst.ToString());
                return lst;
            }
        }

        public double SiteElevation {
            get {
                LogMessage("SiteElevation Get", Profile.Elevation.ToString());
                return Profile.Elevation;
            }
            set {
                if (value >= -300 && value <= 10000) {
                    LogMessage("SiteElevation Set", value.ToString());
                    Profile.Elevation = value;
                }
                else
                    throw new ASCOM.InvalidValueException("SiteElevation");
            }
        }

        public double SiteLatitude {
            get {
                LogMessage("SiteLatitude Get", Profile.Latitude.ToString());
                return Profile.Latitude;
            }
            set {
                LogMessage("SiteLatitude Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("SiteLatitude", true);
            }
        }

        public double SiteLongitude {
            get {
                LogMessage("SiteLongitude Get", Profile.Longitude.ToString());
                return Profile.Longitude;
            }
            set {
                LogMessage("SiteLongitude Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("SiteLongitude", true);
            }
        }

        public short SlewSettleTime {
            get {
                LogMessage("SlewSettleTime Get", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("SlewSettleTime", false);
            }
            set {
                LogMessage("SlewSettleTime Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("SlewSettleTime", true);
            }
        }

        public void SlewToAltAz(double Azimuth, double Altitude) {
            LogMessage("SlewToAltAz", "Not implemented");
            throw new ASCOM.MethodNotImplementedException("SlewToAltAz");
        }

        public void SlewToAltAzAsync(double Azimuth, double Altitude) {
            LogMessage("SlewToAltAzAsync", "Not implemented");
            throw new ASCOM.MethodNotImplementedException("SlewToAltAzAsync");
        }

        public void SlewToCoordinates(double ra, double dec) =>
            SlewToCoordinatesWithWait("SlewToCoordinates", ra, dec, true);

        private void SlewToCoordinatesWithWait(string caller, double ra, double dec, bool wait) {
            if (AtPark) {
                LogMessage(caller, "Parked");
                throw new ASCOM.ParkedException(caller);
            }

            if (ValidateCoordinates(ra, dec)) {
                LogMessage(caller, "RA " + ra.ToString() + ", Dec " + dec.ToString());
                SetTargetCoordinates(ra, dec);
                CommandString(":MS");
                if (wait) {
                    PollUntilZero(":GIS");
                }
            }
            else {
                LogMessage(caller, "Invalid coordinates RA: " + ra.ToString() + ", Dec: " + dec.ToString());
                throw new ASCOM.InvalidValueException(caller);
            }
        }

        private bool SetTargetCoordinates(double rightAscension, double declination) {
            var strRAcmd = ":Sr" + _utilities.HoursToHMS(rightAscension, ":", ":");
            var strDeccmd = $":Sd" + DegreesToDmsWithSign(declination, "*", ":", "");
            LogMessage("SetTargetCoordinates", strRAcmd);
            LogMessage("SetTargetCoordinates", strDeccmd);

            return CommandString(strRAcmd) == "1" && CommandString(strDeccmd) == "1";
        }

        public void SlewToCoordinatesAsync(double ra, double dec) =>
            SlewToCoordinatesWithWait("SlewToCoordinatesAsync", ra, dec, false);

        public void SlewToTarget() {
            if (_targetRaSet && _targetDecSet) {
                LogMessage("SlewToTarget", TargetRightAscension.ToString() + ", " + TargetDeclination.ToString());
                SlewToCoordinates(TargetRightAscension, TargetDeclination);
            }
            else {
                throw new InvalidValueException("TargetRightAscension or TargetDeclination are not set or valid");
            }
        }

        public void SlewToTargetAsync() {
            if (_targetRaSet && _targetDecSet) {
                LogMessage("SlewToTargetAsync", TargetRightAscension.ToString() + ", " + TargetDeclination.ToString());
                SlewToCoordinatesAsync(TargetRightAscension, TargetDeclination);
            }
            else {
                throw new InvalidValueException("TargetRightAscension or TargetDeclination are not set or valid");
            }
        }

        public bool Slewing {
            get {
                bool retVal = Convert.ToBoolean(System.Convert.ToInt32(CommandString(":GIS")));
                LogMessage("Slewing Get", retVal.ToString());
                return retVal;
            }
        }

        public void SyncToAltAz(double Azimuth, double Altitude) {
            LogMessage("SyncToAltAz", "Not implemented");
            throw new ASCOM.MethodNotImplementedException("SyncToAltAz");
        }

        public void SyncToCoordinates(double RightAscension, double Declination) {
            if (AtPark) {
                LogMessage("SyncToCoordinates", "Parked");
                throw new ASCOM.ParkedException("SyncToCoordinates");
            }

            if (ValidateCoordinates(RightAscension, Declination)) {
                LogMessage("SyncToCoordinates", "RA " + RightAscension.ToString() + ", Dec " + Declination.ToString());
                SetTargetCoordinates(RightAscension, Declination);
                CommandBlind(":CM");
            }
            else {
                LogMessage("SyncToCoordinates",
                    "Invalid coordinates RA: " + RightAscension.ToString() + ", Dec: " + Declination.ToString());
                throw new ASCOM.InvalidValueException("SyncToCoordinates");
            }
        }

        private bool ValidateCoordinates(double rightAscension, double declination) {
            return rightAscension <= 24 && rightAscension >= 0 && declination >= -90 && declination <= 90;
        }

        public void SyncToTarget() {
            if (_targetDecSet && _targetRaSet) {
                SyncToCoordinates(TargetRightAscension, TargetDeclination);
            }
        }


        public double TargetDeclination {
            get {
                if (_targetDecSet) {
                    LogMessage("TargetDeclination Get", _targetDec.ToString());
                    return _targetDec;
                }
                else {
                    LogMessage("TargetDeclination Get", "Value not set");
                    throw new ASCOM.ValueNotSetException("TargetDeclination");
                }
            }
            set {
                if (value >= -90 && value <= 90) {
                    LogMessage("TargetDeclination Set", value.ToString());
                    _targetDec = value;
                    _targetDecSet = true;
                }
                else {
                    LogMessage("TargetDeclination Set", "Invalid Value " + value.ToString());
                    throw new ASCOM.InvalidValueException("TargetDeclination");
                }
            }
        }

        public double TargetRightAscension {
            get {
                if (_targetRaSet) {
                    LogMessage("TargetRightAscension Get", _targetRa.ToString());
                    return _targetRa;
                }
                else {
                    LogMessage("TargetRightAscension Get", "Value not set");
                    throw new ASCOM.ValueNotSetException("TargetRightAscension");
                }
            }
            set {
                if (value >= 0 && value <= 24) {
                    LogMessage("TargetRightAscension Set", value.ToString());
                    _targetRa = value;
                    _targetRaSet = true;
                }
                else {
                    LogMessage("TargetRightAscension Set", "Invalid Value " + value.ToString());
                    throw new ASCOM.InvalidValueException("TargetRightAscension");
                }
            }
        }

        public bool Tracking {
            get {
                if (CommandString(":GIT", false) == "0") {
                    _isTracking = false;
                    LogMessage("Tracking", "Get - " + false.ToString());
                }
                else {
                    _isTracking = true;
                    LogMessage("Tracking", "Get - " + true.ToString());
                }

                return _isTracking;
            }
            set {
                if (CommandString(":MT" + Convert.ToInt32(value).ToString(), false) == "1") {
                    _isTracking = value;
                    LogMessage("Tracking Set", value.ToString());
                }
                else {
                    LogMessage("Tracking Set", "Error");
                    throw new ASCOM.DriverException("Error setting tracking state");
                }
            }
        }

        public DriveRates TrackingRate {
            get {
                LogMessage("TrackingRate Get", Enum.GetName(typeof(DriveRates), driveRate));
                return driveRate;
            }
            set {
                LogMessage("TrackingRate Set", "Not implemented");
                throw new ASCOM.PropertyNotImplementedException("TrackingRate", true);
            }
        }

        public ITrackingRates TrackingRates {
            get {
                ITrackingRates trackingRates__1 = new TrackingRates();
                LogMessage("TrackingRates", "Get - ");
                foreach (DriveRates driveRate in trackingRates__1)
                    LogMessage("TrackingRates", "Get - " + driveRate.ToString());
                return trackingRates__1;
            }
        }

        public DateTime UTCDate {
            // ToDo - Can we handle this without bothering the mount?
            get {
                DateTime utcDate__1 = DateTime.UtcNow;
                LogMessage("UTCDate", string.Format("Get - {0}", utcDate__1));
                return utcDate__1;
            }
            set { throw new ASCOM.PropertyNotImplementedException("UTCDate", true); }
        }

        public void Unpark() {
            if (AtPark) {
                // LogMessage("Unpark", "Err : Mount not parked")
                // Throw New ASCOM.DriverException("Mount not parked")
                // Else
                string unprkRet = CommandString(":hU", false);
                if (unprkRet == "1")
                    LogMessage("Unpark", "Unparked mount");
                _isParked = false;
            }
        }

        private string DegreesToDmsWithSign(double degrees, string degSep = "°", string minSep = "'",
            string secondSep = "\"") {
            return (degrees >= 0 ? "+" : "") + _utilities.DegreesToDMS(degrees, degSep, minSep, secondSep);
        }


        // here are some useful properties and methods that can be used as required
        // to help with

#if INPROCESS
        private static void RegUnregASCOM(bool bRegister) {
            using (Profile P = new Profile() {DeviceType = "Telescope"}) {
                if (bRegister)
                    P.Register(driverID, driverDescription);
                else
                    P.Unregister(driverID);
            }
        }

        [ComRegisterFunction()]
        public static void RegisterASCOM(Type T) {
            RegUnregASCOM(true);
        }

        [ComUnregisterFunction()]
        public static void UnregisterASCOM(Type T) {
            RegUnregASCOM(false);
        }
#endif

        /// <summary>
        ///     ''' Returns true if there is a valid connection to the driver hardware
        ///     ''' </summary>
        // TODO check that the driver hardware connection exists and is connected to the hardware
        private bool IsConnected => SharedResources.Connected && _isConnected;

        /// <summary>
        ///     ''' Use this function to throw an exception if we aren't connected to the hardware
        ///     ''' </summary>
        ///     ''' <param name="message"></param>
        private void CheckConnected(string message) {
            if (!IsConnected)
                throw new NotConnectedException(message);
        }

        private int PollUntilZero(string command) {
            // Takes a command to be sent via CommandString, and resends every 1000ms until a 0 is returned.  Returns 0 only when complete.
            string retVal = "";
            while (retVal != "0") {
                retVal = CommandString(command);
                Thread.Sleep(1000);
            }

            return System.Convert.ToInt32(retVal);
        }

        private void LogMessage(string identifier, string message) {
            Debug.WriteLine($"{identifier} - {message}");
            _tl.LogMessage(identifier, message);
        }
    }
}