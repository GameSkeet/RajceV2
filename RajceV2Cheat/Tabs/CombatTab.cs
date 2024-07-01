using System;

using RajceV2Cheat.Features;
using RajceV2Cheat.Features.Combat;

namespace RajceV2Cheat.Tabs
{
    internal class CombatTab : TabBase
    {
        private static WeaponMods wmods = null;

        public override string Name { get; protected set; } = "Combat";

        private unsafe void DoMainSection()
        {
            FrontendPipe.BeginContentBox("Aimbot");
            {
                Aimbot aimbot = FeatureManager.GetFeature<Aimbot>();

                FrontendPipe.AddKeybind("Aimbot key", aimbot.AimbotKey);
                fixed (bool* enabled = &aimbot.ForceHitboxEnabled)
                    FrontendPipe.AddCheckbox("Force hitbox shot", enabled);
                fixed (int* hitbox = &aimbot.ForcedHitbox)
                    FrontendPipe.AddComboBox("Aimbot hitbox", hitbox, aimbot.Hitboxes);
                fixed (int* fov = &aimbot.AimbotFOV)
                    FrontendPipe.AddSliderInt("Aimbot FOV", fov, 1, 180);
            }
            FrontendPipe.EndContentBox();

            FrontendPipe.BeginContentBox("Weapon mods");
            {
                wmods = FeatureManager.GetFeature<WeaponMods>(); // The variable must be static otherwise C# incorrectly converts the methods after the first one

                fixed (bool* enabled = &wmods.IsActive)
                    FrontendPipe.AddCheckbox("Enable mods", enabled, (val) =>
                    {
                        if (val)
                            wmods.UpdateWeaponMods();
                        else wmods.OnDisable();
                    });
                fixed (int* firerate = &wmods.Firerate)
                    FrontendPipe.AddSliderInt("Fire rate", firerate, -1, 20, (val) => wmods.UpdateWeaponMods());
                fixed (bool* spread = &wmods.NoSpread)
                    FrontendPipe.AddCheckbox("No spread", spread, (val) => wmods.UpdateWeaponMods());
                fixed (bool* recoil = &wmods.NoRecoil)
                    FrontendPipe.AddCheckbox("No recoil", recoil, (val) => wmods.UpdateWeaponMods());
                fixed (bool* reload = &wmods.InstantReload)
                    FrontendPipe.AddCheckbox("Instant reload", reload, (val) => wmods.UpdateWeaponMods());
                fixed (bool* fullauto = &wmods.Fullauto)
                    FrontendPipe.AddCheckbox("Full auto", fullauto, (val) => wmods.UpdateWeaponMods());
            }
            FrontendPipe.EndContentBox();
        }

        protected override void AddSections()
        {
            Sections.Add("Main", DoMainSection);
        }
    }
}
