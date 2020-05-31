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
			return await SendCommand(command, false);
		}

		public async Task<CommandResponse> SendCommand(string command)
		{
			return await SendCommand(command, true);
		}

		public bool Connected
		{
			get
			{
				return _client != null && _client.Connected;
			}
		}

		private async Task<CommandResponse> SendCommand(string command, bool needsResponse)
		{
			if (_client == null)
			{
				return new CommandResponse(string.Empty, false, $"Configuration error, IP [{_ip}] or port [{_port}] is invalid.");
			}

			if (!_client.Connected)
			{
				try
				{
					_client = new TcpClient();
					_client.Connect(_ip, _port);
				}
				catch (Exception e)
				{
					Debug.WriteLine($"Failed To connect or create client: \n{e.Message}");
					return new CommandResponse("", false, $"Failed To Connect to Client: {e.Message}");
				}
			}

			_client.ReceiveTimeout = 250;
			_client.SendTimeout = 250;

			string error = String.Empty;

			var stream = _client.GetStream();
			var bytes = Encoding.ASCII.GetBytes(command);
			try
			{
				await stream.WriteAsync(bytes, 0, bytes.Length);
			}
			catch (Exception e)
			{
				Debug.WriteLine(e.Message);
				return new CommandResponse("", false, $"Failed to send message: {e.Message}");
			}

			Debug.WriteLine($"Sent {command}");

			var respString = String.Empty;

			if (needsResponse)
			{
				try
				{
					var response = new byte[256];
					var respCount = await stream.ReadAsync(response, 0, response.Length);
					respString = Encoding.ASCII.GetString(response, 0, respCount).TrimEnd("#".ToCharArray());
					Debug.WriteLine($"Received {respString}");
				}
				catch (Exception e)
				{
					Debug.WriteLine(e.Message);
					return new CommandResponse("", false, $"Failed to receive message: {e.Message}");
				}
			}

			stream.Close();

			return new CommandResponse(respString);
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