using System;
using System.Collections.Generic;
using System.Net;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using OATCommunications.CommunicationHandlers;
using OATCommunications.Model;
using OATCommunications.TelescopeCommandHandlers;

namespace OATCommunications
{
    public class OatmealTelescopeCommandHandlers : ITelescopeCommandHandler
    {
        private readonly ICommunicationHandler _commHandler;

        public OatmealTelescopeCommandHandlers(ICommunicationHandler commHandler) {
            _commHandler = commHandler;
        }

        public async Task<TelescopePosition> GetPosition() {
            var ra = await SendCommand(":GR#");
            var dec = await SendCommand(":GD#");

            if(ra.Success && dec.Success && 
               double.TryParse(ra.Data, out double dRa) && 
               double.TryParse(dec.Data, out double dDec)){ 
                return new TelescopePosition(dRa, dDec, Epoch.JNOW);
            }

            return TelescopePosition.Invalid;
            
        }

        public async Task<bool> StartMoving(string dir) {
            var status = await SendCommand($":M{dir}#");
            return status.Success;
        }

        public async Task<bool> StopMoving(string dir) {
            var status = await SendCommand($":Q{dir}#");
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
