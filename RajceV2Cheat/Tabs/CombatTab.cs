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

        private unsafe void DoMainSection()
        {
            FrontendPipe.BeginContentBox("Aimbot");
            FrontendPipe.EndContentBox();
        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
        }
    }
}
