Imports System.Windows.Forms
Imports System.Runtime.InteropServices
Imports ASCOM.Utilities
Imports ASCOM.OpenAstroTracker

<ComVisible(False)> _
Public Class SetupDialogForm

    Private Sub OK_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OK_Button.Click ' OK button event handler
        ' Persist new values of user settings to the ASCOM profile
        Telescope.comPort = ComboBoxComPort.SelectedItem ' Update the state variables with results from the dialogue
        Telescope.traceState = chkTrace.Checked
        Telescope.latitude = CDbl(txtLat.Text)
        Telescope.longitude = CDbl(txtLong.Text)
        Telescope.elevation = CInt(txtElevation.Text)
        Me.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.Close()
    End Sub

    Private Sub Cancel_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Cancel_Button.Click 'Cancel button event handler
        Me.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Close()
    End Sub

    Private Sub ShowAscomWebPage(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles PictureBox1.DoubleClick, PictureBox1.Click
        ' Click on ASCOM logo event handler
        Try
            System.Diagnostics.Process.Start("http://ascom-standards.org/")
        Catch noBrowser As System.ComponentModel.Win32Exception
            If noBrowser.ErrorCode = -2147467259 Then
                MessageBox.Show(noBrowser.Message)
            End If
        Catch other As System.Exception
            MessageBox.Show(other.Message)
        End Try
    End Sub

    Private Sub SetupDialogForm_Load(sender As System.Object, e As System.EventArgs) Handles MyBase.Load ' Form load event handler
        ' Retrieve current values of user settings from the ASCOM Profile
        InitUI()
    End Sub

    Private Sub InitUI()
        chkTrace.Checked = Telescope.traceState
        ' set the list of com ports to those that are currently available
        ComboBoxComPort.Items.Clear()
        ComboBoxComPort.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames())       ' use System.IO because it's static
        ' select the current port if possible...
        If ComboBoxComPort.Items.Contains(Telescope.comPort) Then
            ComboBoxComPort.SelectedItem = Telescope.comPort
        End If
        txtLat.Text = Telescope.latitude.ToString
        txtLong.Text = Telescope.longitude.ToString
        txtElevation.Text = Telescope.elevation.ToString
    End Sub

    Private Sub txtLat_KeyPress(sender As Object, e As KeyPressEventArgs) Handles txtLat.KeyPress
        If Not Char.IsDigit(e.KeyChar) And Not Char.IsControl(e.KeyChar) And Not e.KeyChar = "." And Not e.KeyChar = "," And Not e.KeyChar = "-" Then
            e.Handled = True
        End If
    End Sub

    Private Sub txtLong_KeyPress(sender As Object, e As KeyPressEventArgs) Handles txtLong.KeyPress
        If Not Char.IsDigit(e.KeyChar) And Not Char.IsControl(e.KeyChar) And Not e.KeyChar = "." And Not e.KeyChar = "," And Not e.KeyChar = "-" Then
            e.Handled = True
        End If
    End Sub

    Private Sub txtElevation_KeyPress(sender As Object, e As KeyPressEventArgs) Handles txtElevation.KeyPress
        If Not Char.IsDigit(e.KeyChar) And Not Char.IsControl(e.KeyChar) Then
            e.Handled = True
        End If
    End Sub


End Class
