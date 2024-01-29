#include "TileClass.cpp"
#include "Other/Types.cpp"
#include "World/WorldContext.cpp"

class WallVent : public TileClass {
    public:
    cstr name = "Wall Vent";
    cstr texture = "resources/tiles/vent-tile.png";

    bool transparent = false;
    bool solid = true;

    u8 rotation = 2;

    // Static update function, run once per game tick
    static void Update(WorldContext ctx, TileSprite& sprite, TileData& data, Vec2<int> position) {
        float direction = float(sprite.rotation) * PI / 2.0f;
        Vec2<int> neighborPos = position + Vec2<float>{-sinf(direction), cosf(direction)}.iRound();
        Tile neighbor = ctx.Get(neighborPos);

        if(neighbor.sprite.gas == GAS_SOLID) return;
        if(neighbor.sprite.gas != GAS_EMPTY && data[TILEDATA_INTERNAL_ELEMENT] != GAS_EMPTY && neighbor.sprite.gas != data[TILEDATA_INTERNAL_ELEMENT]) {
            // Swap chance
            if(GetRandomValue(0, UPDATES_PER_SECOND) != 0) return;

            float oldMass = data[TILEDATA_INTERNAL_MASS];
            u8 oldElement = int(data[TILEDATA_INTERNAL_ELEMENT]);

            data[TILEDATA_INTERNAL_MASS] = neighbor.data[TILEDATA_GAS_MASS];
            data[TILEDATA_INTERNAL_ELEMENT] = neighbor.sprite.gas;

            neighbor.data[TILEDATA_GAS_MASS] = oldMass;
            ctx.GetTileSprite(neighborPos.x, neighborPos.y).gas = oldElement;
            return;
        }

        float mass = data[TILEDATA_INTERNAL_MASS];

        if(data[TILEDATA_INTERNAL_ELEMENT] == GAS_EMPTY) {
            if(neighbor.sprite.gas == GAS_EMPTY) return;

            data[TILEDATA_INTERNAL_ELEMENT] = neighbor.sprite.gas;
        }
        else if(neighbor.sprite.gas == GAS_EMPTY) {
            neighbor.sprite.gas = data[TILEDATA_INTERNAL_ELEMENT];
            ctx.GetTileSprite(neighborPos.x, neighborPos.y).gas = neighbor.sprite.gas;
        }

        mass += neighbor.data[TILEDATA_GAS_MASS];
        mass /= 2;

        data[TILEDATA_INTERNAL_MASS] = mass;
        ctx.GetTileData(neighborPos.x, neighborPos.y)[TILEDATA_GAS_MASS] = mass;
    }
};