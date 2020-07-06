using Livesim.Recording;
using System;
using System.Diagnostics;
using System.IO;

namespace RecordingRunner
{
    class Program
    {
        const string RTI_PATH = @"C:\Program Files\rti_connext_dds-6.0.1";
        private static string USR_STORAGE = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), @"livesim_recording\cdr_recording");

        static bool _recordingState;
        static bool _replayState;
        static Recorder _recordService;
        static Recorder _replayService;

        static void Main(string[] args)
        {
            while (true)
            {
                Console.Clear();
                Console.WriteLine("Recorder Example");
                Console.WriteLine($"(R) {(_recordingState ? "Stop" : "Start")} Recording, (P) {(_replayState ? "Stop" : "Start")} Replay, (L) Replay via Batch, (C) Clear Database, (Q) Quit");
                var select = Console.ReadKey();
                if (select.Key == ConsoleKey.R)
                {
                    if (_recordingState && _recordService != null)
                    {
                        try
                        {
                            _recordService.Stop();
                            _recordService.Dispose();
                            _recordingState = false;
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine("Exception occurred while stop recording....");
                            Console.WriteLine(ex.ToString());
                            Console.ReadKey();
                        }
                    }
                    else
                    {
                        try
                        {
                            _recordService = new Recorder(RecorderModes.RECORD, 0, "recorder_config.xml", "Recording_Service", USR_STORAGE);
                            _recordService.Start();
                            _recordingState = true;
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine("Exception occurred while start recording....");
                            Console.WriteLine(ex.ToString());
                            Console.ReadKey();
                        }
                    }
                }
                if (select.Key == ConsoleKey.P)
                {
                    if (_replayState && _replayService != null)
                    {
                        try
                        {
                            _replayService.Stop();
                            _replayService.Dispose();
                            _replayState = false;
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine("Exception occurred while stop replay....");
                            Console.WriteLine(ex.ToString());
                            Console.ReadKey();
                        }
                    }
                    else
                    {
                        try
                        {
                            _replayService = new Recorder(RecorderModes.REPLAY, 0, "replay_config.xml", "Replay_Service", USR_STORAGE);
                            _replayService.Start();
                            _replayState = true;
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine("Exception occurred while start replay....");
                            Console.WriteLine(ex.ToString());
                            Console.ReadKey();
                        }
                    }
                }
                if (select.Key == ConsoleKey.L)
                {
                    var process = new Process();
                    var appPath = AppDomain.CurrentDomain.BaseDirectory;
                    process.StartInfo = new ProcessStartInfo(Path.Combine(RTI_PATH, @"bin\rtireplayservice.bat"), $"-cfgName Replay_Service -cfgFile {(Path.Combine(appPath, "replay_config.xml"))}");
                    process.StartInfo.WorkingDirectory = USR_STORAGE;
                    process.Start();
                }
                if (select.Key == ConsoleKey.C)
                {
                    Directory.Delete(Path.Combine(USR_STORAGE, "cdr_recording"), true);
                }
                if (select.Key == ConsoleKey.X || select.Key == ConsoleKey.Q)
                {
                    return;
                }
            }
        }
    }
}
