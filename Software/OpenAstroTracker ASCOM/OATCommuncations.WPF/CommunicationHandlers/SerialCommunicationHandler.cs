using OATCommunications.CommunicationHandlers;
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
					_port.Write(command);
				}
				catch
				{
					return new CommandResponse(string.Empty, false, $"Unable to write to {_portName}");
				}

				try
				{
					switch (needsResponse)
					{
						case ResponseType.NoResponse:
							{
								return new CommandResponse(string.Empty, true);
							}

						case ResponseType.DigitResponse:
							{
								string response = new string((char)_port.ReadChar(), 1);
								return new CommandResponse(response, true);
							}

						case ResponseType.FullResponse:
							{
								string response = _port.ReadTo("#");
								return new CommandResponse(response, true);
							}
					}
				}
				catch (Exception ex)
				{
					return new CommandResponse(string.Empty, false, $"Unable to read response to {command} from {_portName}. {ex.Message}");
				}

				return new CommandResponse(string.Empty, false, "Something weird going on...");
			}
			else
			{
				return new CommandResponse(string.Empty, false, $"Unable to open {_portName}");
			}
		}

		private async Task<bool> EnsurePortIsOpen()
		{
			if (!_port.IsOpen)
			{
				try
				{
					_port.Open();
					await Task.Delay(750); // Arduino resets on connection. Give it time to start up.
					_port.Write(":I#");
					return true;
				}
				catch
				{
					return false;
				}
			}
			return true;
		}

		public void Disconnect()
		{
			if (_port.IsOpen)
			{
				_port.Write(":Qq#");
				_port.Close();
				_port = null;
			}
		}

	}
}
