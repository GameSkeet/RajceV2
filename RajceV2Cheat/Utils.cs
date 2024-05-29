using System;
using System.IO;
using System.Reflection;
using System.Collections.Generic;

using UnityEngine;

namespace RajceV2Cheat
{
    internal static class Utils
    {
        private static Dictionary<string, AssetBundle> Bundles = new Dictionary<string, AssetBundle>();

        public static byte[] GetResource(string name)
        {
            Assembly asm = Assembly.GetExecutingAssembly();
            foreach (string rname in asm.GetManifestResourceNames())
            {
                if (rname.EndsWith(name))
                {
                    MemoryStream ms = new MemoryStream();
                    asm.GetManifestResourceStream(rname).CopyTo(ms);

                    return ms.ToArray();
                }
            }

            return null;
        }

        // Stolen straight from the old version
        public static AssetBundle LoadBundle(string name)
        {
            if (Bundles.ContainsKey(name))
                return Bundles[name]; // Return the cached bundle

            AssetBundle bundle = AssetBundle.LoadFromMemory(GetResource(name)); // Load the bundle from the resource

            return Bundles[name] = bundle; // Chache the bundle so we dont need to load it more than once
        }
    }
}
