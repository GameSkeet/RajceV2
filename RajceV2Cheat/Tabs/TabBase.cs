using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace RajceV2Cheat.Tabs
{
    internal abstract class TabBase
    {
        public delegate void DrawSectionDelegate();

        protected static TabBase Instance = null;
        public abstract string Name { get; protected set; }
        protected Dictionary<string, GCHandle> Handles = new Dictionary<string, GCHandle>();
        public Dictionary<string, DrawSectionDelegate> Sections { get; protected set; } = new Dictionary<string, DrawSectionDelegate>();

        protected abstract void AddSections();

        protected TabBase()
        {
            Instance = this;
            AddSections();
        }

        public GCHandle GetHandle(string name)
        {
            if (Handles.TryGetValue(name, out GCHandle handle))
                return handle;

            return default;
        }
        public IntPtr GetHandleAddr(string name)
        {
            if (Handles.TryGetValue(name, out GCHandle handle))
                return handle.AddrOfPinnedObject();

            return IntPtr.Zero;
        }

        public void FreeHandles()
        {
            foreach (KeyValuePair<string, GCHandle> kvp in Handles)
                kvp.Value.Free();

            Handles.Clear();
        }
    }
}
