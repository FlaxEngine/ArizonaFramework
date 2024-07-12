// Copyright (c) Wojciech Figat. All rights reserved.

#if FLAX_EDITOR

using System;
using System.Collections.Generic;
using FlaxEngine;
using FlaxEditor;
using FlaxEditor.Content;
using ArizonaFramework.Debug;

namespace ArizonaFramework.Editor
{
    /// <summary>
    /// Game Instance plugin for Editor.
    /// </summary>
    public sealed class GameInstanceEditor : EditorPlugin
    {
        private AssetProxy[] _assetProxies;

        /// <summary>
        /// Initializes a new instance of the <see cref="GameInstanceEditor"/> class.
        /// </summary>
        public GameInstanceEditor()
        {
            _description = new PluginDescription
            {
                Name = "GameInstance",
                Category = "Game",
                Description = "Main game singleton plugin that manages the game systems and handles Game Mode setup and lifetime for the play.",
                Author = "Flax",
                Version = new Version(1, 0),
            };
        }

        /// <inheritdoc />
        public override Type GamePluginType => typeof(GameInstance);

        /// <inheritdoc />
        public override void InitializeEditor()
        {
            base.InitializeEditor();

            _assetProxies = new[]
            {
                new CustomSettingsProxy(typeof(GameInstanceSettings), "GameInstance"),
                new CustomSettingsProxy(typeof(DebugSettings), "Debug"),
            };
            foreach (var e in _assetProxies)
                Editor.ContentDatabase.Proxy.Add(e);
        }

        /// <inheritdoc />
        public override void DeinitializeEditor()
        {
            foreach (var e in _assetProxies)
                Editor.ContentDatabase.Proxy.Remove(e);
            _assetProxies = null;

            base.DeinitializeEditor();
        }
    }
}

#endif
