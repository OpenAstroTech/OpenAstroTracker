using System;

namespace MenuPrototype
{
	partial class Program
	{
		public class MultiStepActionRunnerModal : ActionRunnerModal
		{
			int _steps = -1;
			bool _completed = false;
			Action<StepStateChangeArgs> _transition;

			public MultiStepActionRunnerModal(string prompt, string tag, Action<StepStateChangeArgs> transition, MenuItem followModal = null) : base(prompt, tag, followModal)
			{
				_transition = transition;
			}

			public override bool isComplete() { return _completed; }

			public override void onDisplay(bool modal = false)
			{
				StepStateChangeArgs args = new StepStateChangeArgs() { Step = _steps, State = StepStateChangeArgs.StepState.Starting };
				if (_steps == -1)
				{
					_steps = 0;
					_transition(args);
					if (args.Result == StepStateChangeArgs.StepStateChange.Completed)
					{
						_completed = true;
					}
				}

				args.State = StepStateChangeArgs.StepState.Running;
				args.Result = StepStateChangeArgs.StepStateChange.NoChange;
				_transition(args);
				if (args.Result == StepStateChangeArgs.StepStateChange.Completed)
				{
					_completed = true;
				}
				else if (args.Result == StepStateChangeArgs.StepStateChange.Backtrack)
				{
					_steps = Math.Max(_steps - 1, 0);
				}
				else if (args.Result == StepStateChangeArgs.StepStateChange.Proceed)
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
