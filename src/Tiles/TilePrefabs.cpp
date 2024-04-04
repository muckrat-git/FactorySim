// File contains all constant tile data

#include "Other/Types.cpp"
#include "GasTypes.cpp"
#include "World/WorldContext.cpp"

struct TilePrefab {
    cstr name;                  // The tile name
    cstr texture;               // The tile filename

    bool transparent;           // Can light pass through the tile
    bool solid;                 // If the tile will block other things
    bool small = false;         // If the tile is smaller than a single unit
    bool interactable = false;

    u8 rotation = 0;            // Default rotation style (0=none 1=0/180 2=0/90/180/270)

    void (*update)(WorldContext, TileSprite&, TileData&, Vec2<int>);
    void (*interact)(WorldContext, TileSprite&, TileData&, Vec2<int>);
};

#define PREFAB(tileClass) (TilePrefab){\
    .name = tileClass.name,\
    .texture = tileClass.texture,\
    .transparent = tileClass.transparent,\
    .solid = tileClass.solid,\
    .small = tileClass.small,\
    .interactable = tileClass.interactable,\
    .rotation = tileClass.rotation,\
    .update = &tileClass.Update,\
    .interact = &tileClass.Interact\
}

enum TileType {
    TILE_EMPTY,
    TILE_STONE,
    TILE_SILT,
    TILE_TITANIUM,
    TILE_COPPER,
    TILE_DENSE_WALL,
    TILE_GLASS,
    TILE_OUTLET,
    TILE_DOOR,
    TILE_DOORPANEL_A,
    TILE_DOORPANEL_B,
    TILE_DOOR_OPEN,
    TILE_PIPE,
    TILE_PIPE_L,
    TILE_PIPE_T,
    TILE_PIPE_PLUS,
    TILE_WALL_PIPE,
    TILE_WALL_VENT,
    TILE_PIPE_PUMP,
    TILE_ITEM_TUBE
};

// Prototype tile functions
Tile DefaultTile(TileType primary, GasType gas, float gasMass);
Tile DefaultTile(TileType primary, GasType gas);
Tile DefaultTile(TileType primary);

#include "TileClass/TileClass.cpp"

const TilePrefab tilePrefabs[] = {
    {
        .name = "Ground",
        .texture = "resources/tiles/ground.png",
        
        .transparent = true,
        .solid = false,
        .small = true
    },
    {
        .name = "Stone",
        .texture = "resources/tiles/stone.png",

        .transparent = false,
        .solid = true
    },
    {
        .name = "Silt",
        .texture = "resources/tiles/silt.png",

        .transparent = false,
        .solid = true
    },
    {
        .name = "Titanium",
        .texture = "resources/tiles/titanium.png",

        .transparent = false,
        .solid = true,
        .rotation = 1
    },
    {
        .name = "Copper",
        .texture = "resources/tiles/copper.png",

        .transparent = false,
        .solid = true,
        .rotation = 1
    },
    {
        .name = "Insulated Tile",
        .texture = "resources/tiles/insulation.png",

        .transparent = false,
        .solid = true
    },
    {
        .name = "Glass",
        .texture = "resources/tiles/glass.png",
        
        .transparent = true,
        .solid = true
    },
    PREFAB(GasOutlet()),
    {
        .name = "Door",
        .texture = "resources/tiles/door.png",
        
        .transparent = false,
        .solid = true,
        .rotation = 2
    },
    PREFAB(DoorPanel(0)),
    PREFAB(DoorPanel(1)),
    {
        .name = "Door",
        .texture = "resources/tiles/door_open.png",
        
        .transparent = true,
        .solid = false,
        .rotation = 2
    },
    PREFAB(PipeTile(0, false)),
    PREFAB(PipeTile(1, false)),
    PREFAB(PipeTile(2, false)),
    PREFAB(PipeTile(3, false)),
    PREFAB(PipeTile(0, true)),
    PREFAB(WallVent()),
    PREFAB(PipePump()),
    {
        .name = "Tube",
        .texture = "resources/tiles/tubes/tube1.png",
        .transparent = true,
        .solid = false,
        .small = true,
        .rotation = 2
    },
    {
        .name = "Tube",
        .texture = "resources/tiles/tubes/tube2.png",
        .transparent = true,
        .solid = false,
        .small = true,
        .rotation = 2
    },
    {
        .name = "Tube",
        .texture = "resources/tiles/tubes/tube3.png",
        .transparent = true,
        .solid = false,
        .small = true,
        .rotation = 2
    },
    {
        .name = "Tube",
        .texture = "resources/tiles/tubes/tube4.png",
        .transparent = true,
        .solid = false,
        .small = true,
        .rotation = 2
    }
};

// Check if a tile at a position is air
u8 WorldContext::IsAir(Vec2<int> pos) {
    return !tilePrefabs[Get(pos).sprite.index].solid;
}
// Check if a tile at a position is air
u8 WorldContext::IsAir(int x, int y) {
    return !tilePrefabs[Get(x, y).sprite.index].solid;
}

GasType getDefaultGas(int id) {
    return (tilePrefabs[id].small || !tilePrefabs[id].solid) ? GAS_EMPTY : GAS_SOLID;
}

u8 getGasAlpha(float mass, GasType gas) {
    float d = defaultGasMass[gas];
    if(d == 0) return 0;
    d = Normalize(mass, 0, d);
    d = Clamp(d, 0, 1);
   
    // Apply circular smoothing to alpha value
    //d = sqrtf(1 - (d - 1)*(d - 1));

    return (u8)(d * 255);
}