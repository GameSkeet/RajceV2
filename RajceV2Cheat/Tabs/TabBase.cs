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

        protected static int[] CreateComboArray(int len)
        {
            int[] arr = new int[len];
            for (int i = 0; i < len; i++)
                arr[i] = -1;

            return arr;
        }
        protected unsafe static T* ArrayToPtr<T>(Array arr)
        {
            return (T*)Marshal.UnsafeAddrOfPinnedArrayElement(arr, 0).ToPointer();
        }

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
