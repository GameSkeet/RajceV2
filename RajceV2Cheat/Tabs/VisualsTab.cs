using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RajceV2Cheat.Tabs
{
    internal class VisualsTab : TabBase
    {
        public override string Name { get; protected set; } = "Visuals";

        private static void DoMainSection()
        {

        }
        private static void DoESPSection()
        {

        }
        private static void DoChamsSection()
        {

        }
        private static void DoWorldSection()
        {

        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
            Sections.Add("ESP", DoESPSection);
            Sections.Add("Chams", DoChamsSection);
            Sections.Add("World", DoWorldSection);
        }
    }
}
