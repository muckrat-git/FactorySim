#pragma once

#include <raylib.h>
#include "Other/Types.cpp"
#include "World/Player.cpp"

#define WORLD_SIZE 600

class World {
    private:
    // Content is the interal representation of the world
    u8 * content;
    Texture2D tex;

    void LoadTex() {
        Image img;
        img.data = RL_CALLOC(WORLD_SIZE * WORLD_SIZE, sizeof(u8));
        img.width = WORLD_SIZE;
        img.height = WORLD_SIZE;
        img.mipmaps = 1;
        img.format = RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
        tex = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    public:
    Player * player;

    World() {}
    World(unsigned int seed) {
        content = new u8[WORLD_SIZE * WORLD_SIZE];
        SetRandomSeed(seed);
        Image heightmap = GenImagePerlinNoise(WORLD_SIZE, WORLD_SIZE, 0, 0, 5);
        LoadTex();
        Generate(heightmap);
        ExportImage(heightmap, "map.png");
        UnloadImage(heightmap);
    }

    u8 get(int x, int y) {
        if(x < 0 || y < 0 || x > WORLD_SIZE || y > WORLD_SIZE) return 0;
        return content[((y * WORLD_SIZE) + x)];
    }
    u8 get(Vec2<int> pos) {
        return get(pos.x, pos.y);
    }
    u8 isair(Vec2<int> pos) {
        return get(pos) == 1;
    }
    u8 isair(int x, int y) {
        return get(x, y) == 1;
    }
    Texture2D getTexture() {
        return tex;
    }
    void update() {
        UpdateTexture(tex, content);
    }

    void set(int x, int y, u8 val) {
        if(x < 0 || y < 0) return;
        content[((y * WORLD_SIZE) + x)] = val;
    }
    
    void Generate(Image heightmap) {
        for(u16 x = 0; x < WORLD_SIZE; ++x) {
            for(u16 y = 0; y < WORLD_SIZE; ++y) {
                set(x, y, (GetImageColor(heightmap, x, y).r > 100));
            }
        }
        update();
    }

    void Unload() {
        delete content;
    }
};