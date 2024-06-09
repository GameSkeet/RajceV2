using System;
using System.Runtime.InteropServices;

namespace RajceV2Cheat.Structs
{
    internal enum KeybindType : byte
    {
        Hold,
        Toggle,
        Always
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 1)]
    internal struct Keybind
    {
        [MarshalAs(UnmanagedType.I4)]
        public int Keycode;
        [MarshalAs(UnmanagedType.I1)]
        public KeybindType Type;
        [MarshalAs(UnmanagedType.Bool)]
        public bool ShowInList;
        [MarshalAs(UnmanagedType.Bool)]
        public bool State;
        [MarshalAs(UnmanagedType.Bool)]
        public bool Rebinding;

        // This should only be set by the frontend
        [MarshalAs(UnmanagedType.U4)]
        public readonly uint ID;
    }
}
