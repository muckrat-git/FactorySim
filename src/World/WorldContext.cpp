#pragma once

#include "Tiles/Tile.cpp"
#include "Tiles/TileData.cpp"

#define UPDATES_PER_SECOND 10.0f
#define WORLD_SIZE 600

class WorldContext {
    public:
    TileSprite * tileSprites;   // Tile representation to be sent to the shader for rendering
    TileData * tileContent;     // Tile data for other non-rendering uses

    Vec2<int> hoverTile;
    bool interacted = false;

    Tile Get(int x, int y) {
        return Tile{tileSprites[((y * WORLD_SIZE) + x)], tileContent[((y * WORLD_SIZE) + x)]};
    }
    Tile Get(Vec2<int> pos) {
        return Get(pos.x, pos.y);
    }

    void Set(int x, int y, Tile value) {
        if(x < 0 || y < 0) return;
        
        // Delete previous content
        tileContent[((y * WORLD_SIZE) + x)].Unload();

        tileSprites[((y * WORLD_SIZE) + x)] = value.sprite;
        tileContent[((y * WORLD_SIZE) + x)] = value.data;
    }

    TileSprite & GetTileSprite(int x, int y) {
        return tileSprites[((y * WORLD_SIZE) + x)];
    }
    TileData & GetTileData(int x, int y) {
        return tileContent[((y * WORLD_SIZE) + x)];
    }

    bool TileInteracted(int x, int y) {
        if(Vec2<int>{x,y} != hoverTile) return false;
        if(interacted) {
            interacted = false;
            return true;
        };
        return false;
    }

    u8 IsAir(Vec2<int> pos);
    u8 IsAir(int x, int y); 
};