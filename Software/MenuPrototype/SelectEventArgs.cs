namespace MenuPrototype
{
	partial class Program
	{
		////////////////////////////////////////////////////////////////////
		public class SelectEventArgs : EventArgs
		{
			string _selected;
			public SelectEventArgs(MenuItem source) : base(source) { }
			public SelectEventArgs(MenuItem source, string selected) : base(source) { _selected = selected; }
			public string getSelected() { return _selected; }
		}
	}
}
