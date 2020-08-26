using System;

namespace MenuPrototype
{
	partial class Program
	{
		/////////////////////////////////////////////////////////////
		public class ActionRunnerModal : MenuItem
		{
			protected MenuItem _followModal;
			protected Func<bool> _isComplete;

			public ActionRunnerModal(string prompt, string tag, MenuItem followModal = null) : base(prompt, tag)
			{
				_isComplete = null;
				_followModal = followModal;
			}

			public ActionRunnerModal(string prompt, string tag, Func<bool> isComplete, MenuItem followModal = null) : base(prompt, tag)
			{
				_isComplete = isComplete;
				_followModal = followModal;
			}

			public virtual bool isComplete() { return true; }

			public override void onDisplay(bool modal = false)
			{
				bool complete = _isComplete == null ? isComplete() : _isComplete();
				if (complete)
				{
					if (_followModal != null)
					{
						getMainMenu().activateDialog(_followModal.getTag());
					}
					else
					{
						getMainMenu().closeDialog();
					}
				}
			}
		}
	}
}
