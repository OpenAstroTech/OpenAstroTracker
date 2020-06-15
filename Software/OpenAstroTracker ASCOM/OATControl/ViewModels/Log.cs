using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace OATControl.ViewModels
{
	public class Log
	{
		private static DateTime appStartTime = DateTime.UtcNow;
		private static object oLock = new object();
		private static string sPath = string.Format("{0}\\oat_{1}-{2}.log", Environment.GetFolderPath(Environment.SpecialFolder.Personal), DateTime.Now.ToString("yyyy-MM-dd_HH-mm-ss"), Environment.UserName);

		private static List<string> lstBuffer = new List<string>();
		private static DateTime dtLastUpdate = DateTime.Now.AddSeconds(5.0);
		private static int maxBuffered = 0;

		public static string Filename
		{
			get
			{
				return Log.sPath;
			}
		}

		public static void Init(string sTitle)
		{
			Log.WriteLine("*********************************");
			Log.WriteLine(string.Format("*  {0} *", sTitle.PadRight(28)));
			Log.WriteLine("*********************************");
			Log.WriteLine("* Started : " + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + " *");
			Log.WriteLine(string.Format("* User    : {0} *", Environment.UserName.PadRight(19)));
			Log.WriteLine("*********************************");
		}

		private static string FormatMessage(string message, object[] args)
		{
			var sb = new StringBuilder(message.Length + 64);

			TimeSpan elapsed = DateTime.UtcNow - Log.appStartTime;
			sb.AppendFormat("[{0}] [{1}]: ", elapsed.ToString("hh\\:mm\\:ss\\.ffff"), Thread.CurrentThread.ManagedThreadId);

			if (args != null && args.Length > 0)
			{
				sb.AppendFormat(message, args);
			}
			else
			{
				sb.Append(message);
			}

			return sb.ToString();
		}

		public static void WriteLine(string message, params object[] args)
		{
			if ((DateTime.UtcNow - Log.dtLastUpdate).TotalMilliseconds > 1000.0)
			{
				lock (Log.oLock)
				{
					File.AppendAllText(Log.sPath, string.Join("\r\n", Log.lstBuffer.ToArray()) + "\r\n");
					Log.lstBuffer.Clear();
				}
				Log.dtLastUpdate = DateTime.UtcNow;
				return;
			}

			string sLine = FormatMessage(message, args);

			lock (Log.oLock)
			{
				Log.lstBuffer.Add(sLine);
				if (Log.lstBuffer.Count > Log.maxBuffered)
				{
					Log.maxBuffered = Log.lstBuffer.Count;
				}
			}
		}

		public static void Quit()
		{
		}
	}
}


