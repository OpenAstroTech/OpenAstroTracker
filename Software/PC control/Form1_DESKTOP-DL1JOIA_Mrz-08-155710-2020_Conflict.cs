using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace OpenAstroTracker_Control
{
    public partial class Form1 : Form
    {
        bool isConnected = false;
        String[] ports;
        SerialPort port;
        int asdf;

        public Form1()
        {
            InitializeComponent();
            disableControls();
            getAvailableComPorts();

            foreach (string port in ports)
            {
                comboBox1.Items.Add(port);
                Console.WriteLine(port);
                if (ports[0] != null)
                {
                    comboBox1.SelectedItem = ports[0];
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
            string selectedPort = comboBox1.GetItemText(comboBox1.SelectedItem);
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
            button2.Enabled = true;
            button3.Enabled = true;
            button4.Enabled = true;
            button5.Enabled = true;
            button6.Enabled = true;
            button7.Enabled = true;            
            numericUpDown1.Enabled = true;
            numericUpDown2.Enabled = true;
            numericUpDown3.Enabled = true;
            numericUpDown4.Enabled = true;
            numericUpDown5.Enabled = true;
            numericUpDown6.Enabled = true;
            numericUpDown7.Enabled = true;
        }

        private void disableControls()
        {
            button2.Enabled = false;
            button3.Enabled = false;
            button4.Enabled = false;
            button5.Enabled = false;
            button6.Enabled = false;
            button7.Enabled = false;            
            numericUpDown1.Enabled = false;
            numericUpDown2.Enabled = false;
            numericUpDown3.Enabled = false;
            numericUpDown4.Enabled = false;
            numericUpDown5.Enabled = false;
            numericUpDown6.Enabled = false;
            numericUpDown7.Enabled = false;
        }      

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button5_Click(object sender, EventArgs e)
        {            
            if (isConnected)
            {
                port.Write(numericUpDown1.Value + "\a" + "R#");
            }                   
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (isConnected)
            {
                port.Write(numericUpDown1.Value + "\a" + "L#");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (isConnected)
            {
                port.Write(numericUpDown1.Value + "\a" + "UP#");
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (isConnected)
            {
                port.Write(numericUpDown1.Value + "\a" + "DOWN#");
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            if (isConnected)
            {
                port.Write(numericUpDown2.Value + "\a" + numericUpDown3.Value + "\b" + numericUpDown4.Value + "\f" + "S");
            }
        }

        
    }
    
}
