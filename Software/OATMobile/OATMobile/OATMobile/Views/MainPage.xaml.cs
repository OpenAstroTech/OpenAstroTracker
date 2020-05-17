using System.ComponentModel;
using System.Reflection;
using Xamarin.Forms;

namespace OATMobile.Views
{
    // Learn more about making custom code visible in the Xamarin.Forms previewer
    // by visiting https://aka.ms/xamarinforms-previewer
    [DesignTimeVisible(false)]
    public partial class MainPage : TabbedPage
    {
        public MainPage()
        {
            InitializeComponent();
            //DebugImages();
        }

        private void DebugImages() {
            // ...
            // NOTE: use for debugging, not in released app code!
            var assembly = typeof(MountControlView).GetTypeInfo().Assembly;
            foreach (var res in assembly.GetManifestResourceNames())
            {
                System.Diagnostics.Debug.WriteLine("found resource: " + res);
            }
        }

    }
}