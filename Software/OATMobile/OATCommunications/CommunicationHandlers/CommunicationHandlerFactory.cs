using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using OATCommunications.ClientAdapters;
using static OATCommunications.ClientAdapters.UdpClientAdapter;

namespace OATCommunications.CommunicationHandlers
{
	public static class CommunicationHandlerFactory
	{
		static IList<string> _available = new List<string>();
		public static void DiscoverDevices()
		{
			foreach (var port in SerialPort.GetPortNames())
			{
				_available.Add("Serial : " + port);
			}

			var searcher = new UdpClientAdapter("OATerScope", 4030);
			searcher.ClientFound += OnWifiClientFound;
			searcher.StartClientSearch();
		}

		private static void OnWifiClientFound(object sender, ClientFoundEventArgs e)
		{
			_available.Add($"WiFi : {e.Name} ({e.Address})");
		}

		public static IList<String> AvailableDevices { get { return _available; } }

		public static ICommunicationHandler ConnectToDevice(string device)
		{
			if (device.StartsWith("Serial : "))
			{
				string comPort = device.Substring("Serial : ".Length);
				return new SerialCommunicationHandler(comPort);
			}
			else if (device.StartsWith("WiFi : "))
			{
				string ipAddress = device.Substring("WiFi : ".Length);
				return new TcpCommunicationHandler(ipAddress);
			}

			return null;
		}

	}
}
