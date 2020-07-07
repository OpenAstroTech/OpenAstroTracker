using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using OATCommunications.ClientAdapters;
using static OATCommunications.ClientAdapters.UdpClientAdapter;
using OATCommunications.CommunicationHandlers;
using System.Collections.ObjectModel;

namespace OATCommunications.CommunicationHandlers
{
	public static class CommunicationHandlerFactory
	{
		static ObservableCollection<string> _available = new ObservableCollection<string>();
		public static void DiscoverDevices()
		{
			_available.Clear();
			//foreach (var port in SerialPort.GetPortNames())
			//{
			//	_available.Add("Serial : " + port);
			//}

			var searcher = new UdpClientAdapter("OAT", 4031);
			searcher.ClientFound += OnWifiClientFound;
			searcher.StartClientSearch();
		}

		private static void OnWifiClientFound(object sender, ClientFoundEventArgs e)
		{
			_available.Insert(0, $"WiFi : {e.Name} ({e.Address}:4030)");
		}

		public static ObservableCollection<String> AvailableDevices { get { return _available; } }

		public static ICommunicationHandler ConnectToDevice(string device)
		{
			//if (device.StartsWith("Serial : "))
			//{
			//	string comPort = device.Substring("Serial : ".Length);
			//	return new SerialCommunicationHandler(comPort);
			//}
			//else 
			if (device.StartsWith("WiFi : "))
			{
				var parts = device.Split("()".ToCharArray());
				string ipAddress = parts[1];
				return new TcpCommunicationHandler(ipAddress);
			}

			return null;
		}

	}
}
