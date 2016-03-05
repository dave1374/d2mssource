using D2MSCommon.DataStructures;
using D2MSCommon;
using Newtonsoft.Json;
using System;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Navigation;
using System.Collections.Generic;
using System.Windows.Data;
using System.Collections;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Xml.Linq;

namespace Injector2
{
  
    public partial class MainWindow : Window
    {
        const string VERSION = "beta 1.0";
        int runCount = 0;
        int totalSecondsInGame = 0;

        //heh.
        SolidColorBrush brown   = new SolidColorBrush(Colors.Brown);
        SolidColorBrush yellow  = new SolidColorBrush(Colors.DarkGoldenrod);
        SolidColorBrush green   = new SolidColorBrush(Colors.Green);
        SolidColorBrush black   = new SolidColorBrush(Colors.Black);
        SolidColorBrush blue    = new SolidColorBrush(Colors.Blue);


        List<GenericItemDisplay> genericItems;
        List<LevelDisplay> levels;
        ObservableCollection<TimerDisplay> timers;
        
        // privileges
        const int PROCESS_CREATE_THREAD     = 0x0002;
        const int PROCESS_QUERY_INFORMATION = 0x0400;
        const int PROCESS_VM_OPERATION      = 0x0008;
        const int PROCESS_VM_WRITE          = 0x0020;
        const int PROCESS_VM_READ           = 0x0010;

        // used for memory allocation
        const uint MEM_COMMIT       = 0x00001000;
        const uint MEM_RESERVE      = 0x00002000;
        const uint PAGE_READWRITE   = 4;

        [DllImport("kernel32.dll")]
        public static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
        static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress,
            uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out UIntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll")]
        static extern IntPtr CreateRemoteThread(IntPtr hProcess,
            IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);


        [DllImport("user32.dll", SetLastError = true)]
        static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
        [DllImport("user32.dll", SetLastError = true)]
        static extern IntPtr FindWindow(string lpClassName, string lpWindowName);


        [DllImport("kernel32.dll", SetLastError = true)]
        static extern UInt32 WaitForSingleObject(IntPtr hHandle, UInt32 dwMilliseconds);
        

        NamedPipeClientStream pipeClient;
        Thread startNamedPipeThread;
        
        public MainWindow()
        {
            InitializeComponent();
        }
        
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);

        [DllImportAttribute("user32.dll")]
        public static extern bool ReleaseCapture();


        //Attach this to the MouseDown event of your drag control to move the window in place of the title bar
        private void WindowDrag(object sender, MouseButtonEventArgs e) // MouseDown
        {
            ReleaseCapture();
            SendMessage(new WindowInteropHelper(this).Handle,
                0xA1, (IntPtr)0x2, (IntPtr)0);
        }

        //Attach this to the PreviewMousLeftButtonDown event of the grip control in the lower right corner of the form to resize the window
        private void WindowResize(object sender, MouseButtonEventArgs e)
        {
            HwndSource hwndSource = PresentationSource.FromVisual((Visual)sender) as HwndSource;
            SendMessage(hwndSource.Handle, 0x112, (IntPtr)61448, IntPtr.Zero);
        }


        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            Process.Start(new ProcessStartInfo(e.Uri.AbsoluteUri));
            e.Handled = true;
        }

        private void Window_Loaded_1(object sender, RoutedEventArgs e)
        {
            FillProcessComboBox();// Populate list

            FillMHComboBox();// Populate list

            HttpWebResponse httpResponse = null;
            HttpWebRequest httpWebRequest;

            httpWebRequest = (HttpWebRequest)WebRequest.Create("http://d2madstats.com/service1.svc/GetLatestVersion");

            httpWebRequest.KeepAlive = false;
            httpWebRequest.ContentType = "text/json";
            httpWebRequest.Method = "GET";



            httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();
            string latestVersion;

            using (StreamReader sr = new StreamReader(httpResponse.GetResponseStream(), System.Text.Encoding.UTF8))
            {
                latestVersion = sr.ReadToEnd();


                sr.Close();
            }

            latestVersion = latestVersion.Replace("\"", "");

            VersionLabel.Content = VERSION;

            if (latestVersion != VERSION)
            {
                MessageLabel.Content = "[" + latestVersion + "] You are not running latest version.";
                MessageLabel.Foreground = new SolidColorBrush(Colors.Yellow);
            }


            httpWebRequest = (HttpWebRequest)WebRequest.Create("http://d2madstats.com/service1.svc/GetLevels");



            httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();

            using (StreamReader sr = new StreamReader(httpResponse.GetResponseStream(), System.Text.Encoding.UTF8))
            {
                string result = sr.ReadToEnd();

                levels = JsonConvert.DeserializeObject<List<LevelDisplay>>(result);


                sr.Close();
            }


            httpWebRequest = (HttpWebRequest)WebRequest.Create("http://d2madstats.com/service1.svc/getgenericitems");


            httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();

            using (StreamReader sr = new StreamReader(httpResponse.GetResponseStream(), System.Text.Encoding.UTF8))
            {
                string result = sr.ReadToEnd();

                genericItems = JsonConvert.DeserializeObject<List<GenericItemDisplay>>(result);


                sr.Close();
            }

            //TimersListView.ItemsSource = timers;


            //CollectionView view = (CollectionView)CollectionViewSource.GetDefaultView(TimersListView.ItemsSource);
            //view.SortDescriptions.Add(new SortDescription("LastUpdated", ListSortDirection.Ascending));

            //view.CustomSort = new sorter();

            timers = new ObservableCollection<TimerDisplay>();

            TimersListView.ItemsSource = timers;

            CreateTotalTimerItem();
        }

        private void CreateTotalTimerItem()
        {
            var tmp = new TimerDisplay();
            tmp.LvlNo = 0;
            tmp.Room = "Total";
            tmp.LastUpdated = DateTime.Now;

            timers.Insert(0, tmp);
        }

        private void Window_Closing_1(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (pipeClient != null && pipeClient.IsConnected)
                pipeClient.Close();

            if (startNamedPipeThread != null && startNamedPipeThread.IsAlive)
                startNamedPipeThread.Abort();

            XElement config = new XElement("D2MS",
                              new XElement("MHVersion", MHVersionComboBox.SelectedItem)
                              //new XElement("Sleep", SleepTextBox.Text),
                            );

            config.Save("D2MSConfig.xml");

        }

        private void Label_MouseDown(object sender, MouseButtonEventArgs e)
        {
            this.Close();

        }
                
        private void FillMHComboBox()
        {
            MHVersionComboBox.Items.Clear();
            MHVersionComboBox.Text = "";

            string[] folders = System.IO.Directory.GetDirectories(Environment.CurrentDirectory + @"\Support", "*", System.IO.SearchOption.AllDirectories);

            foreach (var folder in folders)
            {

                MHVersionComboBox.Items.Add(folder.Substring(folder.LastIndexOf("\\") + 1));
            }

            if (File.Exists("D2MSConfig.xml"))
            {

                XDocument config = XDocument.Load("D2MSConfig.xml");
                var values = from x in config.Elements("D2MS").Descendants()
                             select new
                             {
                                 Value = x.Value,
                                 Name = x.Name
                             };
                
                MHVersionComboBox.SelectedItem = values.Single(v => v.Name == "MHVersion").Value;

            }
            
        }

        private void FillProcessComboBox()
        {
            ProcessesIDsComboBox.Items.Clear();
            ProcessesIDsComboBox.Text = "";

            Process[] targetProcesses = Process.GetProcessesByName("Game").OrderBy(p => p.StartTime).ToArray();

            foreach (var p in targetProcesses)
            {
                ProcessesIDsComboBox.Items.Add(p.Id);
            }


            //if there is no .cfg file in d2 directory, shit will happen.
            //look for it, and try to move it there to prevent errors.

            if (targetProcesses.Length > 0) {
                var fileName = targetProcesses[0].MainModule.FileName;

                string d2Folder = Path.GetDirectoryName(fileName);

                string cfgFile = d2Folder + @"\D2MS.cfg";

                if (!File.Exists(cfgFile)) {
                    //attempt to copy

                    File.Copy(Environment.CurrentDirectory + @"\D2MS.cfg", cfgFile);
                }

            }
        }

        private void InjectSpecificButton_Click(object sender, RoutedEventArgs e)
        {
            uint processid = uint.Parse(ProcessesIDsComboBox.Text);

            Inject(processid);

            startNamedPipeThread = new Thread(new ThreadStart(CreatePipeWrapper));
            startNamedPipeThread.Start();
        }

        private void InjectAllButton_Click(object sender, RoutedEventArgs e)
        {
            foreach (var p in Process.GetProcessesByName("Game"))
            {
                Inject((uint)p.Id);
            }

            startNamedPipeThread = new Thread(new ThreadStart(CreatePipeWrapper));
            startNamedPipeThread.Start();
        }


        public void CreatePipeWrapper() // useless ?
        {
            CreatePipe();
        }

        public void CreatePipe()
        {
            string tmpTrace = "pipe";
            using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
            using (StreamWriter sw = new StreamWriter(fs))
            {
                sw.WriteLine(tmpTrace);
            }

            using (pipeClient = new NamedPipeClientStream(".", "D2MSPipe", PipeDirection.In))
            {
                tmpTrace += "1";

                using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
                using (StreamWriter sw = new StreamWriter(fs))
                {
                    sw.WriteLine(tmpTrace);
                }

                try
                {

                    pipeClient.Connect();
                    tmpTrace += "2";

                    using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
                    using (StreamWriter sw = new StreamWriter(fs))
                    {
                        sw.WriteLine(tmpTrace);
                    }
                }
                catch (Exception ex)
                {
                    using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
                    using (StreamWriter sw = new StreamWriter(fs))
                    {
                        sw.WriteLine(ex.Message + " " + tmpTrace);
                    }
                    throw;
                }

                StreamReader reader = new StreamReader(pipeClient);

                while (true)
                {
                    Process[] targetProcesses = Process.GetProcessesByName("Game").OrderBy(p => p.StartTime).ToArray();

                    if (targetProcesses.Count() == 0)
                    {
                        pipeClient.Close();
                        return;
                    }

                    if (reader.Peek() > -1)
                    {
                        string JSONData = reader.ReadLine();
                        string JSONDataBare;

                        reader.DiscardBufferedData();

                        //string JSONData = "{\"data\":{\"AccountName\":\"\",\"CharName\":\"SilverDeth-IV\",\"GameDiff\":\"1\",\"CompressedData\":[8228138568842941382,8247906688399250381,8244242016143283142]}}";

                        JSONDataBare = JSONData.Substring(8, JSONData.Length - 9);


                        D2MSData data = JsonConvert.DeserializeObject<D2MSData>(JSONDataBare);
                        
                        runCount++;
                        int totalTimeThisRun = 0;
                        
                        foreach (var t in data.Timers)
                        {
                            var tmp = timers.FirstOrDefault(x => x.LvlNo == t.LvlNo);
                            bool addIt = false;

                            if (tmp == null) //never been in that room
                            {
                                addIt = true;

                                tmp = new TimerDisplay();
                                tmp.LvlNo = t.LvlNo;
                                tmp.Room = t.LvlNo.ToString();

                                var tmpLevel = levels.FirstOrDefault(l => l.LvlNo == t.LvlNo);

                                if (tmpLevel != null)
                                    tmp.Room = tmpLevel.LvlName;
                            }
                            
                            if (t.Time < tmp.Best)
                                tmp.Best = t.Time;
                            if (t.Time > tmp.Worst)
                                tmp.Worst = t.Time;

                            tmp.Count++;
                            totalTimeThisRun += t.Time;

                            tmp.LastUpdated = DateTime.Now;
                            tmp.Seconds += t.Time;

                            tmp.Average = (int)(tmp.Seconds / tmp.Count);

                            if (addIt)
                            {
                                this.Dispatcher.Invoke((Action)(() =>
                                {
                                    timers.Insert(1, tmp);
                                }));
                            }

                            totalSecondsInGame += t.Time;
                        }

                        timers[0].Count++;
                        timers[0].Seconds = totalTimeThisRun;
                        timers[0].LastUpdated = DateTime.Now;

                        if (totalTimeThisRun < timers[0].Best)
                            timers[0].Best = totalTimeThisRun;

                        if (totalTimeThisRun > timers[0].Worst)
                            timers[0].Worst = totalTimeThisRun;


                        timers[0].Average = (int)(totalSecondsInGame / runCount);

                        this.Dispatcher.Invoke((Action)(() =>
                        {
                            TimersListView.Items.Refresh();

                        }));
                        
                        

                        foreach (var c in data.CompressedData)
                        {
                            ItemStructure i = DataAlgorithms.DecompressValue(c.ItemValue);
                            DisplayItem displayItem = new DisplayItem();

                            displayItem.Run = runCount;
                            string itemName = i.ItemCode;

                            var genericItem = genericItems.FirstOrDefault(g => g.ItemCode == i.ItemCode);

                            if (genericItems != null)
                            {
                                displayItem.ItemName = genericItem.ItemName;
                            }

                            switch (i.ItemQuality)
                            {
                                case 7:
                                    displayItem.Color = brown;
                                    break;
                                case 4:
                                    displayItem.Color = blue;
                                    break;
                                case 5:
                                    displayItem.Color = green;
                                    break;
                                case 6:
                                    displayItem.Color = yellow;
                                    break;

                                default:
                                    displayItem.Color = black;
                                    break;
                            }

                            string room = i.LvlNo.ToString();

                            var levelName = levels.FirstOrDefault(l => l.LvlNo == i.LvlNo);

                            if (levelName != null)
                            {
                                displayItem.Room = levelName.LvlName;
                            }
                            

                            this.Dispatcher.Invoke((Action)(() =>
                            {
                                ItemsListView.Items.Insert(0, displayItem);
                            }));

                        }
                        
                        

                        HttpWebResponse httpResponse = null;
                        HttpWebRequest  httpWebRequest;

                        httpWebRequest = (HttpWebRequest)WebRequest.Create("http://d2madstats.com/service1.svc/savedata");
                      
                        httpWebRequest.KeepAlive = false;
                        httpWebRequest.ContentType = "text/json";
                        httpWebRequest.Method = "POST";
                        

                        string tmpTimeUnit = totalSecondsInGame > 60 ? totalSecondsInGame > 3600 ? " hours" : " mins." : " secs.";
                        float displayTime = totalSecondsInGame > 60 ? totalSecondsInGame > 3600 ? totalSecondsInGame / 3600.0f : totalSecondsInGame / 60.0f : totalSecondsInGame;

                        this.Dispatcher.Invoke((Action)(() =>
                        {
                            RunCounterLabel.Content = runCount;
                            PlayTimeLabel.Content = displayTime.ToString("0.0") + " " + tmpTimeUnit;
                        }));


                        try
                        {
                            


                            var streamWriter = new StreamWriter(httpWebRequest.GetRequestStream());
                            
                            streamWriter.Write(JSONData);
                            
                            streamWriter.Close();
                            
                            httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();
                            
                            using (StreamReader sr = new StreamReader(httpResponse.GetResponseStream(), System.Text.Encoding.UTF8))
                            {
                                string result = sr.ReadToEnd();
                                sr.Close();                                
                            }

                            httpResponse.Close();

                            using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                sw.WriteLine("success: " + DateTime.Now.ToString());
                            }

                        }
                        catch (Exception ex)
                        {
                            using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
                            using (StreamWriter sw = new StreamWriter(fs))
                            {
                                sw.WriteLine(DateTime.Now.ToString() + " | " + " " + ex.Message + Environment.NewLine + JSONData.ToString());
                            }


                            this.Dispatcher.Invoke((Action)(() =>
                            {
                                MessageLabel.Content = "Could not send data. Logged error";
                                MessageLabel.Foreground = new SolidColorBrush(Colors.DarkRed);
                            }));
                        }
                    }

                    System.Threading.Thread.Sleep(50);
                }
            }

        }

        private void Inject(uint processid)
        {

            string tmpTrace = "";
            try
            {
                tmpTrace += "1";
           

                // geting the handle of the process - with required privileges
                IntPtr procHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, (int)processid);

                tmpTrace += procHandle + "\n";
                // searching for the address of LoadLibraryA and storing it in a pointer
                IntPtr loadLibraryAddr = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
                tmpTrace += "3";



                string dllName = Environment.CurrentDirectory + @"\support\" + MHVersionComboBox.SelectedItem +  @"\D2MS.dll";



                using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
                using (StreamWriter sw = new StreamWriter(fs))
                {
                    sw.WriteLine(dllName);
                }

                uint nameLength = (uint)((dllName.Length + 1) * Marshal.SizeOf(typeof(char)));

                IntPtr allocMemAddress = VirtualAllocEx(procHandle, IntPtr.Zero, nameLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);


                // writing the name of the dll there
                UIntPtr bytesWritten;
                bool b = WriteProcessMemory(procHandle, allocMemAddress, Encoding.Default.GetBytes(dllName), (uint)((dllName.Length + 1) * Marshal.SizeOf(typeof(char))), out bytesWritten);

                tmpTrace += "4\n";
                tmpTrace += b.ToString();

                tmpTrace += bytesWritten.ToString();

                // creating a thread that will call LoadLibraryA with allocMemAddress as argument
                IntPtr handle = CreateRemoteThread(procHandle, IntPtr.Zero, 0, loadLibraryAddr, allocMemAddress, 0, IntPtr.Zero);

                //WaitForThreadToExit(hThread); ? 
                //const UInt32 INFINITE = 0xFFFFFFFF;
                tmpTrace += "\n5";

                //WaitForSingleObject(handle, 1000);

                tmpTrace += "6";

                using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
                using (StreamWriter sw = new StreamWriter(fs))
                {
                    sw.WriteLine(tmpTrace);
                }

            }
            catch (Exception ex)
            {
                using (FileStream fs = new FileStream(Environment.CurrentDirectory + @"\log.txt", FileMode.Append, FileAccess.Write))
                using (StreamWriter sw = new StreamWriter(fs))
                {
                    sw.WriteLine( "error" +  ex.Message + " " + tmpTrace);
                }
                throw;
            }
        }

        private void StatsButton_Click(object sender, RoutedEventArgs e)
        {
            if (StatsButton.Content == FindResource("StatsButtonImage")) {
                StatsButton.Content = FindResource("StatsButtonImageActivated");
                ItemsListViewStack.Visibility = Visibility.Visible;
                D2MSMainWindow.Height = 650;
            }
            else
            {
                StatsButton.Content = FindResource("StatsButtonImage");
                ItemsListViewStack.Visibility = Visibility.Collapsed;
                D2MSMainWindow.Height = 300;
            }
            
        }

        private void RefreshButton_Click(object sender, RoutedEventArgs e)
        {
            FillProcessComboBox();
        }

        private void MinimizeWindow(object sender, MouseButtonEventArgs e)
        {
            this.WindowState = WindowState.Minimized;
        }

        private void ResetTimerButton_Click(object sender, RoutedEventArgs e)
        {
            timers.Clear();
            CreateTotalTimerItem();

            //TimersListView.Items.Clear();
        }

        private void ResetPlayTimeButton_Click(object sender, RoutedEventArgs e)
        {
            totalSecondsInGame = 0;
            runCount = 0;
            PlayTimeLabel.Content = 0;
            RunCounterLabel.Content = 0;
        }
        
    }

    //internal class sorter : IComparer
    //{
    //    int IComparer.Compare(object a, object b)
    //    {
    //        TimerDisplay t1 = (TimerDisplay)a;
    //        TimerDisplay t2 = (TimerDisplay)b;

    //        if (t1.LastUpdated > t2.LastUpdated)
    //            return 1;
    //        if (t1.LastUpdated < t2.LastUpdated)
    //            return -1;
    //        else
    //            return 0;
    //    }
    //}

    internal class TimerDisplay
    {        
        public TimerDisplay()
        {
            Best = int.MaxValue;
            Worst = 0;
        }

        public string   Room        { get; set; }
        public int      LvlNo       { get; set; }
        public int      Seconds     { get; set; }
        public int      Count       { get; set; }
        public int      Average     { get; set; }        
        public DateTime LastUpdated { get; set; }
        public int Best { get; internal set; }
        public int Worst { get; internal set; }
    }

    internal class DisplayItem
    {
        public DisplayItem()
        {
        }

        public SolidColorBrush Color { get; set; }
        public string ItemName { get; set; }
        public int Run { get; set; }
        public string Room { get; set; }
    }
}
