using System;
using System.Linq;
using System.Threading.Tasks;
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

        public OatmealTelescopeCommandHandlers(ICommunicationHandler commHandler) {
            _commHandler = commHandler;
        }

        public async Task<bool> RefreshMountState() {
            var _slewingStates = new []{"SlewToTarget", "FreeSlew", "ManualSlew"};

            var status = await SendCommand(":GX#");
            if (!status.Success) {
                return false;
            }

            var parts = status.Data.Split(',');
            MountState.IsTracking = parts[1][2] == 'T';
            MountState.IsSlewing = _slewingStates.Contains(parts[0]);
            MountState.RightAscension = GetCompactRA(parts[5]);
            MountState.Declination = GetCompactDec(parts[6]);

            return status.Success;

        }

        private double GetCompactDec(string part) {
            var d = int.Parse(part.Substring(0, 3));
            var m = int.Parse(part.Substring(3, 2));
            var s = int.Parse(part.Substring(5, 2));

            return d + m / 60.0 + s / 3600.0;
        }

        private double GetCompactRA(string part) {
            var h = int.Parse(part.Substring(0, 2));
            var m = int.Parse(part.Substring(2, 2));
            var s = int.Parse(part.Substring(4, 2));

            return h + m / 60.0 + s / 3600.0;
        }

        public async Task<TelescopePosition> GetPosition() {
            var ra = await SendCommand(":GR#");
            var dec = await SendCommand(":GD#");

            if(ra.Success && dec.Success &&
               TryParseRA(ra.Data, out double dRa) && 
               TryParseDec(dec.Data, out double dDec)) {

                MountState.RightAscension = dRa;
                MountState.Declination = dDec;
                return new TelescopePosition(dRa, dDec, Epoch.JNOW);
            }

            MountState.RightAscension = 0;
            MountState.Declination = 0;
            return TelescopePosition.Invalid;
            
        }

        private bool TryParseRA(string ra, out double dRa) {
            var parts = ra.Split(':');
            dRa = int.Parse(parts[0]) + int.Parse(parts[1]) / 60.0 + int.Parse(parts[2]) / 3600.0;
            return true;
        }

        private bool TryParseDec(string dec, out double dDec)
        {
            var parts = dec.Split('*', '\'');
            dDec = int.Parse(parts[0]) + int.Parse(parts[1]) / 60.0 + int.Parse(parts[2]) / 3600.0;
            return true;
        }

        public async Task<bool> StartMoving(string dir) {
            var status = await SendCommand($":M{dir}#");
            MountState.IsSlewing = true;
            ++_moveState;
            return status.Success;
        }

        public async Task<bool> StopMoving(string dir) {
            var status = await SendCommand($":Q{dir}#");
            --_moveState;
            if (_moveState <= 0) {
                _moveState = 0;
                MountState.IsSlewing = false;
            }
            return status.Success;
        }

        public Task<bool> Slew(TelescopePosition position) {
            throw new NotImplementedException();
        }

        public Task<bool> Sync(TelescopePosition position) {
            throw new NotImplementedException();
        }

        public async Task<bool> GoHome() {
            var status = await SendCommand(":hP#");
            return status.Success;
        }

        public async Task<bool> SetHome()
        {
            var status = await SendCommand(":hP#");
            return status.Success;
        }

        public async Task<bool> SetTracking(bool enabled) {
            var b = enabled ? 1 : 0;
            var status = await SendCommand($":MT{b}#");
            if (status.Success) {
                MountState.IsTracking = enabled;
            }
            return status.Success;
        }

        public async Task<bool> SetLocation(double lat, double lon, double altitudeInMeters, double lstInHours) {

            // Longitude

            if (lon < 0) {
                lon = 360 + lon;
            }
            int lonFront = (int)lon;
            int lonBack = (int)((lon - lonFront) * 100.0);
            var lonCmd = $":Sg{lonFront:000}*{lonBack:00}#";
            var status = await SendCommand(lonCmd);
            if (!status.Success)  return false;


            // Latitude
            var latSign = lat > 0 ? '+' : '-';
            var absLat = Math.Abs(lat);
            int latFront = (int)absLat;
            int latBack = (int)((absLat - latFront) * 100.0);
            var latCmd = $":St{latSign}{latFront:00}*{latBack:00}#";
            status = await SendCommand(latCmd);
            if (!status.Success) return false;


            // GMT Offset
            var offsetSign = DateTimeOffset.Now.Offset.TotalHours > 0 ? "+" : "-";
            var offset = Math.Abs(DateTimeOffset.Now.Offset.TotalHours);
            status = await SendCommand($":SG{offsetSign}{offset:00}#");
            if (!status.Success) return false;


            // Local Time and Date
            var n = DateTime.Now;
            status = await SendCommand($":SL{n:HH:mm:ss}#");
            if (!status.Success) return false;
            status = await SendCommand($":SC{n:MM/dd/yy}#");
            return status.Success;
        }

        private async Task<CommandResponse> SendCommand(string cmd) {
            if (!cmd.StartsWith(":")) {
                cmd = $":{cmd}";
            }

            if (!cmd.EndsWith("#")) {
                cmd += "#";
            }

            return await _commHandler.SendCommand(cmd);
        }
    }
}
