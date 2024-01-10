#pragma once

#include <raylib.h>
#include "Other/Types.cpp"

#define TILESIZE 16

int tile_count = 0;
int gas_count = 0;
Texture2D gasMap;
Texture2D textureMap;

enum TileType {
    TILE_EMPTY,
    TILE_STONE,
    TILE_SILT,
    TILE_TITANIUM,
    TILE_COPPER
};

enum GasType {
    GAS_EMPTY,
    GAS_SOLID,
    GAS_OXYGEN
};

const GasType defaultGasses[] = {
    GAS_EMPTY,
    GAS_SOLID,
    GAS_SOLID,
    GAS_SOLID,
    GAS_SOLID
};

const float defaultGasMass[] = {
    0,
    0,
    2000
};

u8 getGasAlpha(float mass, GasType gas) {
    float d = defaultGasMass[gas];
    if(d == 0) return 0;
    d = Normalize(mass, 0, d);
    d = Clamp(d, 0, 1);
    return (u8)(d * 255);
}

void LoadTexmap() {
    Image images[255];
    images[tile_count++] = LoadImage("resources/tiles/ground.png");
    images[tile_count++] = LoadImage("resources/tiles/stone.png");
    images[tile_count++] = LoadImage("resources/tiles/silt.png");
    images[tile_count++] = LoadImage("resources/tiles/titanium.png");
    images[tile_count++] = LoadImage("resources/tiles/copper.png");

    Image imgmap = GenImageColor(TILESIZE * tile_count, TILESIZE, WHITE);
    for(int i = 0; i < tile_count; ++i) {
        ImageDraw(&imgmap, images[i], {0, 0, (float)images[i].width, (float)images[i].height}, {(float)(TILESIZE * i), 0, TILESIZE, TILESIZE}, WHITE);
        UnloadImage(images[i]);
    }
    textureMap = LoadTextureFromImage(imgmap);
    ExportImage(imgmap, "texmap.png");
    UnloadImage(imgmap);

    // Generate gas map
    gas_count = 3;
    imgmap = GenImageColor(gas_count, 1, BLANK);
    
    ImageDrawPixel(&imgmap, 0, 0, BLANK);
    ImageDrawPixel(&imgmap, 0, 0, BLANK);
    ImageDrawPixel(&imgmap, 1, 0, (Color){193, 247, 255, 255});

    gasMap = LoadTextureFromImage(imgmap);
    UnloadImage(imgmap);
}