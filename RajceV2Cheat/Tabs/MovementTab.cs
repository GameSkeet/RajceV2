using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RajceV2Cheat.Tabs
{
    internal class MovementTab : TabBase
    {
        public override string Name { get; protected set; } = "Movement";

        private static void DoMainSection()
        {

        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
        }
    }
}
