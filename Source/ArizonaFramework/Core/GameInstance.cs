// Copyright (c) Wojciech Figat. All rights reserved.

namespace ArizonaFramework
{
    partial class GameInstance
    {
        /// <summary>
        /// Gets the game system of the given type.
        /// </summary>
        /// <typeparam name="T">Type of the system to search for. Includes any scripts derived from the type.</typeparam>
        /// <returns>Found system or null.</returns>
        public T GetGameSystem<T>() where T : GameSystem
        {
            return GetGameSystem(typeof(T)) as T;
        }
    }

    partial class GameInstanceSettings
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="GameInstanceSettings"/> class.
        /// </summary>
        public GameInstanceSettings()
        {
            // Init with defaults (C# lacks of proper empty ctor for structures)
            DefaultReplicationSettings = ReplicationSettings.Default;
        }
    }
}
