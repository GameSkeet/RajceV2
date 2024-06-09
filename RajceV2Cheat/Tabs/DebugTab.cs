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

        private static void DoMainSection()
        {
            FrontendPipe.BeginContentBox("Debug");
            FrontendPipe.EndContentBox();
        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
        }
    }
}
