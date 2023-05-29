//
// Created by Garret on 1/11/2023.
//

#ifndef HEATMAP_GAMEGLOBALS_H
#define HEATMAP_GAMEGLOBALS_H

/// Container for global game state, delta time, etc.
namespace GameGlobals {
    /// Time between current frame and last frame.
    extern float deltaTime;
    /// Time of last frame.
    extern float lastFrame;

    /// Updates the delta time based on the current time.
    /// This should only really be called in one place in the main loop.
    void updateDeltaTime();
}

#endif //HEATMAP_GAMEGLOBALS_H
