using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;

namespace netsdk
{
    public class LyncLauncher
    {
        static string _programFiles;
        static List<string> _installPaths;

        static LyncLauncher()
        {
            _programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);

            _installPaths = new List<string> {
                @"Microsoft Office\root\Office16\lync.exe",
                @"Microsoft Office\Office16\lync.exe",
                @"Microsoft Office\root\Office15\lync.exe",
                @"Microsoft Office\Office15\lync.exe"
            };
        }

        public static void StartClient()
        {
            try
            {
                var paths = _installPaths.Select(x => Path.Combine(_programFiles, x));
                var client = paths.First(File.Exists);
                Process.Start(client);
            }
            catch (InvalidOperationException)
            {
                throw new Exception("No Lync or Skype client installed");
            }
        }
    }
}
