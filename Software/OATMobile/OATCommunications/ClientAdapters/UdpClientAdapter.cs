using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace OATCommunications.ClientAdapters
{
    class UdpClientAdapter
    {
        public event EventHandler<ClientFoundEventArgs> ClientFound;

        private readonly string _hostToFind;
        private int _port;

        public UdpClientAdapter(string hostToFind, int port) {
            _hostToFind = hostToFind;
            _port = port;
        }

        public void StartClientSearch()
        {
            using (UdpClient c = new UdpClient(_port))
            {
                var data = Encoding.ASCII.GetBytes($"skyfi:{_hostToFind}?");
                c.Send(data, data.Length);
            }
        }

        private void ClientListener() {
            // create a udp client to listen for requests
            var udpClient = new UdpClient();
            udpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            // listen for computers other than localhost
            udpClient.ExclusiveAddressUse = false;

            // IPAddress.Any allows us to listen to broadcast UDP packets
            udpClient.Client.Bind(new IPEndPoint(IPAddress.Any, _port));

            var from = new IPEndPoint(0, 0);
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
            public string Name { get; set; }
            public IPAddress Address { get; set; }
        }
    }
}
