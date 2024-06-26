﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using UnityEngine;

using RajceV2Cheat.Tabs;
using RajceV2Cheat.Structs;

namespace RajceV2Cheat
{
    internal static class Menu
    {
        private struct TabData
        {
            public uint ID;
            public TabBase Tab;

            public TabData(uint id, TabBase t)
            {
                ID = id;
                Tab = t;
            }
        }

        private static Dictionary<string, TabData> tabToId = new Dictionary<string, TabData>();
        private static Texture2D icon = null;

        public static IntPtr MenuKey { get; private set; } = IntPtr.Zero;

        private static void AddTab(TabBase tab)
        {
            tabToId[tab.Name] = new TabData(FrontendPipe.BeginTab(tab.Name), tab);
            {
                foreach (KeyValuePair<string, TabBase.DrawSectionDelegate> kvp in tab.Sections)
                {
                    FrontendPipe.BeginSection(kvp.Key);
                    kvp.Value(); // Adds the section things
                    FrontendPipe.EndSection();
                }
            }
            FrontendPipe.EndTab();
        }

        public static void Init()
        {
            AssetBundle bundle = Utils.LoadBundle("Data.res");
            icon = bundle.LoadAsset<Texture2D>("Icon");
            if (icon == null)
            {
                RajceV2.Logger.Error("Failed to load icon!");
                return;
            }
            GameObject.DontDestroyOnLoad(icon);

            MenuKey = Keybinds.CreateKey("Menu", UnityEngine.KeyCode.Insert, KeybindType.Toggle, true);

            byte[] fontData = Utils.GetResource("segoeuib.ttf");
            byte[] fontData1 = Utils.GetResource("seguibl.ttf");
            IntPtr fontNormal = Marshal.UnsafeAddrOfPinnedArrayElement(fontData, 0);
            IntPtr fontBold = Marshal.UnsafeAddrOfPinnedArrayElement(fontData1, 0);

            FrontendPipe.LoadFont(UIFonts.Text, fontNormal, fontData.Length);
            FrontendPipe.LoadFont(UIFonts.SectionText, fontNormal, fontData.Length);
            FrontendPipe.LoadFont(UIFonts.Header, fontBold, fontData1.Length);

            AddTab(new CombatTab());
            AddTab(new VisualsTab());
            AddTab(new MovementTab());
            AddTab(new MiscTab());
            AddTab(new SettingsTab());
#if DEBUG
            AddTab(new DebugTab());
#endif

            FrontendPipe.SetMenuIcon(icon.GetNativeTexturePtr());
            FrontendPipe.SetKeycodeConverter((k) =>
            {
                return Marshal.StringToHGlobalUni(((KeyCode)k).ToString());
            });

            // Nothing will render until the key is set
            FrontendPipe.SetMenuKeybind(MenuKey);
        }

        public static void Destroy()
        {
            foreach (KeyValuePair<string, TabData> kvp in tabToId)
            {
                FrontendPipe.RemoveTab(kvp.Value.ID);
                kvp.Value.Tab.FreeHandles();
            }

            tabToId.Clear();
        }
    }
}
