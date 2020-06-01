using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace OATCommuncations.WPF
{
	public class WpfUtilities
	{
        /// <summary>
        /// Runs the given action using the given dispatcher to determine the thread to run on. 
        /// </summary>
        /// <param name="action">The action.</param>
        public static void RunOnUiThread(Action action, Dispatcher dispatcher)
        {
            // If we're on another thread, check whether the main thread dispatcher is available
            // If the dispatcher is available, do a synchronous update, else asynchronous
            if (dispatcher.CheckAccess())
            {
                // If the dispatcher is available, do a synchronous update
                action();
            }
            else
            {
                try
                {
                    // If the dispatcher is available, do an asynchronous update
                    dispatcher.Invoke(action);
                }
                catch (Exception e)
                {
                    dispatcher.BeginInvoke(action);
                }
            }
        }
    }
}
