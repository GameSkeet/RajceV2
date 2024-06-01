using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RajceV2Cheat.Tabs
{
    internal abstract class TabBase
    {
        public delegate void DrawSectionDelegate();

        public abstract string Name { get; protected set; }
        public Dictionary<string, DrawSectionDelegate> Sections { get; protected set; } = new Dictionary<string, DrawSectionDelegate>();

        protected abstract void AddSections();

        protected TabBase() => AddSections();
    }
}
