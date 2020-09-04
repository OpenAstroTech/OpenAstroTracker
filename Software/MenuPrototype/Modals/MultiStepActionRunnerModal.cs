using System;

namespace MenuPrototype
{
	partial class Program
	{
		public class MultiStepActionRunnerModal : ActionRunnerModal
		{
			int _steps = -1;
			bool _completed = false;
			Action<ActionRunnerEventArgs> _stateHandler;

			public MultiStepActionRunnerModal(string displayname, string tag, Action<ActionRunnerEventArgs> stateHandler, MenuItem followModal = null) : base(displayname, tag, followModal)
			{
				_stateHandler = stateHandler;
			}

			public override bool isComplete() { return _completed; }

			public override void onDisplay(bool modal = false)
			{
				ActionRunnerEventArgs args = new ActionRunnerEventArgs() { Step = _steps, State = ActionRunnerEventArgs.StepState.Starting };
				if (_steps == -1)
				{
					_steps = 0;
					_stateHandler(args);
					if (args.Result == ActionRunnerEventArgs.StepStateChange.Completed)
					{
						_completed = true;
					}
				}

				args.State = ActionRunnerEventArgs.StepState.Running;
				args.Result = ActionRunnerEventArgs.StepStateChange.NoChange;
				args.Heading = getDisplayName();

				// Let the user tell us what to do
				_stateHandler(args);

				if (args.Result == ActionRunnerEventArgs.StepStateChange.Completed)
				{
					_completed = true;
				}
				else if (args.Result == ActionRunnerEventArgs.StepStateChange.Backtrack)
				{
					_steps = Math.Max(_steps - 1, 0);
				}
				else if (args.Result == ActionRunnerEventArgs.StepStateChange.Proceed)
				{
					_steps++;
				}

				if (!string.IsNullOrEmpty(args.Heading))
				{
					Console.CursorTop = 1;
					Console.WriteLine(args.Heading + "            ");
				}
				if (!string.IsNullOrEmpty(args.Display))
				{
					Console.CursorTop = 2;
					Console.WriteLine(args.Display + "            ");
				}

				base.onDisplay(modal);
			}
		}
	}
}
