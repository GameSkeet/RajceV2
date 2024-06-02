using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace RajceV2Cheat.Tabs
{
    internal class CombatTab : TabBase
    {
        public override string Name { get; protected set; } = "Combat";
        public bool InnerBool = false;

        private unsafe void DoMainSection()
        {
            IntPtr inner = GetHandleAddr("Test");

            FrontendPipe.BeginContentBox("Aimbot");
            FrontendPipe.AddButton("Kokot", () =>
            {
                RajceV2.Logger.Warning("Called from frontend!");
            });
            fixed(bool* ptr = &InnerBool)
                FrontendPipe.AddCheckbox("Test checkbox", ptr, (val) =>
                {
                    RajceV2.Logger.Warning("New value: {0}", val);
                });
            FrontendPipe.EndContentBox();

            FrontendPipe.BeginContentBox("Misc");
            FrontendPipe.EndContentBox();
        }

        protected override void AddSections()
        {
            Handles.Add("Test", GCHandle.Alloc(InnerBool, GCHandleType.Pinned));

            Sections.Add("Main", DoMainSection);
        }
    }
}
