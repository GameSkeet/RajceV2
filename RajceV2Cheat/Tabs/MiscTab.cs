using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RajceV2Cheat.Tabs
{
    internal class MiscTab : TabBase
    {
        public override string Name { get; protected set; } = "Misc";

        private static void DoMainSection()
        {

        }
        private static void DoHitmarkersSection()
        {

        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
            Sections.Add("Hitmarkers", DoHitmarkersSection);
        }
    }
}
