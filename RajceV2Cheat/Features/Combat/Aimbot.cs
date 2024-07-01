using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RajceV2Cheat.Features.Combat
{
    internal class Aimbot : BaseFeature
    {
        public override string Name { get; protected set; } = "Aimbot";

        public const int DefaultHitbox = 1; // Default is body
        public readonly string[] Hitboxes = new string[]
        {
            "Head", // Does 4x the damage
            "Body", // Does base damage
            "Leg", // Does half the damage
            "SCP106" // Does 10x less damage
        };

        public IntPtr AimbotKey = IntPtr.Zero;

        public bool ForceHitboxEnabled = false;
        public int ForcedHitbox = DefaultHitbox;

        public int AimbotFOV = 10;

        public override void Start()
        {
            AimbotKey = Keybinds.CreateKey("Aimbot", UnityEngine.KeyCode.K, Structs.KeybindType.Toggle);
        }
    }
}
