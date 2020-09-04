namespace MenuPrototype
{
	partial class Program
	{
		public class ActionRunnerEventArgs
		{
			public enum StepState { Starting, Running, Ending };
			public enum StepStateChange { NoChange, Backtrack, Proceed, Completed };

			public int Step { get; set; }
			public StepState State { get; set; }
			public StepStateChange Result { get; set; }
			public string Heading { get; set; }
			public string Display { get; set; }
		}
	}
}
