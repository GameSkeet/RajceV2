using System;
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
        private static Dictionary<string, uint> tabToId = new Dictionary<string, uint>();
        private static Texture2D icon = null;

        public static IntPtr MenuKey { get; private set; } = IntPtr.Zero;

        private static void AddTab(TabBase tab)
        {
            tabToId[tab.Name] = FrontendPipe.BeginTab(tab.Name);
            {

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

            AddTab(new CombatTab());

            byte[] fontData = Utils.GetResource("segoeuib.ttf");
            byte[] fontData1 = Utils.GetResource("seguibl.ttf");
            IntPtr fontNormal = Marshal.UnsafeAddrOfPinnedArrayElement(fontData, 0);
            IntPtr fontBold = Marshal.UnsafeAddrOfPinnedArrayElement(fontData1, 0);

            FrontendPipe.LoadFont(UIFonts.Text, fontNormal, fontData.Length);
            FrontendPipe.LoadFont(UIFonts.BiggerText, fontNormal, fontData.Length);
            FrontendPipe.LoadFont(UIFonts.Header, fontBold, fontData1.Length);

            FrontendPipe.SetMenuIcon(icon.GetNativeTexturePtr());
            // Nothing will render until the key is set
            FrontendPipe.SetMenuKeybind(MenuKey);
        }

        public static void Destroy()
        {
            foreach (KeyValuePair<string, uint> kvp in tabToId)
                FrontendPipe.RemoveTab(kvp.Value);

            tabToId.Clear();
        }
    }
}
