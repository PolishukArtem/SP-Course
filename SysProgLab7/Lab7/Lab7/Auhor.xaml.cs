using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Lab7
{
    public sealed partial class Auhor : Page
    {
        public Auhor() =>
            this.InitializeComponent();

        private void BackToMainPage_btn_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.GoBack();
        }
    }
}
