using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using ASCOM.Utilities;
using ASCOM.OpenAstroGuider;

namespace ASCOM.OpenAstroGuider
{
    [ComVisible(false)]					// Form not registered for COM!
    public partial class SetupDialogForm : Form
    {
        public SetupDialogForm()
        {
            InitializeComponent();
            // Initialise current values of user settings from the ASCOM Profile 
            
            comboBoxComPort.Items.Clear();
            using (ASCOM.Utilities.Serial serial = new Utilities.Serial())
            {
                foreach (var item in serial.AvailableCOMPorts)
                {
                    comboBoxComPort.Items.Add(item);
                    comboBoxComPort.SelectedIndex = 0;
                }
            }
            chkTrace.Checked = Telescope.traceState;
        }

        private void cmdOK_Click(object sender, EventArgs e) // OK button event handler
        {
            // Place any validation constraint checks here

            Telescope.comPort = (string)comboBoxComPort.SelectedItem; // Update the state variables with results from the dialogue
            Telescope.traceState = chkTrace.Checked;
            Close();
        }

        private void cmdCancel_Click(object sender, EventArgs e) // Cancel button event handler
        {
            Close();
        }

        private void BrowseToAscom(object sender, EventArgs e) // Click on ASCOM logo event handler
        {
            try
            {
                System.Diagnostics.Process.Start("http://ascom-standards.org/");
            }
            catch (System.ComponentModel.Win32Exception noBrowser)
            {
                if (noBrowser.ErrorCode == -2147467259)
                    MessageBox.Show(noBrowser.Message);
            }
            catch (System.Exception other)
            {
                MessageBox.Show(other.Message);
            }
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
    }
}