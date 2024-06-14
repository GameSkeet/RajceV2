using UnityEngine;
using UnityEngine.Networking;

namespace RajceV2Cheat.Features.Misc
{
    internal class CursorFix : BaseFeature
    {
        private Texture2D m_tCursor = null;

        public override string Name { get; protected set; } = "Cursor Fix";

        public override void Start()
        {
            if (m_tCursor != null)
                return;

            m_tCursor = (Texture2D)Utils.LoadBundle("Data.res").LoadAsset<Texture2D>("Dot");
            GameObject.DontDestroyOnLoad(m_tCursor);
        }

        public override void OnConnected(NetworkClient client)
        {
            RajceV2.Logger.Warning("Connected to a server!");
            foreach (Item i in Pickup.inv.availableItems)
                i.crosshair = m_tCursor;
        }
    }
}
