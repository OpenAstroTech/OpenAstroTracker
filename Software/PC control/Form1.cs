using System;
using System.Windows.Forms;
using System.IO.Ports;

namespace OpenAstroTracker_Control
{
    public partial class Form1 : Form
    {
        bool isConnected = false;
        String[] ports;
        SerialPort port;

        public Form1()
        {
            InitializeComponent();
            disableControls();
            getAvailableComPorts();

            foreach (string port in ports)
            {
                cbPorts.Items.Add(port);
                Console.WriteLine(port);
                if (ports[0] != null)
                {
                    cbPorts.SelectedItem = ports[0];
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (!isConnected)
            {
                connectToArduino();
            }
            else
            {
                disconnectFromArduino();
            }
        }

        void getAvailableComPorts()
        {
            ports = SerialPort.GetPortNames();
        }

        private void connectToArduino()
        {
            isConnected = true;
            string selectedPort = cbPorts.GetItemText(cbPorts.SelectedItem);
            port = new SerialPort(selectedPort, 57600, Parity.None, 8, StopBits.One);
            port.Open();
            port.Write("START#");
            button1.Text = "Disconnect";
            enableControls();
        }
        private void disconnectFromArduino()
        {
            isConnected = false;
            port.Write("STOP#");
            port.Close();
            button1.Text = "Connect";
            disableControls();
            //resetDefaults();
        }

        private void enableControls()
        {
            btnManualUp.Enabled = true;
            btnManualDown.Enabled = true;
            btnManualLeft.Enabled = true;
            btnManualRight.Enabled = true;

            btnRAMove.Enabled = true;
            btnDecMove.Enabled = true;            

            udManualSteps.Enabled = true;

            udRAHours.Enabled = true;
            udRAMinutes.Enabled = true;
            udRASeconds.Enabled = true;

            udDecSeconds.Enabled = true;
            udDecMinutes.Enabled = true;
            udDecDegrees.Enabled = true;
        }

        private void disableControls()
        {
            btnManualUp.Enabled = false;
            btnManualDown.Enabled = false;
            btnManualLeft.Enabled = false;
            btnManualRight.Enabled = false;

            btnRAMove.Enabled = false;
            btnDecMove.Enabled = false;

            udManualSteps.Enabled = false;

            udRAHours.Enabled = false;
            udRAMinutes.Enabled = false;
            udRASeconds.Enabled = false;

            udDecSeconds.Enabled = false;
            udDecMinutes.Enabled = false;
            udDecDegrees.Enabled = false;
        }      

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void btnRAMove_Click(object sender, EventArgs e)
        {
            if (!isConnected)
                return;

            port.Write(udRAHours.Value + "\a" + udRAMinutes.Value + "\b" + udRASeconds.Value + "\f" + "S");
        }

        private void btnDecMove_Click(object sender, EventArgs e)
        {
            if (!isConnected)
                return;

            // move command missing here
        }

        private void btnManualUp_Click(object sender, EventArgs e)
        {
            if (!isConnected)
                return;

            port.Write(udManualSteps.Value + "\a" + "UP#");
        }

        private void btnManualLeft_Click(object sender, EventArgs e)
        {
            if (!isConnected)
                return;

            port.Write(udManualSteps.Value + "\a" + "L#");
        }

        private void btnManualRight_Click(object sender, EventArgs e)
        {
            if (!isConnected)
                return;

            port.Write(udManualSteps.Value + "\a" + "R#");
        }

        private void btnManualDown_Click(object sender, EventArgs e)
        {
            if (!isConnected)
                return;

            port.Write(udManualSteps.Value + "\a" + "DOWN#");
        }
    }
}
