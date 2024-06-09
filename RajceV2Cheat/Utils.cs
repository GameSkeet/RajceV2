using System;
using System.IO;
using System.Reflection;
using System.Collections.Generic;

using UnityEngine;

using RajceV2Cheat.Structs;
using System.Runtime.CompilerServices;

namespace RajceV2Cheat
{
    internal static class Utils
    {
        private static Dictionary<string, AssetBundle> Bundles = new Dictionary<string, AssetBundle>();

        public static byte[] GetResource(string name)
        {
            Assembly asm = Assembly.GetExecutingAssembly();
            foreach (string rname in asm.GetManifestResourceNames())
            {
                if (rname.EndsWith(name))
                {
                    MemoryStream ms = new MemoryStream();
                    asm.GetManifestResourceStream(rname).CopyTo(ms);

                    return ms.ToArray();
                }
            }

            return null;
        }

        // Stolen straight from the old version
        public static AssetBundle LoadBundle(string name)
        {
            if (Bundles.ContainsKey(name))
                return Bundles[name]; // Return the cached bundle

            AssetBundle bundle = AssetBundle.LoadFromMemory(GetResource(name)); // Load the bundle from the resource

            return Bundles[name] = bundle; // Chache the bundle so we dont need to load it more than once
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe Keybind* GetKeybind(IntPtr keybind) => (Keybind*)keybind.ToPointer();

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe bool GetKeybindState(IntPtr keybind) => GetKeybind(keybind)->State;

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe KeybindType GetKeybindType(IntPtr keybind) => GetKeybind(keybind)->Type;

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe KeyCode GetKeybindKey(IntPtr keybind) => (KeyCode)GetKeybind(keybind)->Keycode;
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe void SetKeybindKey(IntPtr keybind, KeyCode keycode)
        {
            Keybind* kb = GetKeybind(keybind);
            kb->Keycode = (int)keycode;
            kb->Rebinding = false;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static unsafe bool IsKeybindRebinding(IntPtr keybind) => GetKeybind(keybind)->Rebinding;
    }
}
