#pragma once

#include <raylib.h>
#include <rlgl.h>
#include "Other/Types.cpp"
#include "Tiles/Tile.cpp"
#include "External/stb_perlin.h"

#include <thread>

#define WORLD_SIZE 600

class World : public WorldContext {
    Texture2D renderData;       // Texture form of tileSprites to be sent to shader 
    thread updateThread;
    float updateTime = 0;

    static void UpdateTile(int x, int y, WorldContext ctx) {
        TileSprite& tile = ctx.GetTileSprite(x,y);
        TileData& data = ctx.GetTileData(x,y);

        if(data.lock) {
            data.lock = false;
            return;
        }

        if(tile.gas != GAS_SOLID) {
            tile.alpha = getGasAlpha(data[TILEDATA_GAS_MASS], (GasType)tile.gas);

            Vec2<int> neighbor;
            u8 nextGas;
            float mass = data[TILEDATA_GAS_MASS];

            vector<Vec2<int>> swapPos;
            vector<Vec2<int>> identicalPos;
            int identical = 0;  // Number of surronding identical tiles
            
            // Check all neighboring gases
            for(float angle = 0; angle < PI*2; angle+=PI/2) {
                neighbor = ivec2(x+int(cos(angle)*1.5), y+int(sin(angle)*1.5));

                // Skip if out of bounds
                if(neighbor.x >= WORLD_SIZE || neighbor.y >= WORLD_SIZE || neighbor.x < 0 || neighbor.y < 0) continue;

                nextGas = ctx.GetTileSprite(neighbor.x, neighbor.y).gas;
                if(nextGas != GAS_SOLID) {
                    // If empty then change to neighboring gas
                    if(tile.gas == GAS_EMPTY && nextGas != GAS_EMPTY) ctx.GetTileSprite(x,y).gas = nextGas;

                    if(tile.gas == nextGas) {
                        ++identical;
                        identicalPos.push_back(neighbor);

                        // Add mass to mass average
                        mass += ctx.GetTileData(neighbor.x, neighbor.y)[TILEDATA_GAS_MASS];
                        mass /= 2.0f;
                        ctx.GetTileData(neighbor.x, neighbor.y)[TILEDATA_GAS_MASS] = mass;
                    }
                    else swapPos.push_back(neighbor);
                }
            }

            if(mass < 0.02) {
                mass = 0;
                tile.gas = GAS_EMPTY;
            }
            data[TILEDATA_GAS_MASS] = mass;

            // Gas swapping
            if(GetRandomValue(0, UPDATES_PER_SECOND * pow(identical + 1, 2)) == 0 && swapPos.size()) {
                // Find lowest pressure area
                int lowest = 0;
                float pressure = 0; // Pressure difference
                float lastPressure = ctx.GetTileData(swapPos[0].x, swapPos[0].y)[TILEDATA_GAS_MASS];
                for(int i = 1; i < swapPos.size(); ++i) {
                    float nextPressure = abs(lastPressure - ctx.GetTileData(swapPos[i].x, swapPos[i].y)[TILEDATA_GAS_MASS]);
                    if(nextPressure > pressure) {
                        pressure = nextPressure;
                        lastPressure = pressure;
                        lowest = i;
                    }
                }

                // Ensure pressure difference is large enough
                Vec2<int> pos;
                if(pressure > 20) {
                    pos = swapPos[lowest]; 
                    cout << pressure << endl;
                }
                else {
                    // Select random
                    pos = swapPos[GetRandomValue(0, swapPos.size()-1)];
                }
            
                // Swap gasses
                TileData& swapData = ctx.GetTileData(pos.x, pos.y);
                TileSprite& swapSprite = ctx.GetTileSprite(pos.x, pos.y);
                float oldMass = swapData[TILEDATA_GAS_MASS];
                u8 oldGas = swapSprite.gas;

                // Set swap tile
                swapData[TILEDATA_GAS_MASS] = data[TILEDATA_GAS_MASS];
                swapSprite.gas = tile.gas;
                swapSprite.alpha = getGasAlpha(swapData[TILEDATA_GAS_MASS], (GasType)swapSprite.gas);
                swapData.lock = true;

                // Set host tile
                data[TILEDATA_GAS_MASS] = oldMass;
                tile.gas = oldGas;
                tile.alpha = getGasAlpha(oldMass, (GasType)oldGas);
            }
            else if(GetRandomValue(0, UPDATES_PER_SECOND) == 0 && identical && swapPos.size()) {
                // Check for higher pressure neighbors
                bool compress = false;
                for(Vec2<int> pos : swapPos) {
                    if(ctx.GetTileData(pos.x, pos.y)[TILEDATA_GAS_MASS] > data[TILEDATA_GAS_MASS] * 1.5) {
                        compress = true;
                        break;
                    }
                }
                
                // Do random compression
                if(compress) {
                    // Get random neighbor
                    int i = GetRandomValue(0, identical-1);
                    TileData& compressData = ctx.GetTileData(identicalPos[i].x, identicalPos[i].y);

                    // Compress into neighbor
                    compressData[TILEDATA_GAS_MASS] += data[TILEDATA_GAS_MASS];
                    compressData.lock = true;
                    data[TILEDATA_GAS_MASS] = 0;
                    tile.gas = 0;
                    tile.alpha = 0;
                }
            }
        }

        TilePrefab prefab = tilePrefabs[tile.index];

        // Run tile class update function
        if(prefab.update != nullptr)
            prefab.update(ctx, tile, data, Vec2<int>{x,y});

        // Run tile interact function
        if(prefab.interactable && ctx.TileInteracted(x, y))
            prefab.interact(ctx, tile, data, Vec2<int>{x,y});
    }

    static void UpdateWorld(WorldContext ctx) {
        double t1 = GetTime();
        for(int x = 0; x < WORLD_SIZE; ++x) {
            for(int y = 0; y < WORLD_SIZE; ++y) {
                UpdateTile(x, y, ctx);
            }
        }
        //cout << UPDATES_PER_SECOND * (GetTime() - t1) << endl;
    }

    void LoadTex() {
        Image img;
        img.data = RL_CALLOC(WORLD_SIZE * WORLD_SIZE, sizeof(TileSprite));
        img.width = WORLD_SIZE;
        img.height = WORLD_SIZE;
        img.mipmaps = 1;
        img.format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        renderData = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    public:
    WorldContext ctx;
    World() {}
    World(unsigned int seed) {
        tileSprites = new TileSprite[WORLD_SIZE * WORLD_SIZE];
        tileContent = new TileData[WORLD_SIZE * WORLD_SIZE];
        ctx.tileSprites = tileSprites;
        SetRandomSeed(seed);
        LoadTex();
        Generate();
    }

    void StartUpdater() {
        // Init update thread
        ctx.tileContent = new TileData[WORLD_SIZE * WORLD_SIZE];
        for(int i = 0; i < WORLD_SIZE * WORLD_SIZE; ++i) 
            ctx.tileContent[i] = this->tileContent[i].Clone();
            
        ctx.interacted = interacted;
        ctx.hoverTile = hoverTile;
        
        updateThread = thread(UpdateWorld, ctx);
        updateTime = 0;
    }

    void CheckUpdater(float deltaT) {
        updateTime += deltaT;
        if(updateTime > 1.0f/UPDATES_PER_SECOND) {
            updateThread.join();

            interacted = interacted == false && IsKeyDown(KEY_E);

            StartUpdater();

            for(int i = 0; i < WORLD_SIZE * WORLD_SIZE; ++i) {
                this->tileContent[i].Unload();
            }
            delete tileContent;
            tileContent = ctx.tileContent;

            Update();

            interacted = IsKeyDown(KEY_E);
        }
    }

    inline Texture2D GetTexture() {
        return renderData;
    }
    inline void Update() {
        UpdateTexture(renderData, tileSprites);
    }

    bool AreaIsAir(Vec2<float> pos, float area) {
        return 
            IsAir((pos + vec2(area, -area)).Int()) && 
            IsAir((pos + vec2(area, area)).Int()) && 
            IsAir((pos + vec2(-area, -area)).Int()) && 
            IsAir((pos + vec2(-area, area)).Int());
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

                Set(x, y, DefaultTile(tile, tile == TILE_EMPTY ? GAS_EMPTY : GAS_SOLID));
            }
        }
        Update();

        for(int i = 0; i < mapCount; ++i) UnloadImage(maps[i]);
    }

    void Unload() {
        updateThread.join();

        UnloadTexture(renderData);
        delete tileSprites;

        // Delete tile content entries
        for(int i = 0; i < WORLD_SIZE * WORLD_SIZE; ++i) tileContent[i].Unload();
        delete tileContent;
    }
};