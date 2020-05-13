using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using OATCommunications.Model;

namespace OATCommunications.TelescopeCommandHandlers {
    public interface ITelescopeCommandHandler {
        Task<TelescopePosition> GetPosition();
        Task<bool> Slew(TelescopePosition position);
        Task<bool> Sync(TelescopePosition position);
        Task<bool> GoHome();
        Task<bool> SetTracking(bool enabled);
        Task<bool> SetLocation(double lat, double lon, double altitudeInMeters, double lstInHours);
        Task<bool> StartMoving(string dir);
        Task<bool> StopMoving(string dir);
    }
}
