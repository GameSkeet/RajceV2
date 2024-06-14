using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine.Networking;

namespace RajceV2Cheat.Tabs
{
    internal class MiscTab : TabBase
    {
        public override string Name { get; protected set; } = "Misc";

        private static unsafe void DoMainSection()
        {
            FrontendPipe.BeginContentBox("General");
            FrontendPipe.AddCheckbox("Show keybinds", FrontendPipe.GetKeybindListPtr());
            FrontendPipe.AddKeybind("Menu key", Menu.MenuKey);
            FrontendPipe.EndContentBox();
        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
        }
    }
}
