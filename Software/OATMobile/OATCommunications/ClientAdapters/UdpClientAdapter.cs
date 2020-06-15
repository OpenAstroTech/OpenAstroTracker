using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace OATCommunications.ClientAdapters
{
	public class UdpClientAdapter : IDisposable
	{
		public event EventHandler<ClientFoundEventArgs> ClientFound;

		private readonly string _hostToFind;
		private int _port;
		private UdpClient _udpClient;
		private bool disposedValue;

		public UdpClientAdapter(string hostToFind, int port)
		{
			_hostToFind = hostToFind;
			_port = port;
			// create a udp client to listen for requests
			_udpClient = new UdpClient();
		}

		public void StartClientSearch()
		{
			_udpClient = new UdpClient();
			_udpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
			// listen for computers other than localhost
			_udpClient.ExclusiveAddressUse = false;

			// IPAddress.Any allows us to listen to broadcast UDP packets
			_udpClient.Client.Bind(new IPEndPoint(IPAddress.Any, _port));

			var data = Encoding.ASCII.GetBytes($"skyfi:{_hostToFind}?");
			_udpClient.Send(data, data.Length, new IPEndPoint(IPAddress.Broadcast, _port));
			_udpClient.BeginReceive(new AsyncCallback(this.ReceiveCallback), null);
		}

		private void ReceiveCallback(IAsyncResult ar)
		{
			IPEndPoint remoteIpEndPoint = new IPEndPoint(IPAddress.Any, _port);
			byte[] received = _udpClient.EndReceive(ar, ref remoteIpEndPoint);
			string result = Encoding.UTF8.GetString(received);
			var parts = result.Split(":@".ToCharArray(),StringSplitOptions.RemoveEmptyEntries);
			if ((parts.Length == 3) && parts[0].Equals("skyfi"))
			{
				OnClientFound(new ClientFoundEventArgs(parts[1], IPAddress.Parse(parts[2])));
			}
			_udpClient.BeginReceive(new AsyncCallback(this.ReceiveCallback), null);
		}

		public string SendCommand(string command)
		{
			throw new NotImplementedException();
		}

		protected virtual void OnClientFound(ClientFoundEventArgs e)
		{
			var handler = ClientFound;
			handler?.Invoke(this, e);
		}

		public class ClientFoundEventArgs : EventArgs
		{
			public ClientFoundEventArgs(string name, IPAddress ip)
			{
				Name = name;
				Address = ip;
			}

			public string Name { get; set; }
			public IPAddress Address { get; set; }
		}

		protected virtual void Dispose(bool disposing)
		{
			if (!disposedValue)
			{
				if (disposing)
				{
					_udpClient?.Dispose();
				}

				// TODO: free unmanaged resources (unmanaged objects) and override finalizer
				// TODO: set large fields to null
				disposedValue = true;
			}
		}

		// // TODO: override finalizer only if 'Dispose(bool disposing)' has code to free unmanaged resources
		// ~UdpClientAdapter()
		// {
		//     // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
		//     Dispose(disposing: false);
		// }

		public void Dispose()
		{
			// Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
			Dispose(disposing: true);
			GC.SuppressFinalize(this);
		}
	}
}
