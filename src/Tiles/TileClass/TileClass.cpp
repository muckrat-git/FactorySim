#pragma once

class TileClass {
    public:
    cstr name;                      // The tile name
    cstr texture;                   // The tile filename
    
    bool solid = true;          // If the tile will block other things
    bool transparent = false;   // Can light pass through the tile
    bool small = false;         // If the tile is smaller than a single unit
    bool interactable = false;  // If the tile reacts to the player

    u8 rotation = 0;            // Default rotation style (0=none 1=0/180 2=0/90/180/270)

    // Static update function, run once per game tick
    static void Update(WorldContext, TileSprite&, TileData&, Vec2<int>){}

    // Static interaction function, run on game tick of tile interaction
    static void Interact(WorldContext, TileSprite&, TileData&, Vec2<int>){}
};

// Include all tiles
#include "GasOutlet.cpp"
#include "DoorPanel.cpp"
#include "WallVent.cpp"
#include "PipeTile.cpp"
#include "PipePump.cpp"