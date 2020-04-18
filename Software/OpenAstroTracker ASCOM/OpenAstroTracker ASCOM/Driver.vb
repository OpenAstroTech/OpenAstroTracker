Imports System.Threading

'tabs=4
' --------------------------------------------------------------------------------
'
' ASCOM Telescope driver for OpenAstroTracker
'
' Implements:	ASCOM Telescope interface version: 1.0
'
' Driver ID : ASCOM.OpenAstroTracker.Telescope
'
' This definition is used to select code that's only applicable for one device type
'
#Const Device = "Telescope"

Imports ASCOM
Imports ASCOM.Astrometry
Imports ASCOM.Astrometry.AstroUtils
Imports ASCOM.DeviceInterface
Imports ASCOM.Utilities
Imports ASCOM.Astrometry.Transform

Imports System
Imports System.Collections
Imports System.Collections.Generic
Imports System.Globalization
Imports System.Runtime.InteropServices
Imports System.Text

<Guid("7508f019-52bc-4b4a-90d6-4a7fe8c5ea75")> _
<ClassInterface(ClassInterfaceType.None)> _
Public Class Telescope

    ' The Guid attribute sets the CLSID for ASCOM.OpenAstroTracker.Telescope
    ' The ClassInterface/None addribute prevents an empty interface called
    ' _OpenAstroTracker from being created and used as the [default] interface

    Implements ITelescopeV3

    '
    ' Driver ID and descriptive string that shows in the Chooser
    '
    Private Version As String = "0.1.4.1b"
    Friend Shared driverID As String = "ASCOM.OpenAstroTracker.Telescope"
    Private Shared driverDescription As String = "OpenAstroTracker Telescope"

    Friend Shared comPortProfileName As String = "COM Port" 'Constants used for Profile persistence
    Friend Shared traceStateProfileName As String = "Trace Level"
    Friend Shared latitudeProfileName As String = "Latitude"
    Friend Shared longitudeProfileName As String = "Longitude"
    Friend Shared elevationProfileName As String = "Elevation"

    Friend Shared comPortDefault As String = "COM1"
    Friend Shared traceStateDefault As String = "False"
    Friend Shared latitudeDefault As Double = 39.8283
    Friend Shared longitudeDefault As Double = -98.5795
    Friend Shared elevationDefault As Integer = 1

    Friend Shared comPort As String ' Variables to hold the currrent device configuration
    Friend Shared portNum As String
    Friend Shared traceState As Boolean
    Friend Shared latitude As Double
    Friend Shared longitude As Double
    Friend Shared elevation As Integer
    Friend Shared PolarisRAJNow As Double = 0.0


    Private connectedState As Boolean ' Private variable to hold the connected state
    Private utilities As Util ' Private variable to hold an ASCOM Utilities object
    Private astroUtilities As AstroUtils ' Private variable to hold an AstroUtils object to provide the Range method
    Private TL As TraceLogger ' Private variable to hold the trace logger object (creates a diagnostic log file with information that you specify)
    Private objSerial As ASCOM.Utilities.Serial
    Public transform As ASCOM.Astrometry.Transform.Transform
    Private connWait As Boolean = False     ' This is a lame hack to be able to transmit things w/ CommandString before we set connected to true.

    Private isParked As Boolean = False, isTracking As Boolean = True
    Private targetRA As Double, targetDec As Double, targetRASet As Boolean = False, targetDecSet As Boolean = False
    Dim mutexBlind As Mutex, mutexCommand As Mutex

    '
    ' Constructor - Must be public for COM registration!
    '
    Public Sub New()

        ReadProfile() ' Read device configuration from the ASCOM Profile store
        TL = New TraceLogger("", "OpenAstroTracker")
        TL.Enabled = traceState
        TL.LogMessage("Telescope", "Starting initialization")

        connectedState = False ' Initialise connected to false
        utilities = New Util() ' Initialise util object
        astroUtilities = New AstroUtils 'Initialise new astro utiliites object

        'TODO: Implement your additional construction here
        mutexCommand = New Mutex(False, "CommMutex")
        transform = New Transform
        transform.SetJ2000(utilities.HMSToHours("02:31:51.12"), utilities.DMSToDegrees("89:15:51.4"))
        transform.SiteElevation = SiteElevation
        transform.SiteLatitude = SiteLatitude
        transform.SiteLongitude = SiteLongitude
        PolarisRAJNow = transform.RATopocentric
        TL.LogMessage("Telescope", "Completed initialization")
    End Sub

    '
    ' PUBLIC COM INTERFACE ITelescopeV3 IMPLEMENTATION
    '

#Region "Common properties and methods"
    ''' <summary>
    ''' Displays the Setup Dialog form.
    ''' If the user clicks the OK button to dismiss the form, then
    ''' the new settings are saved, otherwise the old values are reloaded.
    ''' THIS IS THE ONLY PLACE WHERE SHOWING USER INTERFACE IS ALLOWED!
    ''' </summary>
    Public Sub SetupDialog() Implements ITelescopeV3.SetupDialog
        ' consider only showing the setup dialog if not connected
        ' or call a different dialog if connected
        If IsConnected Then
            System.Windows.Forms.MessageBox.Show("Already connected, just press OK")
        End If

        Using F As SetupDialogForm = New SetupDialogForm()
            Dim result As System.Windows.Forms.DialogResult = F.ShowDialog()
            If result = DialogResult.OK Then
                WriteProfile() ' Persist device configuration values to the ASCOM Profile store
            End If
        End Using
    End Sub

    Public ReadOnly Property SupportedActions() As ArrayList Implements ITelescopeV3.SupportedActions
        Get
            Dim actionList As New ArrayList
            actionList.Add("Telescope:getFirmwareVer")
            actionList.Add("Utility:JNowtoJ2000")
            TL.LogMessage("SupportedActions Get", "Returning arraylist of " + actionList.Count.ToString + " item(s)")
            Return actionList
        End Get
    End Property

    Public Function Action(ByVal ActionName As String, ByVal ActionParameters As String) As String Implements ITelescopeV3.Action
        Dim suppAct As ArrayList = SupportedActions()
        If suppAct.Contains(ActionName) Then
            Dim retVal As String = "255"        ' Default error code
            Select Case ActionName
                Case "Telescope:getFirmwareVer"
                    retVal = CommandString(":GVP")                  ' Get firmware name
                    retVal = retVal + " " + CommandString(":GVN")   ' Get firmware version number
                Case "Utility:JNowtoJ2000"
                    transform.SetTopocentric(CDbl(ActionParameters.Split(",")(0)), CDbl(ActionParameters.Split(",")(1)))
                    retVal = utilities.HoursToHMS(transform.RAJ2000, ":", ":", String.Empty) + "&" + utilities.DegreesToDMS(transform.DecJ2000, "*", ":", String.Empty)
            End Select
            TL.LogMessage("Action(" + ActionName + ", " + ActionParameters + ")", retVal)
            Return retVal
        Else
            Throw New ActionNotImplementedException("Action " & ActionName & " is not supported by this driver")
        End If

    End Function

    Public Sub CommandBlind(ByVal Command As String, Optional ByVal Raw As Boolean = False) Implements ITelescopeV3.CommandBlind
        If Not connWait Then
            CheckConnected("CommandBlind")
        End If
        mutexCommand.WaitOne()

        If Not Raw Then
            Command = Command + "#"
        End If
        Try
            objSerial.Transmit(Command)
            TL.LogMessage("CommandBlind", "Transmitted " & Command)
        Catch ex As Exception
            TL.LogMessage("CommandBlind(" + Command + ")", "Error : " + ex.Message)
        Finally
            mutexCommand.ReleaseMutex()
        End Try
    End Sub

    Public Function CommandBool(ByVal Command As String, Optional ByVal Raw As Boolean = False) As Boolean _
        Implements ITelescopeV3.CommandBool
        'CheckConnected("CommandBool")
        'Dim ret As String = CommandString(Command, Raw)
        ' TODO decode the return string and return true or false
        Throw New MethodNotImplementedException("CommandBool")
    End Function

    Public Function CommandString(ByVal Command As String, Optional ByVal Raw As Boolean = False) As String _
        Implements ITelescopeV3.CommandString
        Dim response As String
        If Not connWait Then
            CheckConnected("CommandString")
        End If
        mutexCommand.WaitOne()
        If Not Raw Then
            Command = Command + "#"
        End If
        Try
            objSerial.Transmit(Command)
            TL.LogMessage("CommandString", "Transmitted " & Command)
            Dim cmdGroup As String = Command.Substring(1, 1)
            Select Case cmdGroup
                Case "S", "M", "h", "Q"
                    response = objSerial.ReceiveCounted(1)
                    'Case "M"
                    '    response = objSerial.ReceiveCounted(1)
                    'Case "h"
                    '    response = objSerial.ReceiveCounted(1)
                Case Else
                    response = objSerial.ReceiveTerminated("#")
                    response = response.Replace("#", "")
            End Select
            TL.LogMessage("CommandString", "Received " & response)
            Return response
        Catch ex As Exception
            TL.LogMessage("CommandString(" + Command + ")", ex.Message)
            Return "255"
        Finally
            mutexCommand.ReleaseMutex()
        End Try
    End Function

    Public Property Connected() As Boolean Implements ITelescopeV3.Connected
        Get
            TL.LogMessage("Connected Get", IsConnected.ToString())
            Return IsConnected
        End Get
        Set(value As Boolean)

            If value = IsConnected Then
                Return
            End If

            If value Then
                Try
                    connWait = True
                    portNum = Right(comPort, Len(comPort) - 3)
                    objSerial = New ASCOM.Utilities.Serial
                    objSerial.Port = CInt(portNum)
                    objSerial.Speed = SerialSpeed.ps57600
                    ' Default of 5s is too high.  THis will have to be managed, however, for synced commands that take longer (:hP most notably)
                    objSerial.ReceiveTimeout = 2
                    objSerial.Connected = True
                    Thread.Sleep(2000)      ' Disgusting hack to work around arduino resetting when connected.
                    ' I don't know of any way to poll and see if the reset has completed
                    CommandBlind(":I")      ' OAT's command for entering PC Control mode
                    If SiderealTime - PolarisRAJNow < 0 Then
                        CommandString(":SH" + utilities.HoursToHM(24 + (SiderealTime - PolarisRAJNow)), False)
                    Else
                        CommandString(":SH" + utilities.HoursToHM(SiderealTime - PolarisRAJNow), False)
                    End If
                    Dim sign As String = "+"
                    If SiteLatitude < 0 Then
                        sign = "-"
                    End If
                    CommandString(":SY" + sign + utilities.DegreesToDMS(90, "*", ":", String.Empty) + "." + utilities.HoursToHMS(SiderealTime, ":", ":"), False)
                    TL.LogMessage("Connected Set", "Connecting to port " + comPort)
                    connWait = False
                    connectedState = True
                Catch ex As Exception
                    Throw New ASCOM.DriverException(ex.Message)
                    TL.LogMessage("Connected Set", "Error Connecting to port " + comPort + " - " + ex.Message)
                End Try

            Else

                Try
                    CommandBlind(":Qq")     ' OAT's command for exiting PC Control mode
                    Thread.Sleep(1000)
                    objSerial.Connected = False
                    connectedState = False
                    TL.LogMessage("Connected Set", "Disconnecting from port " + comPort)

                Catch ex As Exception
                    TL.LogMessage("Connected Set", "Error Disconnecting from port " + comPort + " - " + ex.Message)
                End Try

            End If
        End Set
    End Property

    Public ReadOnly Property Description As String Implements ITelescopeV3.Description
        Get
            ' this pattern seems to be needed to allow a public property to return a private field
            Dim d As String = driverDescription
            TL.LogMessage("Description Get", d)
            Return d
        End Get
    End Property

    Public ReadOnly Property DriverInfo As String Implements ITelescopeV3.DriverInfo
        Get
            '            Dim m_version As Version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version
            '           Dim s_driverInfo As String = "OpenAstroTracker ASCOM driver version: " + m_version.Major.ToString() + "." + m_version.Minor.ToString() + "." + m_version.Build.ToString()
            Dim s_driverInfo As String = "OpenAstroTracker ASCOM driver version: " + Version
            TL.LogMessage("DriverInfo Get", s_driverInfo)
            Return s_driverInfo
        End Get
    End Property

    Public ReadOnly Property DriverVersion() As String Implements ITelescopeV3.DriverVersion
        Get
            TL.LogMessage("DriverVersion Get", Version)
            Return Version

            ' TL.LogMessage("DriverVersion Get", Reflection.Assembly.GetExecutingAssembly.GetName.Version.ToString(2))
            ' Return Reflection.Assembly.GetExecutingAssembly.GetName.Version.ToString(2)
        End Get
    End Property

    Public ReadOnly Property InterfaceVersion() As Short Implements ITelescopeV3.InterfaceVersion
        Get
            TL.LogMessage("InterfaceVersion Get", "3")
            Return 3
        End Get
    End Property

    Public ReadOnly Property Name As String Implements ITelescopeV3.Name
        Get
            Dim s_name As String = "OAT ASCOM"
            TL.LogMessage("Name Get", s_name)
            Return s_name
        End Get
    End Property

    Public Sub Dispose() Implements ITelescopeV3.Dispose
        ' Clean up the tracelogger and util objects
        TL.Enabled = False
        TL.Dispose()
        TL = Nothing
        utilities.Dispose()
        utilities = Nothing
        astroUtilities.Dispose()
        astroUtilities = Nothing
    End Sub

#End Region

#Region "ITelescope Implementation"
    Public Sub AbortSlew() Implements ITelescopeV3.AbortSlew
        If Not AtPark Then
            CommandString(":Q")
            TL.LogMessage("AbortSlew", ":Q# Sent")
        Else
            Throw New ASCOM.ParkedException("AbortSlew")
        End If

    End Sub

    Public ReadOnly Property AlignmentMode() As AlignmentModes Implements ITelescopeV3.AlignmentMode
        Get
            TL.LogMessage("AlignmentMode Get", "1")
            Return 1        ' 1 is "Polar (equatorial) mount other than German equatorial." from AlignmentModes Enumeration
        End Get
    End Property

    Public ReadOnly Property Altitude() As Double Implements ITelescopeV3.Altitude
        Get
            TL.LogMessage("Altitude", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("Altitude", False)
        End Get
    End Property

    Public ReadOnly Property ApertureArea() As Double Implements ITelescopeV3.ApertureArea
        Get
            TL.LogMessage("ApertureArea Get", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("ApertureArea", False)
        End Get
    End Property

    Public ReadOnly Property ApertureDiameter() As Double Implements ITelescopeV3.ApertureDiameter
        Get
            TL.LogMessage("ApertureDiameter Get", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("ApertureDiameter", False)
        End Get
    End Property

    Public ReadOnly Property AtHome() As Boolean Implements ITelescopeV3.AtHome
        Get
            'This property must be False if the telescope does not support homing.
            'TODO : We'll try to implement homing later.
            TL.LogMessage("AtHome", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property AtPark() As Boolean Implements ITelescopeV3.AtPark
        Get
            TL.LogMessage("AtPark", "Get - " & isParked.ToString())
            Return isParked     ' Custom boolean we added to track parked state
        End Get
    End Property

    Public Function AxisRates(Axis As TelescopeAxes) As IAxisRates Implements ITelescopeV3.AxisRates
        TL.LogMessage("AxisRates", "Get - " & Axis.ToString())
        Return New AxisRates(Axis)
    End Function

    Public ReadOnly Property Azimuth() As Double Implements ITelescopeV3.Azimuth
        Get
            TL.LogMessage("Azimuth Get", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("Azimuth", False)
        End Get
    End Property

    Public ReadOnly Property CanFindHome() As Boolean Implements ITelescopeV3.CanFindHome
        Get
            If Not IsConnected Then
                Throw New ASCOM.NotConnectedException("CanFindHome")
            End If
            TL.LogMessage("CanFindHome", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public Function CanMoveAxis(Axis As TelescopeAxes) As Boolean Implements ITelescopeV3.CanMoveAxis
        TL.LogMessage("CanMoveAxis", "Get - " & Axis.ToString())
        Select Case Axis
            Case TelescopeAxes.axisPrimary
                Return False
            Case TelescopeAxes.axisSecondary
                Return False
            Case TelescopeAxes.axisTertiary
                Return False
            Case Else
                Throw New InvalidValueException("CanMoveAxis", Axis.ToString(), "0 to 2")
        End Select
    End Function

    Public ReadOnly Property CanPark() As Boolean Implements ITelescopeV3.CanPark
        Get
            TL.LogMessage("CanPark", "Get - " & True.ToString())
            Return True
        End Get
    End Property

    Public ReadOnly Property CanPulseGuide() As Boolean Implements ITelescopeV3.CanPulseGuide
        Get
            TL.LogMessage("CanPulseGuide", "Get - " & True.ToString())
            Return True
        End Get
    End Property

    Public ReadOnly Property CanSetDeclinationRate() As Boolean Implements ITelescopeV3.CanSetDeclinationRate
        Get
            TL.LogMessage("CanSetDeclinationRate", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property CanSetGuideRates() As Boolean Implements ITelescopeV3.CanSetGuideRates
        Get
            TL.LogMessage("CanSetGuideRates", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property CanSetPark() As Boolean Implements ITelescopeV3.CanSetPark
        'ToDo  We should allow this
        Get
            TL.LogMessage("CanSetPark", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property CanSetPierSide() As Boolean Implements ITelescopeV3.CanSetPierSide
        Get
            TL.LogMessage("CanSetPierSide", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property CanSetRightAscensionRate() As Boolean Implements ITelescopeV3.CanSetRightAscensionRate
        Get
            TL.LogMessage("CanSetRightAscensionRate", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property CanSetTracking() As Boolean Implements ITelescopeV3.CanSetTracking
        Get
            TL.LogMessage("CanSetTracking", "Get - " & True.ToString())
            Return True
        End Get
    End Property

    Public ReadOnly Property CanSlew() As Boolean Implements ITelescopeV3.CanSlew
        Get
            TL.LogMessage("CanSlew", "Get - " & True.ToString())
            Return True
        End Get
    End Property

    Public ReadOnly Property CanSlewAltAz() As Boolean Implements ITelescopeV3.CanSlewAltAz
        ' TODO - AltAz slewing
        Get
            TL.LogMessage("CanSlewAltAz", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property CanSlewAltAzAsync() As Boolean Implements ITelescopeV3.CanSlewAltAzAsync
        Get
            TL.LogMessage("CanSlewAltAzAsync", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property CanSlewAsync() As Boolean Implements ITelescopeV3.CanSlewAsync
        Get
            TL.LogMessage("CanSlewAsync", "Get - " & True.ToString())
            Return True
        End Get
    End Property

    Public ReadOnly Property CanSync() As Boolean Implements ITelescopeV3.CanSync
        Get
            TL.LogMessage("CanSync", "Get - " & True.ToString())
            Return True
        End Get
    End Property

    Public ReadOnly Property CanSyncAltAz() As Boolean Implements ITelescopeV3.CanSyncAltAz
        Get
            TL.LogMessage("CanSyncAltAz", "Get - " & False.ToString())
            Return False
        End Get
    End Property

    Public ReadOnly Property CanUnpark() As Boolean Implements ITelescopeV3.CanUnpark
        Get
            TL.LogMessage("CanUnpark", "Get - " & True.ToString())
            Return True
        End Get
    End Property

    Public ReadOnly Property Declination() As Double Implements ITelescopeV3.Declination
        Get
            Dim declination__1 As Double = 0.0
            Dim scopeDec As String = CommandString(":GD")
            TL.LogMessage("Declination", "Get - " & scopeDec)
            declination__1 = utilities.DMSToDegrees(scopeDec)
            Return declination__1
        End Get
    End Property

    Public Property DeclinationRate() As Double Implements ITelescopeV3.DeclinationRate
        Get
            Dim declination As Double = 0.0
            TL.LogMessage("DeclinationRate", "Get - " & declination.ToString())
            Return declination
        End Get
        Set(value As Double)
            TL.LogMessage("DeclinationRate Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("DeclinationRate", True)
        End Set
    End Property

    Public Function DestinationSideOfPier(RightAscension As Double, Declination As Double) As PierSide Implements ITelescopeV3.DestinationSideOfPier
        TL.LogMessage("DestinationSideOfPier Get", "Not implemented")
        Throw New ASCOM.MethodNotImplementedException("DestinationSideOfPier")
    End Function

    Public Property DoesRefraction() As Boolean Implements ITelescopeV3.DoesRefraction
        Get
            TL.LogMessage("DoesRefraction Get", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("DoesRefraction", False)
        End Get
        Set(value As Boolean)
            TL.LogMessage("DoesRefraction Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("DoesRefraction", True)
        End Set
    End Property

    Public ReadOnly Property EquatorialSystem() As EquatorialCoordinateType Implements ITelescopeV3.EquatorialSystem
        'TODO : Determine if we're using JNow or J2000, or can use both.  Work on this.
        Get
            Dim equatorialSystem__1 As EquatorialCoordinateType = EquatorialCoordinateType.equTopocentric
            TL.LogMessage("DeclinationRate", "Get - " & equatorialSystem__1.ToString())
            Return equatorialSystem__1
        End Get
    End Property

    Public Sub FindHome() Implements ITelescopeV3.FindHome
        TL.LogMessage("FindHome", "Not implemented")
        Throw New ASCOM.MethodNotImplementedException("FindHome")
    End Sub

    Public ReadOnly Property FocalLength() As Double Implements ITelescopeV3.FocalLength
        Get
            TL.LogMessage("FocalLength Get", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("FocalLength", False)
        End Get
    End Property

    Public Property GuideRateDeclination() As Double Implements ITelescopeV3.GuideRateDeclination
        Get
            TL.LogMessage("GuideRateDeclination Get", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("GuideRateDeclination", False)
        End Get
        Set(value As Double)
            TL.LogMessage("GuideRateDeclination Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("GuideRateDeclination", True)
        End Set
    End Property

    Public Property GuideRateRightAscension() As Double Implements ITelescopeV3.GuideRateRightAscension
        Get
            TL.LogMessage("GuideRateRightAscension Get", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("GuideRateRightAscension", False)
        End Get
        Set(value As Double)
            TL.LogMessage("GuideRateRightAscension Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("GuideRateRightAscension", True)
        End Set
    End Property

    Public ReadOnly Property IsPulseGuiding() As Boolean Implements ITelescopeV3.IsPulseGuiding
        Get
            Dim retVal As Boolean = Convert.ToBoolean(CInt(CommandString(":GIG")))
            TL.LogMessage("isPulseGuiding Get", retVal.ToString)
            Return retVal
        End Get
    End Property

    Public Sub MoveAxis(Axis As TelescopeAxes, Rate As Double) Implements ITelescopeV3.MoveAxis
        'TODO This is coming
        TL.LogMessage("MoveAxis", "Not implemented")
        Throw New ASCOM.MethodNotImplementedException("MoveAxis")
    End Sub

    Public Sub Park() Implements ITelescopeV3.Park
        If Not AtPark Then
            '    TL.LogMessage("Park", "Err : Mount already parked")
            '    Throw New ASCOM.ParkedException("Already Parked")
            'Else
            CommandString(":hP", False)
            PollUntilZero(":GIS")
            isParked = True
            TL.LogMessage("Park", "Parked mount")
        End If

    End Sub

    Public Sub PulseGuide(Direction As GuideDirections, Duration As Integer) Implements ITelescopeV3.PulseGuide
        If Not AtPark Then
            TL.LogMessage("PulseGuide", Direction.ToString + " " + Duration.ToString)
            Dim dir As String, length As String, pgCmd As String = ":MG"
            dir = Direction.ToString.Substring(5, 1)
            length = Right("0000" + Duration.ToString, 4)
            pgCmd = pgCmd + dir + length
            CommandBlind(pgCmd, False)
        Else
            TL.LogMessage("PulseGuide", "Parked")
            Throw New ASCOM.ParkedException("PulseGuide")
        End If
    End Sub

    Public ReadOnly Property RightAscension() As Double Implements ITelescopeV3.RightAscension
        Get

            Dim rightAscension__1 As Double = 0.0
            rightAscension__1 = utilities.HMSToHours(CommandString(":GR"))
            TL.LogMessage("RightAscension", rightAscension__1.ToString)
            Return rightAscension__1

        End Get
    End Property

    Public Property RightAscensionRate() As Double Implements ITelescopeV3.RightAscensionRate
        Get
            Dim rightAscensionRate__1 As Double = 0.0
            TL.LogMessage("RightAscensionRate", "Get - " & rightAscensionRate__1.ToString())
            Return rightAscensionRate__1
        End Get
        Set(value As Double)
            TL.LogMessage("RightAscensionRate Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("RightAscensionRate", True)
        End Set
    End Property

    Public Sub SetPark() Implements ITelescopeV3.SetPark
        TL.LogMessage("SetPark", "Not implemented")
        Throw New ASCOM.MethodNotImplementedException("SetPark")
    End Sub

    Public Property SideOfPier() As PierSide Implements ITelescopeV3.SideOfPier
        Get
            Dim retVal As PierSide
            If SiderealTime < 12 Then
                If RightAscension >= SiderealTime And RightAscension <= SiderealTime + 12 Then
                    retVal = PierSide.pierWest
                Else
                    retVal = PierSide.pierEast
                End If
            Else
                If RightAscension <= SiderealTime And RightAscension >= SiderealTime - 12 Then
                    retVal = PierSide.pierEast
                Else
                    retVal = PierSide.pierWest
                End If
            End If
            TL.LogMessage("SideOfPier Get", retVal.ToString)
            Return retVal
        End Get
        Set(value As PierSide)
            TL.LogMessage("SideOfPier Set", "Not Implemented")
            Throw New ASCOM.PropertyNotImplementedException("SideOfPier", True)
        End Set
    End Property

    Public ReadOnly Property SiderealTime() As Double Implements ITelescopeV3.SiderealTime

        Get
            ' now using novas 3.1
            Dim lst As Double = 0.0
            Using novas As New ASCOM.Astrometry.NOVAS.NOVAS31
                Dim jd As Double = utilities.DateUTCToJulian(DateTime.UtcNow)
                novas.SiderealTime(jd, 0, novas.DeltaT(jd),
                                   Astrometry.GstType.GreenwichMeanSiderealTime,
                                   Astrometry.Method.EquinoxBased,
                                   Astrometry.Accuracy.Reduced,
                                   lst)
            End Using

            ' Allow for the longitude
            lst += SiteLongitude / 360.0 * 24.0

            ' Reduce to the range 0 to 24 hours
            lst = astroUtilities.ConditionRA(lst)

            TL.LogMessage("SiderealTime", "Get - " & lst.ToString())
            Return lst
        End Get
    End Property

    Public Property SiteElevation() As Double Implements ITelescopeV3.SiteElevation
        Get
            TL.LogMessage("SiteElevation Get", elevation.ToString)
            Return elevation
        End Get
        Set(value As Double)
            If value >= -300 And value <= 10000 Then
                TL.LogMessage("SiteElevation Set", value.ToString)
                elevation = value
            Else
                Throw New ASCOM.InvalidValueException("SiteElevation")
            End If
        End Set
    End Property

    Public Property SiteLatitude() As Double Implements ITelescopeV3.SiteLatitude
        Get
            TL.LogMessage("SiteLatitude Get", latitude.ToString)
            Return latitude
        End Get
        Set(value As Double)
            TL.LogMessage("SiteLatitude Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("SiteLatitude", True)
        End Set
    End Property

    Public Property SiteLongitude() As Double Implements ITelescopeV3.SiteLongitude
        Get
            TL.LogMessage("SiteLongitude Get", longitude.ToString)
            Return longitude
        End Get
        Set(value As Double)
            TL.LogMessage("SiteLongitude Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("SiteLongitude", True)
        End Set
    End Property

    Public Property SlewSettleTime() As Short Implements ITelescopeV3.SlewSettleTime
        Get
            TL.LogMessage("SlewSettleTime Get", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("SlewSettleTime", False)
        End Get
        Set(value As Short)
            TL.LogMessage("SlewSettleTime Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("SlewSettleTime", True)
        End Set
    End Property

    Public Sub SlewToAltAz(Azimuth As Double, Altitude As Double) Implements ITelescopeV3.SlewToAltAz
        TL.LogMessage("SlewToAltAz", "Not implemented")
        Throw New ASCOM.MethodNotImplementedException("SlewToAltAz")
    End Sub

    Public Sub SlewToAltAzAsync(Azimuth As Double, Altitude As Double) Implements ITelescopeV3.SlewToAltAzAsync
        TL.LogMessage("SlewToAltAzAsync", "Not implemented")
        Throw New ASCOM.MethodNotImplementedException("SlewToAltAzAsync")
    End Sub

    Public Sub SlewToCoordinates(RightAscension As Double, Declination As Double) Implements ITelescopeV3.SlewToCoordinates
        ' Synchronous slew to given coordinates.  Uses PollUntilZero to wait for slew to finish
        If RightAscension <= 24 And RightAscension >= 0 And Declination >= -90 And Declination <= 90 Then

            If Not AtPark Then
                TL.LogMessage("SlewToCoordinates", "RA " + RightAscension.ToString + ", Dec " + Declination.ToString)
                Dim strRAcmd = ":Sr" + utilities.HoursToHMS(RightAscension, ":", ":")
                Dim strDeccmd = utilities.DegreesToDMS(Declination, "*", ":", "")
                If Declination >= 0 Then
                    strDeccmd = "+" + strDeccmd
                End If
                strDeccmd = ":Sd" + strDeccmd
                TL.LogMessage("SlewToCoordinatesRACmd", strRAcmd)
                TL.LogMessage("SlewToCoordinatesDecCmd", strDeccmd)
                If CommandString(strRAcmd) = "1" Then
                    If CommandString(strDeccmd) = "1" Then
                        CommandString(":MS")
                        PollUntilZero(":GIS")
                    End If

                End If
            Else
                TL.LogMessage("SlewToCoordinates", "Parked")
                Throw New ASCOM.ParkedException("SlewToCoordinates")
            End If
        Else
            TL.LogMessage("SlewToCoordinates", "Invalid coordinates RA: " + RightAscension.ToString + ", Dec: " + Declination.ToString)
            Throw New ASCOM.InvalidValueException("SlewToCoordinates")
        End If
    End Sub

    Public Sub SlewToCoordinatesAsync(RightAscension As Double, Declination As Double) Implements ITelescopeV3.SlewToCoordinatesAsync
        ' ASynchronous slew to coordinates.  Returns immediately after receiving response from :MS that command was accepted
        If RightAscension <= 24 And RightAscension >= 0 And Declination >= -90 And Declination <= 90 Then

            If Not AtPark Then
                TL.LogMessage("SlewToCoordinatesAsync", "RA " + RightAscension.ToString + ", Dec " + Declination.ToString)
                Dim strRAcmd = ":Sr" + utilities.HoursToHMS(RightAscension, ":", ":")
                Dim strDeccmd = utilities.DegreesToDMS(Declination, "*", ":", "")
                If Declination >= 0 Then
                    strDeccmd = "+" + strDeccmd
                End If
                strDeccmd = ":Sd" + strDeccmd
                TL.LogMessage("SlewToCoordinatesAsyncRACmd", strRAcmd)
                TL.LogMessage("SlewToCoordinatesAsyncDecCmd", strDeccmd)
                If CommandString(strRAcmd) = "1" Then
                    If CommandString(strDeccmd) = "1" Then
                        CommandString(":MS")
                    End If

                End If
            Else
                TL.LogMessage("SlewToCoordinatesAsync", "Parked")
                Throw New ASCOM.ParkedException("SlewToCoordinatesAsync")
            End If
        Else
            TL.LogMessage("SlewToCoordinatesAsync", "Invalid coordinates RA: " + RightAscension.ToString + ", Dec: " + Declination.ToString)
            Throw New ASCOM.InvalidValueException("SlewToCoordinatesAsync")
        End If

    End Sub

    Public Sub SlewToTarget() Implements ITelescopeV3.SlewToTarget
        TL.LogMessage("SlewToTarget", TargetRightAscension.ToString + ", " + TargetDeclination.ToString)
        SlewToCoordinates(TargetRightAscension, TargetDeclination)
    End Sub

    Public Sub SlewToTargetAsync() Implements ITelescopeV3.SlewToTargetAsync
        TL.LogMessage("SlewToTargetAsync", TargetRightAscension.ToString + ", " + TargetDeclination.ToString)
        SlewToCoordinatesAsync(TargetRightAscension, TargetDeclination)
    End Sub

    Public ReadOnly Property Slewing() As Boolean Implements ITelescopeV3.Slewing
        Get
            Dim retVal As Boolean = Convert.ToBoolean(CInt(CommandString(":GIS")))
            TL.LogMessage("Slewing Get", retVal.ToString)
            Return retVal
        End Get
    End Property

    Public Sub SyncToAltAz(Azimuth As Double, Altitude As Double) Implements ITelescopeV3.SyncToAltAz
        TL.LogMessage("SyncToAltAz", "Not implemented")
        Throw New ASCOM.MethodNotImplementedException("SyncToAltAz")
    End Sub

    Public Sub SyncToCoordinates(RightAscension As Double, Declination As Double) Implements ITelescopeV3.SyncToCoordinates
        If Not AtPark Then
            If RightAscension <= 24 And RightAscension >= 0 And Declination >= -90 And Declination <= 90 Then
                Dim sign As String = String.Empty
                If Declination >= 0 Then
                    sign = "+"
                End If
                Dim success As String = CommandString(":SY" + sign + utilities.DegreesToDMS(Declination, "*", ":", String.Empty) + "." + utilities.HoursToHMS(RightAscension, ":", ":"), False)
                If success = "1" Then
                    TL.LogMessage("SyncToCoordinates", "Synced to " + utilities.DegreesToDMS(Declination) + ", " + utilities.HoursToHMS(RightAscension))
                Else
                    TL.LogMessage("SyncToCoordinates", "Err - Failed to sync to " + utilities.DegreesToDMS(Declination) + ", " + utilities.HoursToHMS(RightAscension))
                    Throw New ASCOM.DriverException("SyncToCoordinates")
                End If
            Else
                TL.LogMessage("SyncToCoordinates", "Err - Invalid coordinates RA: " + RightAscension.ToString + ", Dec: " + Declination.ToString)
                Throw New ASCOM.InvalidValueException("SyncToCoordinates")
            End If
        Else
            TL.LogMessage("SyncToCoordinates", "Err - Parked")
            Throw New ASCOM.ParkedException("SyncToCoordinates")
        End If

    End Sub

    Public Sub SyncToTarget() Implements ITelescopeV3.SyncToTarget
        If Not AtPark Then
            If targetDecSet Then

                If targetRASet Then

                    Dim sign As String = String.Empty
                    If TargetDeclination >= 0 Then
                        sign = "+"
                    End If
                    Dim success As String = CommandString(":SY" + sign + utilities.DegreesToDMS(TargetDeclination, "*", ":", String.Empty) + "." + utilities.HoursToHMS(TargetRightAscension, ":", ":"), False)
                    If success = "1" Then
                        TL.LogMessage("SyncToTarget", "Synced to " + utilities.DegreesToDMS(TargetDeclination) + ", " + utilities.HoursToHMS(TargetRightAscension))
                    Else
                        TL.LogMessage("SyncToTarget", "Failed to sync to " + utilities.DegreesToDMS(TargetDeclination) + ", " + utilities.HoursToHMS(TargetRightAscension))
                        Throw New ASCOM.DriverException("SyncToTarget")
                    End If

                Else

                    Throw New ASCOM.ValueNotSetException("TargetRightAscension")

                End If

            Else

                Throw New ASCOM.ValueNotSetException("TargetDeclination")

            End If
        Else
            TL.LogMessage("SyncToTarget", "Err - Parked")
            Throw New ASCOM.ParkedException("SyncToTarget")
        End If

    End Sub

    Public Property TargetDeclination() As Double Implements ITelescopeV3.TargetDeclination
        Get
            If targetDecSet Then
                TL.LogMessage("TargetDeclination Get", targetDec.ToString)
                Return targetDec
            Else
                TL.LogMessage("TargetDeclination Get", "Value not set")
                Throw New ASCOM.ValueNotSetException("TargetDeclination")
            End If

        End Get
        Set(value As Double)
            If value >= -90 And value <= 90 Then
                TL.LogMessage("TargetDeclination Set", value.ToString)
                targetDec = value
                targetDecSet = True
            Else
                TL.LogMessage("TargetDeclination Set", "Invalid Value " + value.ToString)
                Throw New ASCOM.InvalidValueException("TargetDeclination")
            End If

        End Set
    End Property

    Public Property TargetRightAscension() As Double Implements ITelescopeV3.TargetRightAscension
        Get
            If targetRASet Then
                TL.LogMessage("TargetRightAscension Get", targetRA.ToString)
                Return targetRA
            Else
                TL.LogMessage("TargetRightAscension Get", "Value not set")
                Throw New ASCOM.ValueNotSetException("TargetRightAscension")
            End If

        End Get
        Set(value As Double)
            If value >= 0 And value <= 24 Then
                TL.LogMessage("TargetRightAscension Set", value.ToString)
                targetRA = value
                targetRASet = True
            Else
                TL.LogMessage("TargetRightAscension Set", "Invalid Value " + value.ToString)
                Throw New ASCOM.InvalidValueException("TargetRightAscension")
            End If

        End Set
    End Property

    Public Property Tracking() As Boolean Implements ITelescopeV3.Tracking

        Get
            If CommandString(":GIT", False) = "0" Then
                isTracking = False
                TL.LogMessage("Tracking", "Get - " & False.ToString())
            Else
                isTracking = True
                TL.LogMessage("Tracking", "Get - " & True.ToString())
            End If
            Return isTracking
        End Get
        Set(value As Boolean)
            If CommandString(":MT" + Convert.ToInt32(value).ToString, False) = "1" Then
                isTracking = value
                TL.LogMessage("Tracking Set", value.ToString)
            Else
                TL.LogMessage("Tracking Set", "Error")
                Throw New ASCOM.DriverException("Error setting tracking state")
            End If

        End Set

    End Property

    Public Property TrackingRate() As DriveRates Implements ITelescopeV3.TrackingRate
        Get
            TL.LogMessage("TrackingRate Get", DriveRates.driveSidereal.ToString)
            Return DriveRates.driveSidereal
            ' Throw New ASCOM.PropertyNotImplementedException("TrackingRate", False)
        End Get
        Set(value As DriveRates)
            TL.LogMessage("TrackingRate Set", "Not implemented")
            Throw New ASCOM.PropertyNotImplementedException("TrackingRate", True)
        End Set
    End Property

    Public ReadOnly Property TrackingRates() As ITrackingRates Implements ITelescopeV3.TrackingRates
        Get
            Dim trackingRates__1 As ITrackingRates = New TrackingRates()
            TL.LogMessage("TrackingRates", "Get - ")
            For Each driveRate As DriveRates In trackingRates__1
                TL.LogMessage("TrackingRates", "Get - " & driveRate.ToString())
            Next
            Return trackingRates__1
        End Get
    End Property

    Public Property UTCDate() As DateTime Implements ITelescopeV3.UTCDate
        ' ToDo - Can we handle this without bothering the mount?
        Get
            Dim utcDate__1 As DateTime = DateTime.UtcNow
            TL.LogMessage("UTCDate", String.Format("Get - {0}", utcDate__1))
            Return utcDate__1
        End Get
        Set(value As DateTime)
            Throw New ASCOM.PropertyNotImplementedException("UTCDate", True)
        End Set
    End Property

    Public Sub Unpark() Implements ITelescopeV3.Unpark
        If AtPark Then
            '    TL.LogMessage("Unpark", "Err : Mount not parked")
            '    Throw New ASCOM.DriverException("Mount not parked")
            'Else
            Dim unprkRet As String = CommandString(":hU", False)
            If unprkRet = "1" Then
                TL.LogMessage("Unpark", "Unparked mount")
            End If
            isParked = False
        End If
    End Sub

#End Region

#Region "Private properties and methods"
    ' here are some useful properties and methods that can be used as required
    ' to help with

#Region "ASCOM Registration"

    Private Shared Sub RegUnregASCOM(ByVal bRegister As Boolean)

        Using P As New Profile() With {.DeviceType = "Telescope"}
            If bRegister Then
                P.Register(driverID, driverDescription)
            Else
                P.Unregister(driverID)
            End If
        End Using

    End Sub

    <ComRegisterFunction()> _
    Public Shared Sub RegisterASCOM(ByVal T As Type)

        RegUnregASCOM(True)

    End Sub

    <ComUnregisterFunction()> _
    Public Shared Sub UnregisterASCOM(ByVal T As Type)

        RegUnregASCOM(False)

    End Sub

#End Region

    ''' <summary>
    ''' Returns true if there is a valid connection to the driver hardware
    ''' </summary>
    Private ReadOnly Property IsConnected As Boolean
        Get
            ' TODO check that the driver hardware connection exists and is connected to the hardware
            Return connectedState
        End Get
    End Property

    ''' <summary>
    ''' Use this function to throw an exception if we aren't connected to the hardware
    ''' </summary>
    ''' <param name="message"></param>
    Private Sub CheckConnected(ByVal message As String)
        If Not IsConnected Then
            Throw New NotConnectedException(message)
        End If
    End Sub

    ''' <summary>
    ''' Read the device configuration from the ASCOM Profile store
    ''' </summary>
    Friend Sub ReadProfile()
        Using driverProfile As New Profile()
            driverProfile.DeviceType = "Telescope"
            traceState = Convert.ToBoolean(driverProfile.GetValue(driverID, traceStateProfileName, String.Empty, traceStateDefault))
            comPort = driverProfile.GetValue(driverID, comPortProfileName, String.Empty, comPortDefault)
            latitude = driverProfile.GetValue(driverID, latitudeProfileName, String.Empty, latitudeDefault)
            longitude = driverProfile.GetValue(driverID, longitudeProfileName, String.Empty, longitudeDefault)
            elevation = driverProfile.GetValue(driverID, elevationProfileName, String.Empty, elevationDefault)
        End Using
    End Sub

    ''' <summary>
    ''' Write the device configuration to the  ASCOM  Profile store
    ''' </summary>
    Friend Sub WriteProfile()
        Using driverProfile As New Profile()
            driverProfile.DeviceType = "Telescope"
            driverProfile.WriteValue(driverID, traceStateProfileName, traceState.ToString())
            driverProfile.WriteValue(driverID, comPortProfileName, comPort.ToString())
            driverProfile.WriteValue(driverID, latitudeProfileName, latitude.ToString())
            driverProfile.WriteValue(driverID, longitudeProfileName, longitude.ToString())
            driverProfile.WriteValue(driverID, elevationProfileName, elevation.ToString)
        End Using

    End Sub

#End Region

#Region "Helper Functions"

    Private Function PollUntilZero(ByVal command As String) As Integer
        ' Takes a command to be sent via CommandString, and resends every 1000ms until a 0 is returned.  Returns 0 only when complete.
        Dim retVal As String = ""
        Do Until retVal = "0"
            retVal = CommandString(command, False)
            Thread.Sleep(1000)
        Loop
        Return CInt(retVal)
    End Function

#End Region

End Class
