using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using OATCommunications.CommunicationHandlers;

namespace OATCommunications
{
    public interface ICommunicationHandler {
        // Send a command, no response expected
        Task<CommandResponse> SendBlind(string command);

        // Send a command, expect a '#' terminated response
        Task<CommandResponse> SendCommand(string command);

        // Send a command, expect a single digit response
        Task<CommandResponse> SendCommandConfirm(string command);

        bool Connected { get; }

        void Disconnect();
    }
}
