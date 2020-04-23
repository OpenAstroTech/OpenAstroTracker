//
// ================
// Shared Resources
// ================
//
// This class is a container for all shared resources that may be needed
// by the drivers served by the Local Server. 
//
// NOTES:
//
//	* ALL DECLARATIONS MUST BE STATIC HERE!! INSTANCES OF THIS CLASS MUST NEVER BE CREATED!
//
// Written by:	Bob Denny	29-May-2007
// Modified by Chris Rowland and Peter Simpson to hamdle multiple hardware devices March 2011
//

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Threading;
using ASCOM.Utilities;


namespace ASCOM.OpenAstroTracker {
    /// <summary>
    /// The resources shared by all drivers and devices, in this example it's a serial port with a shared SendMessage method
    /// an idea for locking the message and handling connecting is given.
    /// In reality extensive changes will probably be needed.
    /// Multiple drivers means that several applications connect to the same hardware device, aka a hub.
    /// Multiple devices means that there are more than one instance of the hardware, such as two focusers.
    /// In this case there needs to be multiple instances of the hardware connector, each with it's own connection count.
    /// </summary>
    public static class SharedResources {
        // object used for locking to prevent multiple drivers accessing common code at the same time
        private static readonly object lockObject = new object();
        private static TraceLogger traceLogger;
        private static Profile driverProfile;
        public static string driverID = "ASCOM.OpenAstroTracker.Telescope";

        private static string comPortProfileName = "COM Port";
        private static string traceStateProfileName = "Trace Level";
        private static string latitudeProfileName = "Latitude";
        private static string longitudeProfileName = "Longitude";
        private static string elevationProfileName = "Elevation";

        private static string comPortDefault = "COM5";
        private static string traceStateDefault = "True";
        private static double latitudeDefault = 30;
        private static double longitudeDefault = -97;
        private static double elevationDefault = 1;

        //
        // Public access to shared resources
        //

        public static TraceLogger tl {
            get {
                if (traceLogger == null) {
                    traceLogger = new TraceLogger("", "OpenAstroTracker.LocalServer");
                    traceLogger.Enabled = true;
                }

                return traceLogger;
            }
        }

        public static ProfileData ReadProfile() {
            using (Profile driverProfile = new Profile()) {
                driverProfile.DeviceType = "Telescope";
                return new ProfileData {
                    TraceState = Convert.ToBoolean(driverProfile.GetValue(driverID, traceStateProfileName, String.Empty, traceStateDefault)),
                    ComPort = driverProfile.GetValue(driverID, comPortProfileName, string.Empty, comPortDefault),
                    Latitude = Convert.ToDouble(driverProfile.GetValue(driverID, latitudeProfileName, string.Empty, latitudeDefault.ToString())),
                    Longitude = Convert.ToDouble(driverProfile.GetValue(driverID, longitudeProfileName, string.Empty, longitudeDefault.ToString())),
                    Elevation = Convert.ToDouble(driverProfile.GetValue(driverID, elevationProfileName, string.Empty, elevationDefault.ToString()))
                };
            }
        }

        public static void WriteProfile(ProfileData profile) {
            using (Profile driverProfile = new Profile()) {
                driverProfile.DeviceType = "Telescope";
                driverProfile.WriteValue(driverID, traceStateProfileName, profile.TraceState.ToString());
                driverProfile.WriteValue(driverID, comPortProfileName, profile.ComPort.ToString());
                driverProfile.WriteValue(driverID, latitudeProfileName, profile.Latitude.ToString());
                driverProfile.WriteValue(driverID, longitudeProfileName, profile.Longitude.ToString());
                driverProfile.WriteValue(driverID, elevationProfileName, profile.Elevation.ToString());
            }
        }

        #region single serial port connector

        //
        // this region shows a way that a single serial port could be connected to by multiple 
        // drivers.
        //
        // Connected is used to handle the connections to the port.
        //
        // SendMessage is a way that messages could be sent to the hardware without
        // conflicts between different drivers.
        //
        // All this is for a single connection, multiple connections would need multiple ports
        // and a way to handle connecting and disconnection from them - see the
        // multi driver handling section for ideas.
        //

        /// <summary>
        /// Shared serial port
        /// </summary>
        public static Serial SharedSerial { get; } = new ASCOM.Utilities.Serial();

        /// <summary>
        /// number of connections to the shared serial port
        /// </summary>
        public static int Connections { get; set; } = 0;

        /// <summary>
        /// Example of a shared SendMessage method, the lock
        /// prevents different drivers tripping over one another.
        /// It needs error handling and assumes that the message will be sent unchanged
        /// and that the reply will always be terminated by a "#" character.
        /// </summary>
        /// <param name="message"></param>
        /// <returns></returns>
        private static string SendMessage(string message, Boolean reply) {
            lock (lockObject) {
                tl.LogMessage("OAT Server", "Lock Object");
                if (!message.EndsWith("#")) {
                    message += "#";
                }

                if (SharedSerial.Connected && !String.IsNullOrEmpty(message)) {
                    tl.LogMessage("Telescope", "Send message: " + message);
                    SharedSerial.ClearBuffers();
                    SharedSerial.Transmit(message);
                    if (reply) {
                        string retVal;
                        string cmdGroup = message.Substring(1, 1);
                        switch (cmdGroup) {
                            case "S":
                            case "M":
                            case "h":
                            case "Q":
                                retVal = SharedSerial.ReceiveCounted(1);
                                break;
                            default:
                                retVal = SharedSerial.ReceiveTerminated("#");
                                retVal = retVal.Replace("#", "");
                                break;
                        }

                        return retVal;
                    }
                    else return "";
                }
                else {
                    tl.LogMessage("OAT Server", "Not connected or Empty Message: " + message);
                    return "";
                }
            }
        }




        /// <summary>
        /// Example of handling connecting to and disconnection from the
        /// shared serial port.
        /// Needs error handling
        /// the port name etc. needs to be set up first, this could be done by the driver
        /// checking Connected and if it's false setting up the port before setting connected to true.
        /// It could also be put here.
        /// </summary>
        public static bool Connected {
            set {
                lock (lockObject) {
                    if (value) {
                        if (Connections == 0) {
                            try {
                                SharedSerial.DTREnable = false;
                                SharedSerial.PortName = ReadProfile().ComPort;
                                SharedSerial.ReceiveTimeoutMs = 2000;
                                SharedSerial.Speed = ASCOM.Utilities.SerialSpeed.ps57600;
                                SharedSerial.Connected = true;

                                Thread.Sleep(250);
                                SharedSerial.Transmit(":I#");
                            }
                            catch (System.IO.IOException exception) {
                                MessageBox.Show("Serial port not opened for " + SharedResources.SharedSerial.PortName,
                                    "Invalid port state", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                SharedResources.tl.LogMessage("Serial port not opened", exception.Message);
                            }
                            catch (System.UnauthorizedAccessException exception) {
                                MessageBox.Show("Access denied to serial port " + SharedResources.SharedSerial.PortName,
                                    "Access denied", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                SharedResources.tl.LogMessage("Access denied to serial port", exception.Message);
                            }
                            catch (ASCOM.DriverAccessCOMException exception) {
                                MessageBox.Show("ASCOM driver exception: " + exception.Message,
                                    "ASCOM driver exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            }
                            catch (System.Runtime.InteropServices.COMException exception) {
                                MessageBox.Show(
                                    "Serial port read timeout for port " + SharedResources.SharedSerial.PortName,
                                    "Timeout", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                SharedResources.tl.LogMessage("Serial port read timeout", exception.Message);
                            }
                        }
                        Connections++;
                        tl.LogMessage("Connected Set", $"{value} - Connection Count is {Connections} Clients");
                    }
                    else {
                        Connections--;
                        tl.LogMessage("Connected Set", $"{value} - Connection Count is {Connections} Clients");
                        if (Connections <= 0) {
                            Connections = 0;
                            tl.LogMessage("Connection Set", $"Connection Count is {Connections} Disconnecting From Device");
                            SharedSerial.Transmit(":Qq#");
                            SharedSerial.Connected = false;
                        }
                    }
                }
            }
            get => SharedSerial.Connected;
        }

        #endregion

        /// <summary>
        /// Skeleton of a hardware class, all this does is hold a count of the connections,
        /// in reality extra code will be needed to handle the hardware in some way
        /// </summary>
        public class DeviceHardware {
            internal int count { set; get; }

            internal DeviceHardware() {
                count = 0;
            }
        }

        public static string SendMessageString(string message) {
            try {
                string answer = SharedResources.SendMessage(message, true);
                return answer;
            }
            catch {
                return "";
            }
        }

        //Add new public methods here

        public static void SendMessageBlind(string message) {
            try {
                string answer = SharedResources.SendMessage(message, false);
            }
            catch { }
        }
    }
}