using OATCommunications.Utilities;
using System;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace OATCommunications.CommunicationHandlers
{
	public class TcpCommunicationHandler : ICommunicationHandler
	{
		private IPAddress _ip;
		private int _port;
		private TcpClient _client;

		public TcpCommunicationHandler(string spec)
		{
			string ip = string.Empty;
			string port = string.Empty;

			var colon = spec.IndexOf(':');
			if (colon > 0)
			{
				try
				{
					ip = spec.Substring(0, colon);
					port = spec.Substring(colon + 1);
					_ip = IPAddress.Parse(ip);
					_port = int.Parse(port);
					_client = new TcpClient();
				}
				catch
				{

				}
			}
		}

		public TcpCommunicationHandler(IPAddress ip, int port)
		{
			_ip = ip;
			_port = port;
			_client = new TcpClient();
		}

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
			if (_client == null)
			{
				return new CommandResponse(string.Empty, false, $"Configuration error, IP [{_ip}] or port [{_port}] is invalid.");
			}

			int attempt = 1;
			var respString = String.Empty;

			while ((attempt < 4) && (_client != null))
			{
				Log.WriteLine("TCP: [{0}] Attempt {1} to send command.", command, attempt);
				if (!_client.Connected)
				{
					try
					{
						_client = new TcpClient();
						_client.Connect(_ip, _port);
					}
					catch (Exception e)
					{
						Log.WriteLine("TCP: [{0}] Failed To connect or create client for command: {1}", command, e.Message);
						return new CommandResponse("", false, $"Failed To Connect to Client: {e.Message}");
					}
				}

				_client.ReceiveTimeout = 500;
				_client.SendTimeout = 500;

				string error = String.Empty;

				var stream = _client.GetStream();
				var bytes = Encoding.ASCII.GetBytes(command);
				try
				{
					await stream.WriteAsync(bytes, 0, bytes.Length);
					Log.WriteLine("TCP: [{0}] Sent command!", command);
				}
				catch (Exception e)
				{
					Log.WriteLine("TCP: [{0}] Unable to write command to stream: {1}", command, e.Message);
					return new CommandResponse("", false, $"Failed to send message: {e.Message}");
				}

				try
				{
					switch (needsResponse)
					{
						case ResponseType.NoResponse:
							attempt = 10;
							Log.WriteLine("TCP: [{0}] No reply needed to command", command);
							break;

						case ResponseType.DigitResponse:
						case ResponseType.FullResponse:
							{
								var response = new byte[256];
								var respCount = await stream.ReadAsync(response, 0, response.Length);
								respString = Encoding.ASCII.GetString(response, 0, respCount).TrimEnd("#".ToCharArray());
								Log.WriteLine("TCP: [{0}] Received reply to command -> [{1}]", command, respString);
								attempt = 10;
							}
							break;
					}
				}
				catch (Exception e)
				{
					Log.WriteLine("TCP: [{0}] Failed to read reply to command. {1} thrown", command, e.GetType().Name);
					if (needsResponse != ResponseType.NoResponse)
					{
						respString = "0#";
					}
				}

				stream.Close();
				attempt++;
			}

			return new CommandResponse(respString);
		}

		public bool Connected
		{
			get
			{
				return _client != null && _client.Connected;
			}
		}

		public void Disconnect()
		{
			if (_client != null && _client.Connected)
			{
				_client.Close();
				_client = null;
			}
		}
	}
}