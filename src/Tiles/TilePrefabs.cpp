// File contains all constant tile data

#include "Other/Types.cpp"

struct TilePrefab {
    cstr name;              // The tile name
    cstr texture;           // The tile filename

    bool transparent;       // Can light pass through the tile
    bool solid;             // If the tile is solid
};

enum TileType {
    TILE_EMPTY,
    TILE_STONE,
    TILE_SILT,
    TILE_TITANIUM,
    TILE_COPPER,
    TILE_DENSE_WALL,
    TILE_GLASS
};

const TilePrefab tilePrefabs[] = {
    {
        .name = "Ground",
        .texture = "resources/tiles/ground.png",
        
        .transparent = true,
        .solid = false
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
        .solid = true
    },
    {
        .name = "Copper",
        .texture = "resources/tiles/copper.png",

        .transparent = false,
        .solid = true
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
    {
        .name = "_Gas Outlet",
        .texture = "resources/tiles/gas_outlet.png",
        
        .transparent = true,
        .solid = true
    },
    {
        .name = "Door",
        .texture = "resources/tiles/door.png",
        
        .transparent = false,
        .solid = true
    },
    {
        .name = "Door Panel A",
        .texture = "resources/tiles/door_panel1.png",
        
        .transparent = false,
        .solid = true
    },
    {
        .name = "Door Panel B",
        .texture = "resources/tiles/door_panel2.png",
        
        .transparent = false,
        .solid = true
    }
};