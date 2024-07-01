using System;
using System.Collections.Generic;

using UnityEngine.Networking;

namespace RajceV2Cheat.Features.Combat
{
    // This works pretty much the same as the old one (just edited to work on the new version)
    internal class WeaponMods : BaseFeature
    {
        private WeaponManager m_Manager = null;
        private Dictionary<WeaponManager.Weapon, Dictionary<string, object>> OrigValues = new Dictionary<WeaponManager.Weapon, Dictionary<string, object>>();

        public override string Name { get; protected set; } = "Weapon mods";

        public int Firerate = -1;
        public bool NoSpread = false;
        public bool NoRecoil = false;
        public bool InstantReload = false;
        public bool Fullauto = false;

        public static void CallUpdateWpns()
        {
            Type t = typeof(WeaponMods);
        }
        
        public void UpdateWeaponMods()
        {
            if (m_Manager == null)
                return;

            foreach (WeaponManager.Weapon w in m_Manager.weapons)
            {
                if (!OrigValues.TryGetValue(w, out var values))
                    continue;

                if (Firerate > 0)
                    w.shotsPerSecond = Firerate;
                else w.shotsPerSecond = (float)values["firerate"];
                if (NoSpread)
                    w.unfocusedSpread = 0;
                else w.unfocusedSpread = (float)values["spread"];
                if (NoRecoil)
                    w.recoil = new RecoilProperties();
                else w.recoil = (RecoilProperties)values["recoil"];
                if (InstantReload)
                    w.reloadingTime = 0;
                else w.reloadingTime = (float)values["reload_time"];
                if (Fullauto)
                    w.allowFullauto = true;
                else w.allowFullauto = (bool)values["fullauto"];
            }
        }
        public void OnDisable()
        {
            if (m_Manager == null)
                return;

            foreach (WeaponManager.Weapon w in m_Manager.weapons)
            {
                if (!OrigValues.TryGetValue(w, out var values))
                    continue;

                w.shotsPerSecond = (float)values["firerate"];
                w.unfocusedSpread = (float)values["spread"];
                w.recoil = (RecoilProperties)values["recoil"];
                w.reloadingTime = (float)values["reload_time"];
                w.allowFullauto = (bool)values["fullauto"];
            }
        }

        public override void OnConnected(NetworkClient client)
        {
            m_Manager = PlayerManager.localPlayer.GetComponent<WeaponManager>();
            if (m_Manager == null)
                return;

            foreach (WeaponManager.Weapon w in m_Manager.weapons)
            {
                Dictionary<string, object> values = new Dictionary<string, object>();
                values["firerate"] = w.shotsPerSecond;
                values["spread"] = w.unfocusedSpread;
                values["recoil"] = w.recoil;
                values["reload_time"] = w.reloadingTime;
                values["fullauto"] = w.allowFullauto;

                OrigValues.Add(w, values);
            }

            UpdateWeaponMods();
        }
        public override void OnDisconnected()
        {
            m_Manager = null;
            OrigValues.Clear();
        }
    }
}
