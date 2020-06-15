using System;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using Microsoft.Win32.SafeHandles;
using OATCommunications.CommunicationHandlers;
using OATCommunications.Model;
using OATCommunications.TelescopeCommandHandlers;

namespace OATCommunications
{
	public class OatmealTelescopeCommandHandlers : ITelescopeCommandHandler
	{
		private readonly ICommunicationHandler _commHandler;

		private int _moveState = 0;

		public MountState MountState { get; } = new MountState();

		public OatmealTelescopeCommandHandlers(ICommunicationHandler commHandler)
		{
			_commHandler = commHandler;
		}

		public bool Connected { get { return _commHandler.Connected; } }

		public async Task<bool> RefreshMountState()
		{
			var _slewingStates = new[] { "SlewToTarget", "FreeSlew", "ManualSlew" };

			var status = await SendCommand(":GX#,#");
			if (!status.Success)
			{
				return false;
			}

			var parts = status.Data.Split(',');
			MountState.IsTracking = parts[1][2] == 'T';
			MountState.IsSlewing = _slewingStates.Contains(parts[0]);
			MountState.RightAscension = GetCompactRA(parts[5]);
			MountState.Declination = GetCompactDec(parts[6]);

			return status.Success;

		}

		private double GetCompactDec(string part)
		{
			var d = int.Parse(part.Substring(0, 3));
			var m = int.Parse(part.Substring(3, 2));
			var s = int.Parse(part.Substring(5, 2));

			return d + m / 60.0 + s / 3600.0;
		}

		private double GetCompactRA(string part)
		{
			var h = int.Parse(part.Substring(0, 2));
			var m = int.Parse(part.Substring(2, 2));
			var s = int.Parse(part.Substring(4, 2));

			return h + m / 60.0 + s / 3600.0;
		}

		public async Task<TelescopePosition> GetPosition()
		{
			var ra = await SendCommand(":GR#,#");
			var dec = await SendCommand(":GD#,#");

			if (ra.Success && dec.Success &&
			   TryParseRA(ra.Data, out double dRa) &&
			   TryParseDec(dec.Data, out double dDec))
			{

				MountState.RightAscension = dRa;
				MountState.Declination = dDec;
				return new TelescopePosition(dRa, dDec, Epoch.JNOW);
			}

			MountState.RightAscension = 0;
			MountState.Declination = 0;
			return TelescopePosition.Invalid;
		}

		public async Task<float> GetSiteLatitude()
		{
			var lat = await SendCommand(":Gt#,#");

			if (lat.Success && TryParseDec(lat.Data, out double dec))
			{
				return (float)dec;
			}

			return 0;
		}

		public async Task<float> GetSiteLongitude()
		{
			var lon = await SendCommand(":Gg#,#");

			if (lon.Success && TryParseDec(lon.Data, out double dec))
			{
				if (dec > 180)
				{
					dec -= 360;
				}
				return (float)dec;
			}

			return 0;
		}

		public async Task<string> SetSiteLatitude(float latitude)
		{
			char sgn = latitude < 0 ? '-' : '+';
			int latInt = (int)Math.Abs(latitude);
			int latMin = (int)((Math.Abs(latitude) - latInt) * 60.0f);
			var lat = await SendCommand($":St{sgn}{latInt:00}*{latMin:00}#,n");
			if (lat.Success)
			{
				return lat.Data;
			}

			return "0";
		}

		public async Task<string> SetSiteLongitude(float longitude)
		{
			longitude = longitude < 0 ? longitude + 360 : longitude;
			int lonInt = (int)longitude;
			int lonMin = (int)((longitude - lonInt) * 60.0f);
			var lon = await SendCommand($":Sg{lonInt:000}*{lonMin:00}#,n");
			if (lon.Success)
			{
				return lon.Data;
			}

			return "0";
		}

		private void FloatToHMS(double val, out int h, out int m, out int s)
		{
			h = (int)Math.Floor(val);
			val = (val - h) * 60;
			m = (int)Math.Floor(val);
			val = (val - m) * 60;
			s = (int)Math.Round(val);
		}

		private bool TryParseRA(string ra, out double dRa)
		{
			var parts = ra.Split(':');
			dRa = int.Parse(parts[0]) + int.Parse(parts[1]) / 60.0 + int.Parse(parts[2]) / 3600.0;
			return true;
		}

		private bool TryParseDec(string dec, out double dDec)
		{
			var parts = dec.Split('*', '\'');
			dDec = int.Parse(parts[0]) + int.Parse(parts[1]) / 60.0;
			if (parts.Length > 2)
			{
				dDec += int.Parse(parts[2]) / 3600.0;
			}
			
			return true;
		}

		public async Task<bool> StartMoving(string dir)
		{
			var status = await SendCommand($":M{dir}#");
			MountState.IsSlewing = true;
			++_moveState;
			return status.Success;
		}

		public async Task<bool> StopMoving(string dir)
		{
			var status = await SendCommand($":Q{dir}#");
			--_moveState;
			if (_moveState <= 0)
			{
				_moveState = 0;
				MountState.IsSlewing = false;
			}
			return status.Success;
		}

		public async Task<bool> Slew(TelescopePosition position)
		{
			int deg, hour, min, sec;

			FloatToHMS(Math.Abs(position.Declination), out deg, out min, out sec);
			string sign = position.Declination < 0 ? "-" : "+";
			var result = await SendCommand(string.Format(":Sd{0}{1:00}*{2:00}:{3:00}#,n", sign, deg, min, sec));
			if (!result.Success || result.Data != "1") return false;
			FloatToHMS(Math.Abs(position.RightAscension), out hour, out min, out sec);
			result = await SendCommand(string.Format(":Sr{0:00}:{1:00}:{2:00}#,n", hour, min, sec));
			if (!result.Success || result.Data != "1") return false;
			result = await SendCommand($":MS#");
			return result.Success;
		}

		public async Task<bool> Sync(TelescopePosition position)
		{
			int deg, hour, min, sec;

			FloatToHMS(Math.Abs(position.Declination), out deg, out min, out sec);
			string sign = position.Declination < 0 ? "-" : "+";
			var result = await SendCommand(string.Format(":Sd{0}{1:00}*{2:00}:{3:00}#,n", sign, deg, min, sec));
			if (!result.Success || result.Data != "1") return false;
			FloatToHMS(Math.Abs(position.RightAscension), out hour, out min, out sec);
			result = await SendCommand(string.Format(":Sr{0:00}:{1:00}:{2:00}#,n", hour, min, sec));
			if (!result.Success || result.Data != "1") return false;
			result = await SendCommand($":CM#");
			return result.Success;
		}

		public async Task<bool> GoHome()
		{
			var status = await SendCommand(":hP#");
			return status.Success;
		}

		public async Task<bool> SetHome()
		{
			var status = await SendCommand(":hP#");
			return status.Success;
		}

		public async Task<bool> SetTracking(bool enabled)
		{
			var b = enabled ? 1 : 0;
			var status = await SendCommand($":MT{b}#,n");
			if (status.Success)
			{
				MountState.IsTracking = enabled;
			}
			return status.Success;
		}

		public async Task<bool> SetLocation(double lat, double lon, double altitudeInMeters, double lstInHours)
		{

			// Longitude

			if (lon < 0)
			{
				lon = 360 + lon;
			}
			int lonFront = (int)lon;
			int lonBack = (int)((lon - lonFront) * 60);
			var lonCmd = $":Sg{lonFront:000}*{lonBack:00}#,n";
			var status = await SendCommand(lonCmd);
			if (!status.Success) return false;


			// Latitude
			var latSign = lat > 0 ? '+' : '-';
			var absLat = Math.Abs(lat);
			int latFront = (int)absLat;
			int latBack = (int)((absLat - latFront) * 60.0);
			var latCmd = $":St{latSign}{latFront:00}*{latBack:00}#,n";
			status = await SendCommand(latCmd);
			if (!status.Success) return false;


			// GMT Offset
			var offsetSign = DateTimeOffset.Now.Offset.TotalHours > 0 ? "+" : "-";
			var offset = Math.Abs(DateTimeOffset.Now.Offset.TotalHours);
			status = await SendCommand($":SG{offsetSign}{offset:00}#,n");
			if (!status.Success) return false;


			// Local Time and Date
			var n = DateTime.Now;
			status = await SendCommand($":SL{n:HH:mm:ss}#,n");
			if (!status.Success) return false;
			status = await SendCommand($":SC{n:MM/dd/yy}#,#");
			return status.Success;
		}

		public async Task<CommandResponse> SendCommand(string cmd)
		{
			if (!cmd.StartsWith(":"))
			{
				cmd = $":{cmd}";
			}

			if (cmd.EndsWith("#,#"))
			{
				return await _commHandler.SendCommand(cmd.Substring(0, cmd.Length - 2));
			}
			else if (cmd.EndsWith("#,n"))
			{
				return await _commHandler.SendCommandConfirm(cmd.Substring(0, cmd.Length - 2));
			}

			if (!cmd.EndsWith("#"))
			{
				cmd += "#";
			}
			return await _commHandler.SendBlind(cmd);
		}
	}
}
