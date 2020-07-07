using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using OATCommunications.ClientAdapters;
using static OATCommunications.ClientAdapters.UdpClientAdapter;
using OATCommunications.CommunicationHandlers;
using System.Collections.ObjectModel;
using OATCommuncations.WPF;
using OATCommunications.Utilities;

namespace OATCommunications.WPF.CommunicationHandlers
{
	public static class CommunicationHandlerFactory
	{
		static ObservableCollection<string> _available = new ObservableCollection<string>();
		public static void DiscoverDevices()
		{
			Log.WriteLine("COMMFACTORY: Device Discovery initiated.");
			Log.WriteLine("COMMFACTORY: Checking Serial ports....");

			_available.Clear();
			foreach (var port in SerialPort.GetPortNames())
			{
				Log.WriteLine("COMMFACTORY: Found Serial port [{0}]", port);
				_available.Add("Serial : " + port);
			}

			Log.WriteLine("COMMFACTORY: Starting Wifi search by Broadcastign OAT on port 4031");
			var searcher = new UdpClientAdapter("OAT", 4031);
			searcher.ClientFound += OnWifiClientFound;
			searcher.StartClientSearch();
		}

		private static void OnWifiClientFound(object sender, ClientFoundEventArgs e) => WpfUtilities.RunOnUiThread(
						   () =>
						   {
							   Log.WriteLine($"COMMFACTORY: Wifi device {e.Name} replied from {e.Address}:4030!");
							   _available.Insert(0, $"WiFi : {e.Name} ({e.Address}:4030)");
						   },
						   System.Windows.Application.Current.Dispatcher);

		public static ObservableCollection<String> AvailableDevices { get { return _available; } }

		public static ICommunicationHandler ConnectToDevice(string device)
		{
			Log.WriteLine($"COMMFACTORY: Attempting to connect to device {device}...");
			if (device.StartsWith("Serial : "))
			{
				string comPort = device.Substring("Serial : ".Length);
				return new SerialCommunicationHandler(comPort);
			}
			else if (device.StartsWith("WiFi : "))
			{
				var parts = device.Split("()".ToCharArray());
				string ipAddress = parts[1];
				return new TcpCommunicationHandler(ipAddress);
			}

			return null;
		}

	}
}
