using OATCommunications.CommunicationHandlers;
using OATCommunications.Utilities;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace OATCommunications.WPF.CommunicationHandlers
{
	public class SerialCommunicationHandler : ICommunicationHandler
	{
		private string _portName;
		private SerialPort _port;

		public SerialCommunicationHandler(string comPort)
		{
			Log.WriteLine($"COMMFACTORY: Creating Serial handler on {comPort} at 57600 baud...");

			_portName = comPort;
			_port = new SerialPort(comPort);
			_port.BaudRate = 57600;
			_port.DtrEnable = false;
			_port.ReadTimeout = 250;
			_port.WriteTimeout = 250;
		}

		public bool Connected => _port.IsOpen;

		public async Task<CommandResponse> SendBlind(string command)
		{
			return await SendCommand(command, ResponseType.NoResponse);
		}

		public async Task<CommandResponse> SendCommand(string command)
		{
			return await SendCommand(command, ResponseType.FullResponse);
		}

		public async Task<CommandResponse> SendCommandConfirm(string command)
		{
			return await SendCommand(command, ResponseType.DigitResponse);
		}

		private async Task<CommandResponse> SendCommand(string command, ResponseType needsResponse)
		{
			if (await EnsurePortIsOpen())
			{
				try
				{
					Log.WriteLine("SERIAL: [{0}] Sending command", command);
					_port.Write(command);
				}
				catch (Exception ex)
				{
					Log.WriteLine("SERIAL: [{0}] Failed to send command. {1}", command, ex.Message);
					return new CommandResponse(string.Empty, false, $"Unable to write to {_portName}. " + ex.Message);
				}

				try
				{
					switch (needsResponse)
					{
						case ResponseType.NoResponse:
							{
								Log.WriteLine("SERIAL: [{0}] No response needed for command", command);
								return new CommandResponse(string.Empty, true);
							}

						case ResponseType.DigitResponse:
							{
								Log.WriteLine("SERIAL: [{0}] Expecting single digit response for command, waiting...", command);
								string response = new string((char)_port.ReadChar(), 1);
								Log.WriteLine("SERIAL: [{0}] Received single digit response '{1}' for command", command, response);
								return new CommandResponse(response, true);
							}

						case ResponseType.FullResponse:
							{
								Log.WriteLine("SERIAL: [{0}] Expecting #-delimited response for Command, waiting...", command);
								string response = _port.ReadTo("#");
								Log.WriteLine("SERIAL: [{0}] Received response '{1}' for command", command, response);
								return new CommandResponse(response, true);
							}
					}
				}
				catch (Exception ex)
				{
					Log.WriteLine("SERIAL: [{0}] Failed to receive response to command. {1}", command, ex.Message);
					return new CommandResponse(string.Empty, false, $"Unable to read response to {command} from {_portName}. {ex.Message}");
				}

				return new CommandResponse(string.Empty, false, "Something weird going on...");
			}
			else
			{
				Log.WriteLine("SERIAL: Failed to open port {0}", _portName);
				return new CommandResponse(string.Empty, false, $"Unable to open {_portName}");
			}
		}

		private async Task<bool> EnsurePortIsOpen()
		{
			if (!_port.IsOpen)
			{
				try
				{
					Log.WriteLine("SERIAL: Port {0} is not open, attempting to open...", _portName);
					_port.Open();
					await Task.Delay(750); // Arduino resets on connection. Give it time to start up.
					Log.WriteLine("SERIAL: Port is open, sending initial [:I#] command..");
					_port.Write(":I#");
					return true;
				}
				catch (Exception ex)
				{
					Log.WriteLine("SERIAL: Failed to open the port. {0}", ex.Message);
					return false;
				}
			}
			return true;
		}

		public void Disconnect()
		{
			if (_port.IsOpen)
			{
				Log.WriteLine("SERIAL: Port is open, sending shutdown command [:Qq#]");
				_port.Write(":Qq#");
				Log.WriteLine("SERIAL: Closing port...");
				_port.Close();
				_port = null;
				Log.WriteLine("SERIAL: Disconnected...");
			}
		}
	}
}
