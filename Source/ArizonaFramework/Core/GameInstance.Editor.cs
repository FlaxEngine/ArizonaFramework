// Copyright (c) Wojciech Figat. All rights reserved.

#if FLAX_EDITOR

using System;
using FlaxEngine;
using FlaxEditor;
using FlaxEditor.Content;

namespace ArizonaFramework.Editor
{
    /// <summary>
    /// Game Instance plugin for Editor.
    /// </summary>
    public sealed class GameInstanceEditor : EditorPlugin
    {
        private AssetProxy _assetProxy;

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

            _assetProxy = new CustomSettingsProxy(typeof(GameInstanceSettings), "GameInstance");
            Editor.ContentDatabase.Proxy.Add(_assetProxy);
        }

        /// <inheritdoc />
        public override void DeinitializeEditor()
        {
            Editor.ContentDatabase.Proxy.Remove(_assetProxy);
            _assetProxy = null;

            base.DeinitializeEditor();
        }
    }
}

#endif
