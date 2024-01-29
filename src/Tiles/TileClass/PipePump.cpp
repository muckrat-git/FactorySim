#include "TileClass.cpp"
#include "Other/Types.cpp"
#include "World/WorldContext.cpp"
#include <vector>
#include <algorithm>

using namespace std;

#define PUMP_RATE 1000.0f

class PipePump : public TileClass {
    public:
    cstr name = "Pipe";
    cstr texture;

    bool transparent = true;
    bool solid = true;
    bool small = true;

    u8 rotation = 2;

    PipePump() {
        texture = "resources/tiles/pipes/pipe-pump.png";
    }

    // Get connected pipes
    static vector<Vec2<int>> GetConnected(u8 rotation, u8 index) {
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
        float direction = float(sprite.rotation) * PI / 2.0f;
        Vec2<int> intakePos = position + Vec2<float>{sinf(direction), -cosf(direction)}.iRound();
        Tile intake = ctx.Get(intakePos);

        if(!intake.data.entryExists(TILEDATA_INTERNAL_ELEMENT)) return; 
        if(intake.data[TILEDATA_INTERNAL_ELEMENT] == GAS_EMPTY) return;

        // Check if connected
        vector<Vec2<int>> connections = GetConnected(intake.sprite.rotation, intake.sprite.index);
        if(find(connections.begin(), connections.end(), ivec2(0) - (intakePos-position)) == connections.end()) return;

        // Ensure elements are transferable
        if(data[TILEDATA_INTERNAL_ELEMENT] == GAS_EMPTY) data[TILEDATA_INTERNAL_ELEMENT] = intake.data[TILEDATA_INTERNAL_ELEMENT];
        if(data[TILEDATA_INTERNAL_ELEMENT] != intake.data[TILEDATA_INTERNAL_ELEMENT]) return;

        // Ensure gas mass is more than rate
        if(intake.data[TILEDATA_INTERNAL_MASS] <= PUMP_RATE / UPDATES_PER_SECOND) {
            data[TILEDATA_INTERNAL_MASS] += intake.data[TILEDATA_INTERNAL_MASS];
            intake.data[TILEDATA_INTERNAL_MASS] = 0;
            intake.data[TILEDATA_INTERNAL_ELEMENT] = GAS_EMPTY;
            return;
        }

        // Transfer
        data[TILEDATA_INTERNAL_MASS] += PUMP_RATE / UPDATES_PER_SECOND;
        intake.data[TILEDATA_INTERNAL_MASS] -= PUMP_RATE / UPDATES_PER_SECOND;
    }
};