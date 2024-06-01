using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RajceV2Cheat.Tabs
{
    internal class SettingsTab : TabBase
    {
        public override string Name { get; protected set; } = "Settings";

        private static void DoConfigsSection()
        {

        }
        private static void DoMenuStyleSection()
        {

        }
        private static void DoLuaSection()
        {

        }

        protected override void AddSections()
        {
            Sections.Add("Configs", DoConfigsSection);
            Sections.Add("Menu styles", DoMenuStyleSection);
            Sections.Add("Lua??", DoLuaSection);
        }
    }
}
