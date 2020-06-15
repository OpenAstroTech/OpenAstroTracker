using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace OATControl.ViewModels
{
	public class ViewModelBase : INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		public void OnPropertyChanged(string propertyName)
		{
			var handler = PropertyChanged;
			if (handler != null)
			{
				handler(this, new PropertyChangedEventArgs(propertyName));
			}
		}

		protected void SetPropertyValue<T>(ref T variable, T newValue, [CallerMemberName] string memberName = "")
		{
			if (!variable.Equals(newValue))
			{
				variable = newValue;
				OnPropertyChanged(memberName);
			}
		}

		protected void SetPropertyValue<T>(ref T variable, T newValue, Action<T, T> onChangedcallBack, [CallerMemberName] string memberName = "")
		{
			if (!variable.Equals(newValue))
			{
				var oldValue = variable;
				variable = newValue;
				onChangedcallBack(oldValue, variable);
				OnPropertyChanged(memberName);
			}
		}
	}

}
