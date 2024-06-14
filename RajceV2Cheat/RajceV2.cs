using System;
using System.IO;
using System.Runtime.InteropServices;

using MelonLoader;

using RajceV2Cheat.Features;

namespace RajceV2Cheat
{
    public class RajceV2 : MelonMod
    {
        public static RajceV2 Inst { get; private set; }
        public static MelonLogger.Instance Logger => Inst.LoggerInstance;
        public static HarmonyLib.Harmony harmony => Inst.HarmonyInstance;
        public static string UserData => Path.Combine(MelonUtils.UserDataDirectory, "RajceV2");

        public override unsafe void OnInitializeMelon()
        {
            Inst = this;

            if (!Directory.Exists(UserData))
                Directory.CreateDirectory(UserData);

            if (!FrontendPipe.InitTransporter())
            {
                Logger.Error("Failed to initialize frontend library, Error: {0}", FrontendPipe.InitErrorCode);
                return;
            }

            FeatureManager.Init();
            Menu.Init();
        }

        public override void OnUpdate()
        {
            Keybinds.OnUpdate();
            FeatureManager.OnUpdate();
        }
        public override void OnGUI()
        {
            FeatureManager.OnGUI();
        }

        public override void OnDeinitializeMelon()
        {
            Menu.Destroy();
            FeatureManager.Destroy();

            FrontendPipe.DestroyTransporter();
        }
    }
}
