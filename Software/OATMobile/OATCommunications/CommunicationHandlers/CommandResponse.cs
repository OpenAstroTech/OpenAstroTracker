namespace OATCommunications.CommunicationHandlers
{
    public enum ResponseType { NoResponse, DigitResponse, FullResponse };
    public class CommandResponse {
        public string Data { get; }
        public bool Success { get; }
        public string StatusMessage { get; }

        public CommandResponse(string data, bool success = true, string statusMessage = "") {
            Data = data;
            Success = success;
            StatusMessage = statusMessage;
        }
    }
}