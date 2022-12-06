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
}
