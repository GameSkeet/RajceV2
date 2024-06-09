using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RajceV2Cheat.Tabs
{
    internal class DebugTab : TabBase
    {
        public override string Name { get; protected set; } = "Debug";
        private static IntPtr testKeybind = IntPtr.Zero;

        private static void DoMainSection()
        {
            FrontendPipe.BeginContentBox("Debug");
            for (int i = 0; i < 50; i++)
                FrontendPipe.AddButton("Btn" + i);
            FrontendPipe.EndContentBox();

            FrontendPipe.BeginContentBox("Debug2");
            FrontendPipe.EndContentBox();
            FrontendPipe.BeginContentBox("Debug3");
            FrontendPipe.EndContentBox();
            FrontendPipe.BeginContentBox("Debug4");
            FrontendPipe.AddKeybind("Test key", testKeybind);
            FrontendPipe.EndContentBox();
        }

        protected override void AddSections()
        {
            testKeybind = Keybinds.CreateKey("Debug key", UnityEngine.KeyCode.Tab, Structs.KeybindType.Toggle);

            Sections.Add("Main", DoMainSection);
        }
    }
}
