using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using UnityEngine;
using HarmonyLib;

namespace RajceV2Cheat.Patches
{
    internal static class WeaponManagerPatch
    {
        [HarmonyPatch(typeof(WeaponManager))]
        [HarmonyPatch("CallCmdShoot")]
        [HarmonyPatch(MethodType.Normal)]
        [HarmonyPatch(new Type[] { typeof(GameObject), typeof(string), typeof(Vector3), typeof(Vector3), typeof(Vector3) })]
        private static class WM_CallCmdShootPatch
        {
            private static void Prefix(ref GameObject target, ref string hitboxType, Vector3 dir, ref Vector3 sourcePos, ref Vector3 targetPos)
            {
                
            }
        }
    }
}
