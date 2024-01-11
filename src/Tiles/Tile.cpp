#pragma once

#include <map>

#include "Other/Types.cpp"
#include "Tiles/TileData.cpp"
#include "Tiles/TileTypes.cpp"

using namespace std;

// TileSprite contains information vital for displaying the tile
struct TileSprite {
    u8 index;       // Index used to detirmine tile properties and appearance
    u8 gas = 0;     // Index of the gas that is over the tile
    u8 alpha = 0;   // The opacity of the gas coverage
};

struct Tile {
    TileSprite sprite;
    TileData data;
};

TileData InitialTileData(TileType primary, GasType gas, float gasMass) {
    // Temporary dynamic map to be transfered to fixed size TileData object
    map<u8, float> dataMap;

    if(gas != GAS_SOLID) dataMap.insert({TILEDATA_GAS_MASS, gasMass});

    // Move map data into tile data
    TileData tileData = TileData(dataMap.size());
    u8 i = 0;
    for(auto elem : dataMap) {
        tileData.overwrite(i, elem.first, elem.second);
        ++i;
    }
    return tileData;
}

// Generates a full Tile from a basic tile types
Tile DefaultTile(TileType primary, GasType gas, float gasMass) {
    return Tile{
        .sprite = TileSprite {
            (u8)primary,
            (u8)gas,
            getGasAlpha(gasMass, gas),
        },
        .data = InitialTileData(primary, gas, gasMass)
    };
}

// Generates a full Tile from a basic tile types (default gas mass)
Tile DefaultTile(TileType primary, GasType gas) {
    return DefaultTile(primary, gas, defaultGasMass[gas]);
}


// Generates a full Tile from a basic tile types (default gas)
Tile DefaultTile(TileType primary) {
    return DefaultTile(primary, defaultGasses[primary]);
}

struct SerializedTile {
    u8 entries;
    u8 tileSprite[3];
    u8 * indices;
    u8 * values;

    Tile Deserialize() {
        // Load tileSprite
        TileSprite tileSprite = {
            this->tileSprite[0],
            this->tileSprite[1],
            this->tileSprite[2]
        };

        // Load tileData
        TileData tileData = TileData(entries);
        for(int i = 0; i < entries; ++i) {
            tileData.data[i].index = indices[i];
            tileData.data[i].value = ((float *)indices)[i];
        }

        return {tileSprite, tileData};
    }

    SerializedTile() {}

    SerializedTile(Tile tile) {
        // Load tileSprite
        tileSprite[0] = tile.sprite.index;
        tileSprite[1] = tile.sprite.gas;
        tileSprite[2] = tile.sprite.alpha;

        // Get data entry count
        entries = tile.data.size;

        // Allocate tile data
        indices = (u8 *)malloc(tile.data.size);
        values = (u8 *)malloc(tile.data.size * sizeof(float));  // Allocate as if array of floats

        // Transfer entries
        for(int i = 0; i < entries; ++i) {
            indices[i] = tile.data.data[i].index;
            ((float *)indices)[i] = tile.data.data[i].value;
        }
    }

    u8 * ToBytes() {
        // Byte array to be returned
        u8 * ret = new u8[4 + entries + (entries * sizeof(float))];

        // Copy 'entries'
        ret[0] = entries;
        
        // Copy over tile sprite
        int i = 0;
        for(i = 1; i < 4; ++i) ret[i] = tileSprite[i-1];
        int offset = i;

        // Copy indices
        for(i = 0; i < entries; ++i) ret[i+offset] = indices[i];
        offset += i;

        // Copy values
        for(i = 0; i < entries * sizeof(float); ++i) ret[i+offset] = values[i];

        return ret;
    }

    void FromBytes(u8 * bytes) {
        // Retrive 'entries'
        entries = bytes[0];

        // Retrive tile sprite
        for(int i = 0; i < 3; ++i) tileSprite[i] = bytes[i+1];

        // Allocate tile data
        indices = (u8 *)malloc(entries);
        values = (u8 *)malloc(entries * sizeof(float));  // Allocate as if array of floats

        // Retrive tile data
        for(int i = 0; i < entries; ++i) indices[i] = bytes[i + 4];
        for(int i = 0; i < entries * sizeof(float); ++i) values[i] = bytes[i + 4 + entries];
    }

    // Returns the amount of bytes in the tile
    int GetSize() {
        return 4 + entries + (entries * sizeof(float));
    }

    void Unload() {
        // Free malloc'd data
        free(indices);
        free(values);
    }
};