using System;
using System.Linq;
using System.Text;
using System.Collections.Generic;

using UnityEngine;

using RajceV2Cheat.Structs;
using System.Runtime.InteropServices;

namespace RajceV2Cheat
{
    internal static class Keybinds
    {
        private static List<GCHandle> keybinds = new List<GCHandle>();

        private static bool GetPressedKey(out KeyCode keycode)
        {
            keycode = KeyCode.None;

            foreach (KeyCode kc in Enum.GetValues(typeof(KeyCode)))
                if (Input.GetKeyUp(kc)) // The key must actually be pressed
                {
                    keycode = kc;
                    return true;
                }

            return false;
        }

        public unsafe static void OnUpdate()
        {
            for (int i = 0; i < keybinds.Count; i++)
            {
                Keybind* key = (Keybind*)keybinds[i].AddrOfPinnedObject();
                if (key->Rebinding)
                {
                    KeyCode k;
                    if (!GetPressedKey(out k))
                        continue;

                    key->Keycode = (int)k;
                    key->Rebinding = false;
                }
                if (key->Keycode == 0)
                {
                    key->State = false;
                    continue;
                }

                switch (key->Type)
                {
                    case KeybindType.Hold:
                        key->State = Input.GetKey((KeyCode)key->Keycode);
                        break;
                    case KeybindType.Toggle:
                        if (Input.GetKeyDown((KeyCode)key->Keycode))
                            key->State = !key->State;
                        break;
                    case KeybindType.Always:
                        key->State = true;
                        break;
                }
            }
        }

        // The name is what will be shown in the keybind list
        public unsafe static IntPtr CreateKey(string name, KeyCode keycode, KeybindType type, bool initVal = false, bool showInList = true)
        {
            Keybind kb = new Keybind();
            kb.Keycode = (int)keycode;
            kb.Type = type;
            kb.ShowInList = showInList;
            kb.State = initVal;

            GCHandle h = GCHandle.Alloc(kb, GCHandleType.Pinned);
            IntPtr ptr = h.AddrOfPinnedObject();

            FrontendPipe.RegisterKeybind(name, ptr); // Register the keybind on the frontend
            keybinds.Add(h); // Add the keybind to our backend list

            return ptr;
        }

        public static void DestroyKeybinds()
        {
            foreach (GCHandle h in keybinds)
                h.Free();

            keybinds.Clear();
        }
    }
}
