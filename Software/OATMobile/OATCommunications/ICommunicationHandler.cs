using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using OATCommunications.CommunicationHandlers;

namespace OATCommunications
{
    public interface ICommunicationHandler {
        Task<CommandResponse> SendBlind(string command);
        Task<CommandResponse> SendCommand(string command);
        bool Connected { get; }
    }
}
