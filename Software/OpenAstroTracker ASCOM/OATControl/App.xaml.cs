using MahApps.Metro;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;

namespace OATControl
{
	/// <summary>
	/// Interaction logic for App.xaml
	/// </summary>

	public partial class App : Application
	{
        protected override void OnStartup(StartupEventArgs e)
        {
            ThemeManager.AddAccent("RedAccent", new Uri("pack://application:,,,/OATControl;component/Resources/RedAccent.xaml"));
            ThemeManager.AddAppTheme("RedTheme", new Uri("pack://application:,,,/OATControl;component/Resources/RedTheme.xaml"));
            ThemeManager.AddAccent("RedControls", new Uri("pack://application:,,,/OATControl;component/Resources/RedControls.xaml"));

            // get the current app style (theme and accent) from the application
            // you can then use the current theme and custom accent instead set a new theme
            Tuple<AppTheme, Accent> appStyle = ThemeManager.DetectAppStyle(Application.Current);

            // now set the Green accent and dark theme
            ThemeManager.ChangeAppStyle(Application.Current,
                                        ThemeManager.GetAccent("RedAccent"),
                                        ThemeManager.GetAppTheme("RedTheme")); 

            base.OnStartup(e);
        }
    }
}
