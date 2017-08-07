using System;
using System.Management;

namespace netsdk.Monitors
{
    public class ProcessWatcher : IDisposable
    {
        public static ProcessWatcher OnCreate(string targetName, Action action)
        {
            return new ProcessWatcher(BuildQuery("__InstanceCreationEvent", targetName), action);
        }

        public static ProcessWatcher OnDelete(string targetName, Action action)
        {
            return new ProcessWatcher(BuildQuery("__InstanceDeletionEvent", targetName), action);
        }

        private static string BuildQuery(string source, string target)
        {
            return "SELECT TargetInstance " +
                   "FROM " + source + " " +
                   "WITHIN  10 " +
                   "WHERE TargetInstance ISA 'Win32_Process' " +
                   "AND TargetInstance.Name = '" + target + "'";
        }

        private Action _action;
        private ManagementEventWatcher _eventWatcher;
        private const string scope = @"\\.\root\CIMV2";

        private ProcessWatcher(string query, Action action)
        {
            _action = action;

            _eventWatcher = new ManagementEventWatcher(scope, query);
            _eventWatcher.EventArrived += WatcherEventArrived;
            _eventWatcher.Start();
        }

        private void WatcherEventArrived(object sender, EventArrivedEventArgs e)
        {
            _action();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (_eventWatcher != null)
                {
                    _eventWatcher.Stop();
                    _eventWatcher.EventArrived -= WatcherEventArrived;
                    _eventWatcher.Dispose();
                    _eventWatcher = null;
                }
            }
        }
    }
}
