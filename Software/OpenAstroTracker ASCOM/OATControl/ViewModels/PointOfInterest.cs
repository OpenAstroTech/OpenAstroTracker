using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace OATControl.ViewModels
{
	public class PointOfInterest
	{
		public PointOfInterest(XElement e)
		{
			float h, m, s;
			Name = e.Attribute("Name").Value;
			var ra = e.Attribute("RA").Value.Split(":".ToCharArray());
			if ((ra.Length == 3) && float.TryParse(ra[0], out h) && float.TryParse(ra[1], out m) && float.TryParse(ra[2], out s))
			{
				RA = h + m / 60.0f + s / 3600.0f;
			}
			var dec = e.Attribute("DEC").Value.Split(":".ToCharArray());
			if ((dec.Length == 3) && float.TryParse(dec[0], out h) && float.TryParse(dec[1], out m) && float.TryParse(dec[2], out s))
			{
				DEC = h + m / 60.0f + s / 3600.0f;
			}
		}

		public PointOfInterest(string name)
		{
			Name= name;
			RA = 0;
			DEC = 90;
		}

		public string Name { get; }
		public double RA { get; }
		public double DEC { get; }
	}
}
