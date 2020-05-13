using System;
using System.Collections.Generic;
using System.Text;

namespace OATCommunications.Model
{
    public class TelescopePosition
    {
        public static TelescopePosition Invalid = new TelescopePosition(-1, 0, Epoch.JNOW);
        public double RightAscension { get; }
        public double Declination { get;  }
        public Epoch Epoch { get; }


        public TelescopePosition(double rightAscension, double declination, Epoch epoch) {
            RightAscension = rightAscension;
            Declination = declination;
            Epoch = epoch;
        }

        public bool IsValid => this != Invalid && 
                               RightAscension >= 0 && RightAscension < 24 && 
                               Declination >= -90 && Declination <= 90;
    }

    public enum Epoch {
        J2000,
        JNOW
    }
}
