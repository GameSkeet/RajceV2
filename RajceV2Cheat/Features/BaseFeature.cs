using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using UnityEngine.Networking;

namespace RajceV2Cheat.Features
{
    internal abstract class BaseFeature
    {
        public abstract string Name { get; protected set; }

        public bool IsActive = false;

        public virtual void Start() {}
        public virtual void OnDestroy() {}

        // This also works as OnRender (not unity's OnGUI but rendering with the frontend)
        public virtual void OnUpdate() {}
        public virtual void OnGUI() {}

        public virtual void OnConnected(NetworkClient client) {}
        public virtual void OnDisconnected() {}
    }
}
