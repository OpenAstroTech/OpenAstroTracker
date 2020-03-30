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
        Me.GroupBox4 = New System.Windows.Forms.GroupBox()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.txtTargetDec = New System.Windows.Forms.TextBox()
        Me.txtTargetRA = New System.Windows.Forms.TextBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.txtMountDec = New System.Windows.Forms.TextBox()
        Me.txtMountRA = New System.Windows.Forms.TextBox()
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.btnHalt = New System.Windows.Forms.Button()
        Me.btnPark = New System.Windows.Forms.Button()
        Me.lblVersion = New System.Windows.Forms.Label()
        Me.ToolTipPark = New System.Windows.Forms.ToolTip(Me.components)
        Me.ToolTipSlew = New System.Windows.Forms.ToolTip(Me.components)
        Me.ToolTipSlewAsync = New System.Windows.Forms.ToolTip(Me.components)
        Me.ToolTipHalt = New System.Windows.Forms.ToolTip(Me.components)
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        CType(Me.nud_Decm, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_Decs, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_Decd, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_RAm, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_RAs, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nud_RAh, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox4.SuspendLayout()
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
        'btnSlewAsync
        '
        Me.btnSlewAsync.Enabled = False
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
        Me.GroupBox3.Location = New System.Drawing.Point(12, 198)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(361, 180)
        Me.GroupBox3.TabIndex = 8
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "Manual Slew"
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.lblVersion)
        Me.GroupBox4.Controls.Add(Me.Label11)
        Me.GroupBox4.Controls.Add(Me.Label12)
        Me.GroupBox4.Controls.Add(Me.txtTargetDec)
        Me.GroupBox4.Controls.Add(Me.txtTargetRA)
        Me.GroupBox4.Controls.Add(Me.Label2)
        Me.GroupBox4.Controls.Add(Me.Label1)
        Me.GroupBox4.Controls.Add(Me.txtMountDec)
        Me.GroupBox4.Controls.Add(Me.txtMountRA)
        Me.GroupBox4.Location = New System.Drawing.Point(379, 101)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(354, 277)
        Me.GroupBox4.TabIndex = 9
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "Mount Information"
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Location = New System.Drawing.Point(19, 104)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(61, 13)
        Me.Label11.TabIndex = 8
        Me.Label11.Text = "Target Dec"
        Me.Label11.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Location = New System.Drawing.Point(24, 78)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(56, 13)
        Me.Label12.TabIndex = 7
        Me.Label12.Text = "Target RA"
        Me.Label12.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'txtTargetDec
        '
        Me.txtTargetDec.Location = New System.Drawing.Point(85, 101)
        Me.txtTargetDec.Name = "txtTargetDec"
        Me.txtTargetDec.Size = New System.Drawing.Size(95, 20)
        Me.txtTargetDec.TabIndex = 6
        '
        'txtTargetRA
        '
        Me.txtTargetRA.Location = New System.Drawing.Point(85, 75)
        Me.txtTargetRA.Name = "txtTargetRA"
        Me.txtTargetRA.Size = New System.Drawing.Size(95, 20)
        Me.txtTargetRA.TabIndex = 5
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(19, 53)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(60, 13)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "Mount Dec"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(24, 27)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(55, 13)
        Me.Label1.TabIndex = 3
        Me.Label1.Text = "Mount RA"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'txtMountDec
        '
        Me.txtMountDec.Location = New System.Drawing.Point(85, 50)
        Me.txtMountDec.Name = "txtMountDec"
        Me.txtMountDec.Size = New System.Drawing.Size(95, 20)
        Me.txtMountDec.TabIndex = 2
        '
        'txtMountRA
        '
        Me.txtMountRA.Location = New System.Drawing.Point(85, 24)
        Me.txtMountRA.Name = "txtMountRA"
        Me.txtMountRA.Size = New System.Drawing.Size(95, 20)
        Me.txtMountRA.TabIndex = 0
        '
        'Timer1
        '
        Me.Timer1.Interval = 1000
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
        'btnPark
        '
        Me.btnPark.Location = New System.Drawing.Point(117, 118)
        Me.btnPark.Name = "btnPark"
        Me.btnPark.Size = New System.Drawing.Size(96, 23)
        Me.btnPark.TabIndex = 17
        Me.btnPark.Text = "Park"
        Me.ToolTipPark.SetToolTip(Me.btnPark, "Returns mount to home position, and stops motors.  (NOTE : Motors do not currentl" & _
        "y stop!)")
        Me.btnPark.UseVisualStyleBackColor = True
        '
        'lblVersion
        '
        Me.lblVersion.AutoSize = True
        Me.lblVersion.Location = New System.Drawing.Point(214, 24)
        Me.lblVersion.Name = "lblVersion"
        Me.lblVersion.Size = New System.Drawing.Size(134, 13)
        Me.lblVersion.TabIndex = 9
        Me.lblVersion.Text = "OpenAstroTracker V0.0.00"
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
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(745, 391)
        Me.Controls.Add(Me.GroupBox4)
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
        Me.GroupBox4.ResumeLayout(False)
        Me.GroupBox4.PerformLayout()
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
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
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
    Friend WithEvents lblVersion As System.Windows.Forms.Label
    Friend WithEvents ToolTipPark As System.Windows.Forms.ToolTip
    Friend WithEvents ToolTipHalt As System.Windows.Forms.ToolTip
    Friend WithEvents ToolTipSlewAsync As System.Windows.Forms.ToolTip
    Friend WithEvents ToolTipSlew As System.Windows.Forms.ToolTip

End Class
