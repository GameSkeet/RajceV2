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
        public int TestIntVal = 0;
        public float TestFloatVal = 5.0f;
        public int[] ComboValues = CreateComboArray(30);

        private unsafe void DoMainSection()
        {
            FrontendPipe.BeginContentBox("Aimbot");
            FrontendPipe.AddButton("Kokot", () =>
            {
                RajceV2.Logger.Warning("Called from frontend!");
            });
            fixed (int* ptr_i = &TestIntVal)
                FrontendPipe.AddSliderInt("Test Int Slider", ptr_i, -1, 90, (val) =>
                {
                    RajceV2.Logger.Msg("Slider val: {0}", val);
                });
            FrontendPipe.EndContentBox();

            FrontendPipe.BeginContentBox("Misc");
            fixed (bool* ptr = &InnerBool)
                FrontendPipe.AddCheckbox("Test checkbox", ptr);
            fixed (float* ptr_f = &TestFloatVal)
                FrontendPipe.AddSliderFloat("Float slider", ptr_f, 0, 150);
            List<string> values = new List<string>();
            for (int i = 0; i < ComboValues.Length; i++)
                values.Add("Value" + i);
            FrontendPipe.AddComboBox("Combo box #1", ArrayToPtr<int>(ComboValues), values.ToArray(), true);
            FrontendPipe.EndContentBox();
        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
        }
    }
}
