using System;
using System.Collections.Generic;

using RajceV2Cheat.Features.Misc;
using RajceV2Cheat.Features.Combat;

using UnityEngine.Networking;

namespace RajceV2Cheat.Features
{
    internal static class FeatureManager
    {
        private static Dictionary<Type, BaseFeature> Features = new Dictionary<Type, BaseFeature>();

        public static bool Connected { get; private set; } = false;

        private static void AddFeature(BaseFeature bf)
        {
            Features.Add(bf.GetType(), bf);
            bf.Start();
        }
        public static T GetFeature<T>() where T : BaseFeature
        {
            if (Features.TryGetValue(typeof(T), out BaseFeature bf))
                return (T)bf;

            return null;
        }

        public static void Init()
        {
            // Combat
            AddFeature(new Aimbot());
            AddFeature(new WeaponMods());

            // Misc
            AddFeature(new CursorFix());

            /*foreach (KeyValuePair<Type, BaseFeature> kvp in Features)
                RajceV2.Logger.Msg("Type: {0}, Name: {1}", kvp.Key.FullName, kvp.Value.Name);*/
        }
        public static void Destroy()
        {
            foreach (BaseFeature bf in Features.Values)
                bf.OnDestroy();

            Features.Clear();
        }

        public static void OnUpdate()
        {
            bool changed = false;
            bool connected = false;
            NetworkManager cnm = CustomNetworkManager.singleton;
            NetworkClient nclient = null;
            if (cnm != null)
            {
                nclient = cnm.client;
                connected = cnm.IsClientConnected() && cnm.clientLoadedScene;
                /*RajceV2.Logger.Msg("Off: {0}, Onl: {1}", cnm.offlineScene, cnm.onlineScene);
                RajceV2.Logger.Msg("Loaded: {0}", cnm.clientLoadedScene);*/

                if (connected && !Connected)
                    changed = true;
                if (!connected && Connected)
                    changed = true;

                Connected = connected;
            }

            foreach (BaseFeature bf in Features.Values)
            {
                bf.OnUpdate();

                if (changed)
                    if (connected)
                        bf.OnConnected(nclient);
                    else bf.OnDisconnected();
            }
        }
        public static void OnGUI()
        {
            foreach (BaseFeature bf in Features.Values)
                bf.OnGUI();
        }
    }
}
