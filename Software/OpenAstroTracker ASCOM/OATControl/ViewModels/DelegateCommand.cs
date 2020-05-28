using System;
using System.Windows.Input;

namespace OATControl.ViewModels
{ 

    public class DelegateCommand : ICommand
    {
        Action<object> commandAction;
        Func<object, bool> commandEnabledAction;

        public DelegateCommand(Action<object> command)
        {
            this.commandAction = command;
            this.commandEnabledAction = _ => true;
        }

        public DelegateCommand(Action<object> command, Func<bool> commandEnabled)
        {
            this.commandAction = command;
            this.commandEnabledAction = _ => commandEnabled();
        }

        public DelegateCommand(Action command)
            : this(_ => command())
        {
        }

        public DelegateCommand(Action command, Func<bool> commandEnabled)
            : this(_ => command(), _ => commandEnabled())
        {
        }

        public DelegateCommand(Action<object> command, Func<object, bool> commandEnabled)
        {
            this.commandAction = command;
            this.commandEnabledAction = commandEnabled;
        }

        public bool CanExecute(object parameter)
        {
            return commandEnabledAction(parameter);
        }

        public event EventHandler CanExecuteChanged;

        public void Execute(object parameter)
        {
            commandAction(parameter);
        }

        internal void Requery()
        {
            if (this.CanExecuteChanged != null)
            {
                this.CanExecuteChanged(this, EventArgs.Empty);
            }
        }
    }
}
