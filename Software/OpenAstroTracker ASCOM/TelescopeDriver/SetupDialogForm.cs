using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ASCOM.OpenAstroTracker {
    [ComVisible(false)] // Form not registered for COM!
    public partial class SetupDialogForm : Form {
        private ProfileData _profile;

        public SetupDialogForm(ProfileData profile) {
            _profile = profile;
            InitializeComponent();
        }

        private void OK_Button_Click(System.Object sender, System.EventArgs e) // OK button event handler
        {
            // Persist new values of user settings to the ASCOM profile
            _profile.ComPort =
                (string) ComboBoxComPort.SelectedItem; // Update the state variables with results from the dialogue
            _profile.TraceState = chkTrace.Checked;
            _profile.Latitude = System.Convert.ToDouble(txtLat.Text);
            _profile.Longitude = System.Convert.ToDouble(txtLong.Text);
            _profile.Elevation = System.Convert.ToInt32(txtElevation.Text);
            this.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.Close();
        }

        public ProfileData GetProfileData() => _profile;

        private void Cancel_Button_Click(System.Object sender, System.EventArgs e) // Cancel button event handler
        {
            this.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Close();
        }

        private void ShowAscomWebPage(System.Object sender, System.EventArgs e) {
            // Click on ASCOM logo event handler
            try {
                System.Diagnostics.Process.Start("http://ascom-standards.org/");
            }
            catch (Win32Exception noBrowser) {
                if (noBrowser.ErrorCode == -2147467259)
                    MessageBox.Show(noBrowser.Message);
            }
            catch (Exception other) {
                MessageBox.Show(other.Message);
            }
        }

        private void SetupDialogForm_Load(System.Object sender, System.EventArgs e) // Form load event handler
        {
            // Retrieve current values of user settings from the ASCOM Profile
            InitUI();
        }

        private void InitUI() {
            chkTrace.Checked = _profile.TraceState;
            // set the list of com ports to those that are currently available
            ComboBoxComPort.Items.Clear();
            ComboBoxComPort.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames()); // use System.IO because it's static
            // select the current port if possible...
            if (ComboBoxComPort.Items.Contains(_profile.ComPort))
                ComboBoxComPort.SelectedItem = _profile.ComPort;
            txtLat.Text = _profile.Latitude.ToString();
            txtLong.Text = _profile.Longitude.ToString();
            txtElevation.Text = _profile.Elevation.ToString();
        }

        private void txtLat_KeyPress(object sender, KeyPressEventArgs e) {
            if (!char.IsDigit(e.KeyChar) && !char.IsControl(e.KeyChar) && e.KeyChar != '.' && e.KeyChar != ',' &&
                e.KeyChar != '-')
                e.Handled = true;
        }

        private void txtLong_KeyPress(object sender, KeyPressEventArgs e) {
            if (!char.IsDigit(e.KeyChar) && !char.IsControl(e.KeyChar) && e.KeyChar != '.' && e.KeyChar != ',' &&
                e.KeyChar != '-')
                e.Handled = true;
        }

        private void txtElevation_KeyPress(object sender, KeyPressEventArgs e) {
            if (!char.IsDigit(e.KeyChar) && !char.IsControl(e.KeyChar))
                e.Handled = true;
        }
    }
}