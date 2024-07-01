using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using RajceV2Cheat.Features;

using UnityEngine.Networking;

namespace RajceV2Cheat.Tabs
{
    internal class DebugTab : TabBase
    {
        public override string Name { get; protected set; } = "Debug";

        private static void DoMainSection()
        {
            FrontendPipe.BeginContentBox("Debug");
            FrontendPipe.EndContentBox();

            FrontendPipe.BeginContentBox("Networking");
            {
                FrontendPipe.AddButton("Get channels", () =>
                {
                    if (!FeatureManager.Connected)
                        return;

                    NetworkManager manager = CustomNetworkManager.singleton;
                    NetworkClient client = manager.client;
                    
                    for (int i = 0; i < manager.channels.Count; i++) 
                        RajceV2.Logger.Msg("Channel #{0}: {1}", i, manager.channels[i]);
                });
            }
            FrontendPipe.EndContentBox();
        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
        }
    }
}
