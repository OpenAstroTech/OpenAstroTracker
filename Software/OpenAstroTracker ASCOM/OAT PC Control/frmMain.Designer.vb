<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.labelDriverId = New System.Windows.Forms.Label()
        Me.buttonConnect = New System.Windows.Forms.Button()
        Me.buttonChoose = New System.Windows.Forms.Button()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.btnPark = New System.Windows.Forms.Button()
        Me.btnHalt = New System.Windows.Forms.Button()
        Me.btnSlewAsync = New System.Windows.Forms.Button()
        Me.btnSlewSync = New System.Windows.Forms.Button()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.nud_Decm = New System.Windows.Forms.NumericUpDown()
        Me.nud_Decs = New System.Windows.Forms.NumericUpDown()
        Me.nud_Decd = New System.Windows.Forms.NumericUpDown()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.nud_RAm = New System.Windows.Forms.NumericUpDown()
        Me.nud_RAs = New System.Windows.Forms.NumericUpDown()
        Me.nud_RAh = New System.Windows.Forms.NumericUpDown()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.btnEast = New System.Windows.Forms.Button()
        Me.btnWest = New System.Windows.Forms.Button()
        Me.btnSouth = New System.Windows.Forms.Button()
        Me.btnNorth = New System.Windows.Forms.Button()
        Me.groupBoxInfo = New System.Windows.Forms.GroupBox()
        Me.Label18 = New System.Windows.Forms.Label()
        Me.Label16 = New System.Windows.Forms.Label()
        Me.txtTargetDec2000 = New System.Windows.Forms.TextBox()
        Me.txtTargetRA2000 = New System.Windows.Forms.TextBox()
        Me.txtMountDec2000 = New System.Windows.Forms.TextBox()
        Me.txtMountRA2000 = New System.Windows.Forms.TextBox()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.lblDriverVersion = New System.Windows.Forms.Label()
        Me.Label17 = New System.Windows.Forms.Label()
        Me.lblFirmwareVersion = New System.Windows.Forms.Label()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.txtTargetDec = New System.Windows.Forms.TextBox()
        Me.txtTargetRA = New System.Windows.Forms.TextBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.txtMountDec = New System.Windows.Forms.TextBox()
        Me.txtMountRA = New System.Windows.Forms.TextBox()
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.ToolTipPark = New System.Windows.Forms.ToolTip(Me.components)
        Me.ToolTipSlew = New System.Windows.Forms.ToolTip(Me.components)
        Me.ToolTipSlewAsync = New System.Windows.Forms.ToolTip(Me.components)
        Me.ToolTipHalt = New System.Windows.Forms.ToolTip(Me.components)
        Me.ToolTipSlewAxis = New System.Windows.Forms.ToolTip(Me.components)
        Me.GroupBox5 = New System.Windows.Forms.GroupBox()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.txtLong = New System.Windows.Forms.TextBox()
        Me.txtLat = New System.Windows.Forms.TextBox()
        Me.GroupBox6 = New System.Windows.Forms.GroupBox()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        CType(Me.nud_Decm, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_Decs, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_Decd, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_RAm, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_RAs, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_RAh, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox3.SuspendLayout()
        Me.groupBoxInfo.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        Me.SuspendLayout()
        '
        'labelDriverId
        '
        Me.labelDriverId.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.labelDriverId.DataBindings.Add(New System.Windows.Forms.Binding("Text", Global.ASCOM.OpenAstroTracker.My.MySettings.Default, "DriverId", True, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged))
        Me.labelDriverId.Location = New System.Drawing.Point(27, 16)
        Me.labelDriverId.Name = "labelDriverId"
        Me.labelDriverId.Size = New System.Drawing.Size(243, 21)
        Me.labelDriverId.TabIndex = 5
        Me.labelDriverId.Text = Global.ASCOM.OpenAstroTracker.My.MySettings.Default.DriverId
        Me.labelDriverId.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'buttonConnect
        '
        Me.buttonConnect.Location = New System.Drawing.Point(276, 43)
        Me.buttonConnect.Name = "buttonConnect"
        Me.buttonConnect.Size = New System.Drawing.Size(72, 23)
        Me.buttonConnect.TabIndex = 4
        Me.buttonConnect.Text = "Connect"
        Me.buttonConnect.UseVisualStyleBackColor = True
        '
        'buttonChoose
        '
        Me.buttonChoose.Location = New System.Drawing.Point(276, 14)
        Me.buttonChoose.Name = "buttonChoose"
        Me.buttonChoose.Size = New System.Drawing.Size(72, 23)
        Me.buttonChoose.TabIndex = 3
        Me.buttonChoose.Text = "Choose"
        Me.buttonChoose.UseVisualStyleBackColor = True
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.buttonChoose)
        Me.GroupBox1.Controls.Add(Me.labelDriverId)
        Me.GroupBox1.Controls.Add(Me.buttonConnect)
        Me.GroupBox1.Location = New System.Drawing.Point(379, 12)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(354, 83)
        Me.GroupBox1.TabIndex = 6
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Connection"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.btnPark)
        Me.GroupBox2.Controls.Add(Me.btnHalt)
        Me.GroupBox2.Controls.Add(Me.btnSlewAsync)
        Me.GroupBox2.Controls.Add(Me.btnSlewSync)
        Me.GroupBox2.Controls.Add(Me.Label8)
        Me.GroupBox2.Controls.Add(Me.Label9)
        Me.GroupBox2.Controls.Add(Me.Label10)
        Me.GroupBox2.Controls.Add(Me.nud_Decm)
        Me.GroupBox2.Controls.Add(Me.nud_Decs)
        Me.GroupBox2.Controls.Add(Me.nud_Decd)
        Me.GroupBox2.Controls.Add(Me.Label7)
        Me.GroupBox2.Controls.Add(Me.Label6)
        Me.GroupBox2.Controls.Add(Me.Label5)
        Me.GroupBox2.Controls.Add(Me.Label4)
        Me.GroupBox2.Controls.Add(Me.Label3)
        Me.GroupBox2.Controls.Add(Me.nud_RAm)
        Me.GroupBox2.Controls.Add(Me.nud_RAs)
        Me.GroupBox2.Controls.Add(Me.nud_RAh)
        Me.GroupBox2.Location = New System.Drawing.Point(12, 12)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(361, 180)
        Me.GroupBox2.TabIndex = 7
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "Slew to Target"
        '
        'btnPark
        '
        Me.btnPark.Enabled = False
        Me.btnPark.Location = New System.Drawing.Point(117, 118)
        Me.btnPark.Name = "btnPark"
        Me.btnPark.Size = New System.Drawing.Size(96, 23)
        Me.btnPark.TabIndex = 17
        Me.btnPark.Text = "Park"
        Me.ToolTipPark.SetToolTip(Me.btnPark, "Returns mount to home position, and stops motors.  (NOTE : Motors do not currentl" & _
        "y stop!)")
        Me.btnPark.UseVisualStyleBackColor = True
        '
        'btnHalt
        '
        Me.btnHalt.Enabled = False
        Me.btnHalt.Location = New System.Drawing.Point(117, 89)
        Me.btnHalt.Name = "btnHalt"
        Me.btnHalt.Size = New System.Drawing.Size(96, 23)
        Me.btnHalt.TabIndex = 16
        Me.btnHalt.Text = "Halt"
        Me.ToolTipHalt.SetToolTip(Me.btnHalt, "Stops all mount movement." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Currently unreliable, pay attention!")
        Me.btnHalt.UseVisualStyleBackColor = True
        '
        'btnSlewAsync
        '
        Me.btnSlewAsync.Location = New System.Drawing.Point(14, 116)
        Me.btnSlewAsync.Name = "btnSlewAsync"
        Me.btnSlewAsync.Size = New System.Drawing.Size(96, 23)
        Me.btnSlewAsync.TabIndex = 15
        Me.btnSlewAsync.Text = "Slew (Async)"
        Me.ToolTipSlewAsync.SetToolTip(Me.btnSlewAsync, resources.GetString("btnSlewAsync.ToolTip"))
        Me.btnSlewAsync.UseVisualStyleBackColor = True
        '
        'btnSlewSync
        '
        Me.btnSlewSync.Enabled = False
        Me.btnSlewSync.Location = New System.Drawing.Point(14, 89)
        Me.btnSlewSync.Name = "btnSlewSync"
        Me.btnSlewSync.Size = New System.Drawing.Size(96, 23)
        Me.btnSlewSync.TabIndex = 14
        Me.btnSlewSync.Text = "Slew"
        Me.ToolTipSlew.SetToolTip(Me.btnSlewSync, resources.GetString("btnSlewSync.ToolTip"))
        Me.btnSlewSync.UseVisualStyleBackColor = True
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(229, 43)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(14, 13)
        Me.Label8.TabIndex = 13
        Me.Label8.Text = "S"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Location = New System.Drawing.Point(158, 43)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(16, 13)
        Me.Label9.TabIndex = 12
        Me.Label9.Text = "M"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Location = New System.Drawing.Point(88, 43)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(15, 13)
        Me.Label10.TabIndex = 11
        Me.Label10.Text = "D"
        '
        'nud_Decm
        '
        Me.nud_Decm.Location = New System.Drawing.Point(109, 41)
        Me.nud_Decm.Maximum = New Decimal(New Integer() {59, 0, 0, 0})
        Me.nud_Decm.Name = "nud_Decm"
        Me.nud_Decm.Size = New System.Drawing.Size(43, 20)
        Me.nud_Decm.TabIndex = 10
        '
        'nud_Decs
        '
        Me.nud_Decs.Location = New System.Drawing.Point(180, 41)
        Me.nud_Decs.Maximum = New Decimal(New Integer() {59, 0, 0, 0})
        Me.nud_Decs.Name = "nud_Decs"
        Me.nud_Decs.Size = New System.Drawing.Size(43, 20)
        Me.nud_Decs.TabIndex = 9
        '
        'nud_Decd
        '
        Me.nud_Decd.Location = New System.Drawing.Point(39, 41)
        Me.nud_Decd.Maximum = New Decimal(New Integer() {90, 0, 0, 0})
        Me.nud_Decd.Minimum = New Decimal(New Integer() {90, 0, 0, -2147483648})
        Me.nud_Decd.Name = "nud_Decd"
        Me.nud_Decd.Size = New System.Drawing.Size(43, 20)
        Me.nud_Decd.TabIndex = 8
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(6, 43)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(27, 13)
        Me.Label7.TabIndex = 7
        Me.Label7.Text = "Dec"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(11, 20)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(22, 13)
        Me.Label6.TabIndex = 6
        Me.Label6.Text = "RA"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(229, 20)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(14, 13)
        Me.Label5.TabIndex = 5
        Me.Label5.Text = "S"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(158, 20)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(16, 13)
        Me.Label4.TabIndex = 4
        Me.Label4.Text = "M"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(88, 20)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(15, 13)
        Me.Label3.TabIndex = 3
        Me.Label3.Text = "H"
        '
        'nud_RAm
        '
        Me.nud_RAm.Location = New System.Drawing.Point(109, 18)
        Me.nud_RAm.Maximum = New Decimal(New Integer() {59, 0, 0, 0})
        Me.nud_RAm.Name = "nud_RAm"
        Me.nud_RAm.Size = New System.Drawing.Size(43, 20)
        Me.nud_RAm.TabIndex = 2
        '
        'nud_RAs
        '
        Me.nud_RAs.Location = New System.Drawing.Point(180, 18)
        Me.nud_RAs.Maximum = New Decimal(New Integer() {59, 0, 0, 0})
        Me.nud_RAs.Name = "nud_RAs"
        Me.nud_RAs.Size = New System.Drawing.Size(43, 20)
        Me.nud_RAs.TabIndex = 1
        '
        'nud_RAh
        '
        Me.nud_RAh.Location = New System.Drawing.Point(39, 18)
        Me.nud_RAh.Maximum = New Decimal(New Integer() {23, 0, 0, 0})
        Me.nud_RAh.Name = "nud_RAh"
        Me.nud_RAh.Size = New System.Drawing.Size(43, 20)
        Me.nud_RAh.TabIndex = 0
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.btnEast)
        Me.GroupBox3.Controls.Add(Me.btnWest)
        Me.GroupBox3.Controls.Add(Me.btnSouth)
        Me.GroupBox3.Controls.Add(Me.btnNorth)
        Me.GroupBox3.Location = New System.Drawing.Point(12, 198)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(361, 117)
        Me.GroupBox3.TabIndex = 8
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "Manual Slew"
        '
        'btnEast
        '
        Me.btnEast.Enabled = False
        Me.btnEast.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnEast.Location = New System.Drawing.Point(195, 49)
        Me.btnEast.Name = "btnEast"
        Me.btnEast.Size = New System.Drawing.Size(28, 23)
        Me.btnEast.TabIndex = 3
        Me.btnEast.Text = "E"
        Me.ToolTipSlewAxis.SetToolTip(Me.btnEast, "Slew in the direction indicated." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "The mount will move in the direction indicate" & _
        "d as long as the" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "button remains pressed.  Releasing the button will stop" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "the s" & _
        "lew." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Not yet implemented.")
        Me.btnEast.UseVisualStyleBackColor = True
        '
        'btnWest
        '
        Me.btnWest.Enabled = False
        Me.btnWest.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnWest.Location = New System.Drawing.Point(124, 49)
        Me.btnWest.Name = "btnWest"
        Me.btnWest.Size = New System.Drawing.Size(28, 23)
        Me.btnWest.TabIndex = 2
        Me.btnWest.Text = "W"
        Me.ToolTipSlewAxis.SetToolTip(Me.btnWest, "Slew in the direction indicated." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "The mount will move in the direction indicate" & _
        "d as long as the" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "button remains pressed.  Releasing the button will stop" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "the s" & _
        "lew." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Not yet implemented.")
        Me.btnWest.UseVisualStyleBackColor = True
        '
        'btnSouth
        '
        Me.btnSouth.Enabled = False
        Me.btnSouth.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnSouth.Location = New System.Drawing.Point(161, 78)
        Me.btnSouth.Name = "btnSouth"
        Me.btnSouth.Size = New System.Drawing.Size(28, 23)
        Me.btnSouth.TabIndex = 1
        Me.btnSouth.Text = "S"
        Me.ToolTipSlewAxis.SetToolTip(Me.btnSouth, "Slew in the direction indicated." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "The mount will move in the direction indicate" & _
        "d as long as the" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "button remains pressed.  Releasing the button will stop" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "the s" & _
        "lew." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Not yet implemented.")
        Me.btnSouth.UseVisualStyleBackColor = True
        '
        'btnNorth
        '
        Me.btnNorth.Enabled = False
        Me.btnNorth.Font = New System.Drawing.Font("Microsoft Sans Serif", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.btnNorth.Location = New System.Drawing.Point(161, 19)
        Me.btnNorth.Name = "btnNorth"
        Me.btnNorth.Size = New System.Drawing.Size(28, 23)
        Me.btnNorth.TabIndex = 0
        Me.btnNorth.Text = "N"
        Me.ToolTipSlewAxis.SetToolTip(Me.btnNorth, "Slew in the direction indicated." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "The mount will move in the direction indicate" & _
        "d as long as the" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "button remains pressed.  Releasing the button will stop" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "the s" & _
        "lew." & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "Not yet implemented.")
        Me.btnNorth.UseVisualStyleBackColor = True
        '
        'groupBoxInfo
        '
        Me.groupBoxInfo.Controls.Add(Me.Label18)
        Me.groupBoxInfo.Controls.Add(Me.Label16)
        Me.groupBoxInfo.Controls.Add(Me.txtTargetDec2000)
        Me.groupBoxInfo.Controls.Add(Me.txtTargetRA2000)
        Me.groupBoxInfo.Controls.Add(Me.txtMountDec2000)
        Me.groupBoxInfo.Controls.Add(Me.txtMountRA2000)
        Me.groupBoxInfo.Controls.Add(Me.Label15)
        Me.groupBoxInfo.Controls.Add(Me.lblDriverVersion)
        Me.groupBoxInfo.Controls.Add(Me.Label17)
        Me.groupBoxInfo.Controls.Add(Me.lblFirmwareVersion)
        Me.groupBoxInfo.Controls.Add(Me.Label11)
        Me.groupBoxInfo.Controls.Add(Me.Label12)
        Me.groupBoxInfo.Controls.Add(Me.txtTargetDec)
        Me.groupBoxInfo.Controls.Add(Me.txtTargetRA)
        Me.groupBoxInfo.Controls.Add(Me.Label2)
        Me.groupBoxInfo.Controls.Add(Me.Label1)
        Me.groupBoxInfo.Controls.Add(Me.txtMountDec)
        Me.groupBoxInfo.Controls.Add(Me.txtMountRA)
        Me.groupBoxInfo.Location = New System.Drawing.Point(379, 198)
        Me.groupBoxInfo.Name = "groupBoxInfo"
        Me.groupBoxInfo.Size = New System.Drawing.Size(354, 222)
        Me.groupBoxInfo.TabIndex = 9
        Me.groupBoxInfo.TabStop = False
        Me.groupBoxInfo.Text = "Current Mount Information"
        '
        'Label18
        '
        Me.Label18.AutoSize = True
        Me.Label18.Location = New System.Drawing.Point(236, 83)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(36, 13)
        Me.Label18.TabIndex = 20
        Me.Label18.Text = "J2000"
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Location = New System.Drawing.Point(137, 83)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(34, 13)
        Me.Label16.TabIndex = 19
        Me.Label16.Text = "JNow"
        '
        'txtTargetDec2000
        '
        Me.txtTargetDec2000.Location = New System.Drawing.Point(208, 183)
        Me.txtTargetDec2000.Name = "txtTargetDec2000"
        Me.txtTargetDec2000.Size = New System.Drawing.Size(95, 20)
        Me.txtTargetDec2000.TabIndex = 18
        '
        'txtTargetRA2000
        '
        Me.txtTargetRA2000.Location = New System.Drawing.Point(208, 157)
        Me.txtTargetRA2000.Name = "txtTargetRA2000"
        Me.txtTargetRA2000.Size = New System.Drawing.Size(95, 20)
        Me.txtTargetRA2000.TabIndex = 17
        '
        'txtMountDec2000
        '
        Me.txtMountDec2000.Location = New System.Drawing.Point(208, 132)
        Me.txtMountDec2000.Name = "txtMountDec2000"
        Me.txtMountDec2000.Size = New System.Drawing.Size(95, 20)
        Me.txtMountDec2000.TabIndex = 14
        '
        'txtMountRA2000
        '
        Me.txtMountRA2000.Location = New System.Drawing.Point(208, 106)
        Me.txtMountRA2000.Name = "txtMountRA2000"
        Me.txtMountRA2000.Size = New System.Drawing.Size(95, 20)
        Me.txtMountRA2000.TabIndex = 13
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Location = New System.Drawing.Point(38, 49)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(73, 13)
        Me.Label15.TabIndex = 12
        Me.Label15.Text = "Driver Version"
        Me.Label15.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'lblDriverVersion
        '
        Me.lblDriverVersion.AutoSize = True
        Me.lblDriverVersion.Location = New System.Drawing.Point(117, 49)
        Me.lblDriverVersion.Name = "lblDriverVersion"
        Me.lblDriverVersion.Size = New System.Drawing.Size(31, 13)
        Me.lblDriverVersion.TabIndex = 11
        Me.lblDriverVersion.Text = "0.0.0"
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Location = New System.Drawing.Point(24, 24)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(87, 13)
        Me.Label17.TabIndex = 10
        Me.Label17.Text = "Firmware Version"
        Me.Label17.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'lblFirmwareVersion
        '
        Me.lblFirmwareVersion.AutoSize = True
        Me.lblFirmwareVersion.Location = New System.Drawing.Point(117, 24)
        Me.lblFirmwareVersion.Name = "lblFirmwareVersion"
        Me.lblFirmwareVersion.Size = New System.Drawing.Size(31, 13)
        Me.lblFirmwareVersion.TabIndex = 9
        Me.lblFirmwareVersion.Text = "0.0.0"
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Location = New System.Drawing.Point(38, 186)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(61, 13)
        Me.Label11.TabIndex = 8
        Me.Label11.Text = "Target Dec"
        Me.Label11.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Location = New System.Drawing.Point(43, 160)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(56, 13)
        Me.Label12.TabIndex = 7
        Me.Label12.Text = "Target RA"
        Me.Label12.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'txtTargetDec
        '
        Me.txtTargetDec.Location = New System.Drawing.Point(107, 183)
        Me.txtTargetDec.Name = "txtTargetDec"
        Me.txtTargetDec.Size = New System.Drawing.Size(95, 20)
        Me.txtTargetDec.TabIndex = 6
        '
        'txtTargetRA
        '
        Me.txtTargetRA.Location = New System.Drawing.Point(107, 157)
        Me.txtTargetRA.Name = "txtTargetRA"
        Me.txtTargetRA.Size = New System.Drawing.Size(95, 20)
        Me.txtTargetRA.TabIndex = 5
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(38, 135)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(60, 13)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "Mount Dec"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(43, 109)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(55, 13)
        Me.Label1.TabIndex = 3
        Me.Label1.Text = "Mount RA"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'txtMountDec
        '
        Me.txtMountDec.Location = New System.Drawing.Point(107, 132)
        Me.txtMountDec.Name = "txtMountDec"
        Me.txtMountDec.Size = New System.Drawing.Size(95, 20)
        Me.txtMountDec.TabIndex = 2
        '
        'txtMountRA
        '
        Me.txtMountRA.Location = New System.Drawing.Point(107, 106)
        Me.txtMountRA.Name = "txtMountRA"
        Me.txtMountRA.Size = New System.Drawing.Size(95, 20)
        Me.txtMountRA.TabIndex = 0
        '
        'Timer1
        '
        Me.Timer1.Interval = 2000
        '
        'ToolTipPark
        '
        Me.ToolTipPark.ToolTipTitle = "Park the Mount"
        '
        'ToolTipSlew
        '
        Me.ToolTipSlew.ToolTipTitle = "Slew (Synchronous)"
        '
        'ToolTipSlewAsync
        '
        Me.ToolTipSlewAsync.ToolTipTitle = "Slew (Asynchronous)"
        '
        'ToolTipHalt
        '
        Me.ToolTipHalt.ToolTipTitle = "Halt"
        '
        'ToolTipSlewAxis
        '
        Me.ToolTipSlewAxis.ToolTipTitle = "Manual Slew"
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.Label13)
        Me.GroupBox5.Controls.Add(Me.Label14)
        Me.GroupBox5.Controls.Add(Me.txtLong)
        Me.GroupBox5.Controls.Add(Me.txtLat)
        Me.GroupBox5.Location = New System.Drawing.Point(379, 101)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(354, 91)
        Me.GroupBox5.TabIndex = 10
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "Site Information"
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Location = New System.Drawing.Point(24, 27)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(66, 13)
        Me.Label13.TabIndex = 18
        Me.Label13.Text = "Site Latitude"
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Location = New System.Drawing.Point(15, 53)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(75, 13)
        Me.Label14.TabIndex = 17
        Me.Label14.Text = "Site Longitude"
        '
        'txtLong
        '
        Me.txtLong.Location = New System.Drawing.Point(104, 50)
        Me.txtLong.Name = "txtLong"
        Me.txtLong.Size = New System.Drawing.Size(84, 20)
        Me.txtLong.TabIndex = 16
        '
        'txtLat
        '
        Me.txtLat.Location = New System.Drawing.Point(104, 24)
        Me.txtLat.Name = "txtLat"
        Me.txtLat.Size = New System.Drawing.Size(84, 20)
        Me.txtLat.TabIndex = 15
        '
        'GroupBox6
        '
        Me.GroupBox6.Location = New System.Drawing.Point(12, 321)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(361, 99)
        Me.GroupBox6.TabIndex = 11
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "Debug Area"
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(745, 432)
        Me.Controls.Add(Me.GroupBox6)
        Me.Controls.Add(Me.GroupBox5)
        Me.Controls.Add(Me.groupBoxInfo)
        Me.Controls.Add(Me.GroupBox3)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Name = "frmMain"
        Me.Text = "OpenAstroTracker ASCOM PC Control - v0.1.0b"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        CType(Me.nud_Decm, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_Decs, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_Decd, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_RAm, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_RAs, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nud_RAh, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox3.ResumeLayout(False)
        Me.groupBoxInfo.ResumeLayout(False)
        Me.groupBoxInfo.PerformLayout()
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox5.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents labelDriverId As System.Windows.Forms.Label
    Private WithEvents buttonConnect As System.Windows.Forms.Button
    Private WithEvents buttonChoose As System.Windows.Forms.Button
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents btnSlewAsync As System.Windows.Forms.Button
    Friend WithEvents btnSlewSync As System.Windows.Forms.Button
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents nud_Decm As System.Windows.Forms.NumericUpDown
    Friend WithEvents nud_Decs As System.Windows.Forms.NumericUpDown
    Friend WithEvents nud_Decd As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents nud_RAm As System.Windows.Forms.NumericUpDown
    Friend WithEvents nud_RAs As System.Windows.Forms.NumericUpDown
    Friend WithEvents nud_RAh As System.Windows.Forms.NumericUpDown
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents groupBoxInfo As System.Windows.Forms.GroupBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents txtMountDec As System.Windows.Forms.TextBox
    Friend WithEvents txtMountRA As System.Windows.Forms.TextBox
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents txtTargetDec As System.Windows.Forms.TextBox
    Friend WithEvents txtTargetRA As System.Windows.Forms.TextBox
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    Friend WithEvents btnHalt As System.Windows.Forms.Button
    Friend WithEvents btnPark As System.Windows.Forms.Button
    Friend WithEvents ToolTipPark As System.Windows.Forms.ToolTip
    Friend WithEvents ToolTipHalt As System.Windows.Forms.ToolTip
    Friend WithEvents ToolTipSlewAsync As System.Windows.Forms.ToolTip
    Friend WithEvents ToolTipSlew As System.Windows.Forms.ToolTip
    Friend WithEvents btnEast As System.Windows.Forms.Button
    Friend WithEvents btnWest As System.Windows.Forms.Button
    Friend WithEvents btnSouth As System.Windows.Forms.Button
    Friend WithEvents btnNorth As System.Windows.Forms.Button
    Friend WithEvents ToolTipSlewAxis As System.Windows.Forms.ToolTip
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents txtLong As System.Windows.Forms.TextBox
    Friend WithEvents txtLat As System.Windows.Forms.TextBox
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents lblDriverVersion As System.Windows.Forms.Label
    Friend WithEvents Label17 As System.Windows.Forms.Label
    Friend WithEvents lblFirmwareVersion As System.Windows.Forms.Label
    Friend WithEvents Label18 As System.Windows.Forms.Label
    Friend WithEvents Label16 As System.Windows.Forms.Label
    Friend WithEvents txtTargetDec2000 As System.Windows.Forms.TextBox
    Friend WithEvents txtTargetRA2000 As System.Windows.Forms.TextBox
    Friend WithEvents txtMountDec2000 As System.Windows.Forms.TextBox
    Friend WithEvents txtMountRA2000 As System.Windows.Forms.TextBox

End Class
