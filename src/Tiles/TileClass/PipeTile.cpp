#include "TileClass.cpp"
#include "Other/Types.cpp"
#include "World/WorldContext.cpp"
#include <vector>
#include <algorithm>

using namespace std;

class PipeTile : public TileClass {
    public:
    cstr name = "Pipe";
    cstr texture;

    bool transparent = true;
    bool solid = false;
    bool small = true;

    u8 rotation = 2;

    PipeTile(u8 type, bool wall) {
        switch(type) {
            case 0:
                texture = "resources/tiles/pipes/pipe.png";
                break;
            case 1:
                texture = "resources/tiles/pipes/pipe-l.png";
                break;
            case 2:
                texture = "resources/tiles/pipes/pipe-t.png";
                break;
            case 3:
                texture = "resources/tiles/pipes/pipe-plus.png";
                break;
        }

        if(wall) texture = "resources/tiles/pipes/wall-pipe.png";

        transparent = true && !wall;
        solid = false || wall;
        small = true && !wall;
    }

    // Get connected pipes
    static vector<Vec2<int>> GetConnected(u8 rotation, u8 index) {
        if(index == TILE_PIPE_PUMP) return vector<Vec2<int>>{vec2(-sinf(float(rotation) * PI / 2.0f), cosf(float(rotation) * PI / 2.0f)).iRound()};
        vector<u8> angles;
        angles.push_back(0);

        switch(index) {
            case TILE_PIPE_L:
                angles.push_back(3);
                break;
            case TILE_PIPE_T:
                angles.push_back(1);
                angles.push_back(3);
                break;
            case TILE_PIPE_PLUS:
                angles.push_back(1);
                angles.push_back(2);
                angles.push_back(3);
                break;
            default:
                angles.push_back(2);
                break;
        }

        vector<Vec2<int>> ret;
        for(u8 angle : angles) {
            ret.push_back(vec2(-sinf(float(angle+rotation) * PI / 2.0f), cosf(float(angle+rotation) * PI / 2.0f)).iRound());
        }
        return ret;
    }

    // Static update function, run once per game tick
    static void Update(WorldContext ctx, TileSprite& sprite, TileData& data, Vec2<int> position) {
        vector<Vec2<int>> neighbors = GetConnected(sprite.rotation, sprite.index);

        vector<Vec2<int>> validNeighbors;
        float mass = data[TILEDATA_INTERNAL_MASS];

        vector<Vec2<int>> swapPos;

        for(Vec2<int> relativePos : neighbors) {
            // Get absolute position and tile object
            Vec2<int> neighborPos = relativePos + position;
            Tile neighbor = ctx.Get(neighborPos);

            // Ensure is pipe-able
            if(!neighbor.data.entryExists(TILEDATA_INTERNAL_ELEMENT)) continue;
            
            // Check if connected
            vector<Vec2<int>> connections = GetConnected(neighbor.sprite.rotation, neighbor.sprite.index);
            if(find(connections.begin(), connections.end(), ivec2(0) - relativePos) == connections.end()) continue;

            if(data[TILEDATA_INTERNAL_ELEMENT] == GAS_EMPTY) data[TILEDATA_INTERNAL_ELEMENT] = neighbor.data[TILEDATA_INTERNAL_ELEMENT]; 
            if(neighbor.data[TILEDATA_INTERNAL_ELEMENT] == GAS_EMPTY) ctx.GetTileData(neighborPos.x, neighborPos.y)[TILEDATA_INTERNAL_ELEMENT] = data[TILEDATA_INTERNAL_ELEMENT];
            if(neighbor.data[TILEDATA_INTERNAL_ELEMENT] != data[TILEDATA_INTERNAL_ELEMENT]) {
                swapPos.push_back(neighborPos);
                continue;
            }

            mass += neighbor.data[TILEDATA_INTERNAL_MASS];

            validNeighbors.push_back(neighborPos);
        }

        mass /= validNeighbors.size()+1;

        // Now apply mass
        for(Vec2<int> neighborPos : validNeighbors) {
            ctx.GetTileData(neighborPos.x, neighborPos.y)[TILEDATA_INTERNAL_MASS] = mass;
        }

        data[TILEDATA_INTERNAL_MASS] = mass;

        // Gas swapping
        if(GetRandomValue(0, 2) == 0 && swapPos.size()) {
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
            }
            else {
                // Select random
                pos = swapPos[GetRandomValue(0, swapPos.size()-1)];
            }
            
            // Swap gasses
            TileData& swapData = ctx.GetTileData(pos.x, pos.y);
            float oldMass = swapData[TILEDATA_INTERNAL_MASS];
            float oldGas = swapData[TILEDATA_INTERNAL_ELEMENT];

            // Set swap tile
            swapData[TILEDATA_INTERNAL_MASS] = data[TILEDATA_INTERNAL_MASS];
            swapData[TILEDATA_INTERNAL_ELEMENT] = data[TILEDATA_INTERNAL_ELEMENT];
            swapData.lock = true;

            // Set host tile
            data[TILEDATA_INTERNAL_MASS] = oldMass;
            data[TILEDATA_INTERNAL_ELEMENT] = oldGas;
        }
    }
};