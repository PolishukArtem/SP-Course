using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// Документацию по шаблону элемента "Пустая страница" см. по адресу https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x419

namespace Lab8SP
{
    public sealed partial class MainPage : Page
    {
        private ObservableCollection<FileManagerItem> fileManagerItemsCollection;
        private StorageFolder currentFolder;
        public ObservableCollection<FileManagerItem> FileManagerItems
        {
            get
            {
                return fileManagerItemsCollection;
            }
            private set
            {
                fileManagerItemsCollection = value;
            }
        }
        public StorageFolder CurrentFolder
        {
            get
            {
                return currentFolder;
            }
            private set
            {
                currentFolder = value;
            }
        }

        public MainPage()
        {
            this.InitializeComponent();
            CurrentFolder = Windows.ApplicationModel.Package.Current.InstalledLocation;
            FileManagerItems = new ObservableCollection<FileManagerItem>();
            foldersList.ItemsSource = FileManagerItems;
            FillOutFileManagerItemsCollection(CurrentFolder);
            DisableFileManageBtns();
        }

        private async void FoldersList_ItemClick(object sender, ItemClickEventArgs e)
        {
            FileManagerItem item = e.ClickedItem as FileManagerItem;
            if (item == null)
            {
                ShowMsgBox("Error!", "Choose element");
                return;
            }
            if (item.Size == "")
            {                   
                DisableFileManageBtns();
                CurrentFolder = await CurrentFolder.GetFolderAsync(item.Name);
                FillOutFileManagerItemsCollection(CurrentFolder);
            }
            else
            {
                EnableFileManageBtns();
            }
        }

        private void DisableFileManageBtns()
        {
            copyFile.IsEnabled = false;
            deleteFile.IsEnabled = false;
        }

        private void EnableFileManageBtns()
        {
            copyFile.IsEnabled = true;
            deleteFile.IsEnabled = true;
        }

        private async void goParentFolder_btn_click(object sender, RoutedEventArgs e)
        {
            StorageFolder childFolder = CurrentFolder;
            CurrentFolder = await CurrentFolder.GetParentAsync();
            if (CurrentFolder == null)
            {
                currentFolder = childFolder;
                ShowMsgBox("Ошибка!", "У вас нет доступа к родительской папке\n");
                return;
            }
            else
                FillOutFileManagerItemsCollection(CurrentFolder);
        }

        private async void ShowMsgBox(string title, string msg)
        {
            ContentDialog contentDialog = new ContentDialog()
            {
                Title = title,
                Content = msg,
                CloseButtonText = "OK"
            };
            await contentDialog.ShowAsync();
        }

        private void exit_btn_click(object sender, RoutedEventArgs e)
        {
            App.Current.Exit();
        }

        private async void FillOutFileManagerItemsCollection(StorageFolder folder)
        {
            FileManagerItems.Clear();

            IReadOnlyList<StorageFolder> allSubfolders = await folder.GetFoldersAsync();

            foreach (var subFolder in allSubfolders)
            {
                FileManagerItems.Add(new FileManagerItem()
                {
                    Name = subFolder.Name,
                    Size = "",
                    CreationDate = subFolder.DateCreated.ToString("yyyy:mm:dd hh:mm")
                });
            }

            IReadOnlyList<StorageFile> allFiles = await folder.GetFilesAsync();

            foreach (var file in allFiles)
            {
                var basicProp = await file.GetBasicPropertiesAsync();

                FileManagerItems.Add(new FileManagerItem()
                {
                    Name = file.Name,
                    Size = SizeInBytesToOptimalUnits(basicProp.Size),
                    CreationDate = file.DateCreated.ToString("yyyy:mm:dd hh:mm")
                });
            }

            currentFolderPath_tblock.Text = CurrentFolder.Path;
        }

        private string SizeInBytesToOptimalUnits(ulong sizeInBytes)
        {
            string[] unitsNames = { "B", "KB", "MB", "GB", "TB" };
            int unitIndex = 0;

            while (sizeInBytes >= 1024 && unitIndex < unitsNames.Length)
            {
                unitIndex++;
                sizeInBytes /= 1024;
            }

            return sizeInBytes.ToString() + " " + unitsNames[unitIndex];
        }

        private async void OpenFolderDialog_btn_Click(object sender, RoutedEventArgs e)
        {
            FolderPicker folderPicker = new FolderPicker();
            folderPicker.ViewMode = PickerViewMode.List;
            folderPicker.SuggestedStartLocation = PickerLocationId.ComputerFolder;
            folderPicker.FileTypeFilter.Add("*");
            StorageFolder newFolder = await folderPicker.PickSingleFolderAsync();
            if (newFolder != null)
            {
                CurrentFolder = newFolder;
                FillOutFileManagerItemsCollection(CurrentFolder);
            }
        }

        private async void DeleteFile_Click(object sender, RoutedEventArgs e)
        {
            FileManagerItem item = foldersList.SelectedItem as FileManagerItem;
            if (item != null)
            {
                StorageFile file = await CurrentFolder.GetFileAsync(item.Name);
                if (file != null)
                {
                    // delete file 
                    await file.DeleteAsync();
                    FillOutFileManagerItemsCollection(CurrentFolder);
                }
            }
        }

        private async void CopyFile_Click(object sender, RoutedEventArgs e)
        {
            FileManagerItem item = foldersList.SelectedItem as FileManagerItem;
            if (item != null)
            {
                StorageFile file = await CurrentFolder.GetFileAsync(item.Name);
                if (file != null)
                {
                    await file.CopyAsync(CurrentFolder, file.DisplayName + "(копия)" + file.FileType);
                    FillOutFileManagerItemsCollection(CurrentFolder);
                }
            }
        }

        private void FoldersList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }
    }

    public class FileManagerItem
    {
        public string Name { get; set; }
        public string Size { get; set; }
        public string CreationDate { get; set; }
    }
}
