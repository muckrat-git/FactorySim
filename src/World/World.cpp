#pragma once

#include <raylib.h>
#include <rlgl.h>
#include "Other/Types.cpp"
#include "Tiles/Tile.cpp"
#include "External/stb_perlin.h"

#define WORLD_SIZE 600

class World {
    private:
    TileSprite * tileSprites;   // Tile representation to be sent to the shader for rendering
    TileData * tileContent;     // Tile data for other non-rendering uses

    Texture2D renderData;       // Texture form of tileSprites to be sent to shader 

    void LoadTex() {
        Image img;
        img.data = RL_CALLOC(WORLD_SIZE * WORLD_SIZE, sizeof(TileSprite));
        img.width = WORLD_SIZE;
        img.height = WORLD_SIZE;
        img.mipmaps = 1;
        img.format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8;
        renderData = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    public:
    World() {}
    World(unsigned int seed) {
        tileSprites = new TileSprite[WORLD_SIZE * WORLD_SIZE];
        tileContent = new TileData[WORLD_SIZE * WORLD_SIZE];
        SetRandomSeed(seed);
        LoadTex();
        Generate();
    }

    Tile get(int x, int y) {
        if(x < 0 || y < 0 || x > WORLD_SIZE || y > WORLD_SIZE) return {0, 0};
        return Tile{tileSprites[((y * WORLD_SIZE) + x)], tileContent[((y * WORLD_SIZE) + x)]};
    }
    Tile get(Vec2<int> pos) {
        return get(pos.x, pos.y);
    }
    u8 isair(Vec2<int> pos) {
        return get(pos).sprite.index   == TILE_EMPTY;
    }
    u8 isair(int x, int y) {
        return get(x, y).sprite.index  == TILE_EMPTY;
    }

    Texture2D getTexture() {
        return renderData;
    }
    void update() {
        UpdateTexture(renderData, tileSprites);
    }

    bool AreaIsAir(Vec2<float> pos, float area) {
        return 
            isair((pos + vec2(area, -area)).Int()) && 
            isair((pos + vec2(area, area)).Int()) && 
            isair((pos + vec2(-area, -area)).Int()) && 
            isair((pos + vec2(-area, area)).Int());
    }

    void set(int x, int y, Tile value) {
        if(x < 0 || y < 0) return;
        
        // Delete previous content
        tileContent[((y * WORLD_SIZE) + x)].Unload();

        tileSprites[((y * WORLD_SIZE) + x)] = value.sprite;
        tileContent[((y * WORLD_SIZE) + x)] = value.data;
    }
    
    void Generate() {
        const int mapCount = 4;
        Image maps[mapCount];
        maps[0] = GenImagePerlinNoise(WORLD_SIZE, WORLD_SIZE, 0, 0, 20);
        maps[1] = GenImagePerlinNoise(WORLD_SIZE, WORLD_SIZE, WORLD_SIZE, 0, 70);
        maps[2] = GenImagePerlinNoise(WORLD_SIZE, WORLD_SIZE, WORLD_SIZE * 2, 0, 90);
        maps[3] = GenImagePerlinNoise(WORLD_SIZE, WORLD_SIZE, WORLD_SIZE * 3, 0, 7);

        for(u16 x = 0; x < WORLD_SIZE; ++x) {
            for(u16 y = 0; y < WORLD_SIZE; ++y) {
                u8 color = GetImageColor(maps[0], x, y).r;
                TileType tile = (color > 70) ? TILE_STONE : TILE_EMPTY;

                if(tile == 1) {
                    if((float)GetImageColor(maps[1], x, y).r / 255.0f < 0.2f) tile = TILE_COPPER;
                    if((float)GetImageColor(maps[2], x, y).r / 255.0f < 0.12f) tile = TILE_TITANIUM;
                    if((float)GetImageColor(maps[3], x, y).r / 255.0f < 0.5 && color < 100) tile = TILE_SILT;
                }

                set(x, y, DefaultTile(tile, tile == TILE_EMPTY ? GAS_OXYGEN : GAS_SOLID));
            }
        }
        update();

        for(int i = 0; i < mapCount; ++i) UnloadImage(maps[i]);
    }

    void Unload() {
        UnloadTexture(renderData);
        delete tileSprites;

        // Delete tile content entries
        for(int i = 0; i < WORLD_SIZE * WORLD_SIZE; ++i) tileContent[i].Unload();
        delete tileContent;
    }
};