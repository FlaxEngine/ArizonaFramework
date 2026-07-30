// Copyright (c) Wojciech Figat. All rights reserved.

using FlaxEngine;

namespace ArizonaFramework
{
    partial class PlayerUI
    {
        private void SetViewportInternal(ref Float4 viewportRect)
        {
            if (Actor is UICanvas canvas)
            {
                canvas.ViewportRect = viewportRect;
            }
        }
    }
}
