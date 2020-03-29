using System;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;

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
			try
			{
				string selectedPort = cbPorts.GetItemText(cbPorts.SelectedItem);
				port = new SerialPort(selectedPort, 9600, Parity.None, 8, StopBits.One);
				Thread.Sleep(500);
				port.Open();
				Thread.Sleep(500);
				port.Write("|START#");
				button1.Text = "Disconnect";
				enableControls();
				isConnected = true;
			}
			catch (Exception ex)
			{
				MessageBox.Show("Unable to connect. " + ex.Message, "Failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		private void disconnectFromArduino()
		{
			isConnected = false;
			port.Write("|STOP#");
			button1.Text = "Connect";
			disableControls();
			Thread.Sleep(500);
			port.Close();
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

			port.Write(udRAHours.Value + "," + udRAMinutes.Value + "," + udRASeconds.Value + "|SRA#");
		}

		private void btnDecMove_Click(object sender, EventArgs e)
		{
			if (!isConnected)
				return;

			port.Write(udDecDegrees.Value + "," + udDecMinutes.Value + "," + udDecSeconds.Value + "|SDEC#");
		}

		private void btnManualUp_Click(object sender, EventArgs e)
		{
			if (!isConnected)
				return;

			port.Write(udManualSteps.Value + "|UP#");
		}

		private void btnManualLeft_Click(object sender, EventArgs e)
		{
			if (!isConnected)
				return;

			port.Write(udManualSteps.Value + "|LEFT#");
		}

		private void btnManualRight_Click(object sender, EventArgs e)
		{
			if (!isConnected)
				return;

			port.Write(udManualSteps.Value + "|RIGHT#");
		}

		private void btnManualDown_Click(object sender, EventArgs e)
		{
			if (!isConnected)
				return;

			port.Write(udManualSteps.Value + "|DOWN#");
		}
	}
}
