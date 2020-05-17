using System.ComponentModel;

namespace OATCommunications.Model {
    public class MountState : ObservableClass, IReadOnlyMountState{
        public bool IsTracking { get; set; }
        public bool IsSlewing { get; set; }
        public bool IsSlewingRA { get; set; }
        public bool IsSlewingDec { get; set; }
        public double RightAscension { get; set; }
        public double Declination { get; set; }
    }

    public interface IReadOnlyMountState : INotifyPropertyChanged {
        bool IsTracking { get; }
        bool IsSlewing { get; }
        bool IsSlewingRA { get; }
        bool IsSlewingDec { get; }
        double RightAscension { get; }
        double Declination { get; }


    }
}
