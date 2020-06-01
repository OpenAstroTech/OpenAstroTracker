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
		}

		public bool Connected => _port.IsOpen;

		public async Task<CommandResponse> SendBlind(string command)
		{
			return await SendCommand(command, false);
		}

		public async Task<CommandResponse> SendCommand(string command)
		{
			return await SendCommand(command, true);
		}

		public async Task<CommandResponse> SendCommand(string command, bool needsResponse)
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

				if (needsResponse)
				{
					try
					{
						var response = _port.ReadTo("#");
						return new CommandResponse(response, true);
					}
					catch
					{
						return new CommandResponse(string.Empty, false, $"Unable to read response to {command} from {_portName}");
					}
				}
				return new CommandResponse(string.Empty, true);
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
				_port.Close();
			}
		}

	}
}
