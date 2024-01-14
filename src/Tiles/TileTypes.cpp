#pragma once

#include <raylib.h>
#include "Other/Types.cpp"
#include "TilePrefabs.cpp"

#define TILESIZE 16

int tileCount = 0;
int gasCount = 0;
Texture2D gasMap;
Texture2D textureMap;

enum GasType {
    GAS_EMPTY,
    GAS_SOLID,
    GAS_OXYGEN
};

const float defaultGasMass[] = {
    0,
    0,
    2000
};

cstr gasNames[] = {
    "Void",
    "Void",
    "Oxygen"
};

GasType getDefaultGas(int id) {
    return tilePrefabs[id].solid ? GAS_SOLID : GAS_OXYGEN;
}

u8 getGasAlpha(float mass, GasType gas) {
    float d = defaultGasMass[gas];
    if(d == 0) return 0;
    d = Normalize(mass, 0, d);
    d = Clamp(d, 0, 1);
    return (u8)(d * 255);
}

// Generate constant functions for tile information on the shader
string genShaderFunctions() {
    // Generate 'IsTransparent'
    string out = "bool IsTransparent(int index) {bool lookup[] = bool[" + to_string(tileCount) + "](";
    for(int i = 0; i < tileCount; ++i) {
        out += tilePrefabs[i].transparent ? "true" : "false";
        if(i != tileCount - 1) out += ",";
    }
    out += "); return lookup[index];}\n";

    // Generate 'IsSolid'
    out += "bool IsSolid(int index) {bool lookup[] = bool[" + to_string(tileCount) + "](";
    for(int i = 0; i < tileCount; ++i) {
        out += tilePrefabs[i].solid ? "true" : "false";
        if(i != tileCount - 1) out += ",";
    }
    out += "); return lookup[index];}\n";

    // Generate shorthands
    out += "bool transparent(vec2 pos) {return IsTransparent(int(texture(world, pos / WORLDSIZE).x * 255 + 0.5));}\n";
    out += "bool transparent(float x, float y) {return transparent(vec2(x, y));}\n";
    out += "bool solid(vec2 pos) {return IsSolid(int(texture(world, pos / WORLDSIZE).x * 255 + 0.5));}\n";
    out += "bool solid(float x, float y) {return solid(vec2(x, y));}\n";
    return out;
}

void LoadTexmap() {
    // Calculate tileCount
    tileCount = sizeof(tilePrefabs) / sizeof(TilePrefab);

    // Allocate image buffers
    Image images[tileCount];
    Image imgmap = GenImageColor(TILESIZE * tileCount, TILESIZE, BLANK);

    // Load texture onto image map
    for(int i = 0; i < tileCount; ++i) {
        images[i] = LoadImage(tilePrefabs[i].texture);
        ImageDraw(&imgmap, images[i], {0, 0, (float)images[i].width, (float)images[i].height}, {(float)(TILESIZE * i), 0, TILESIZE, TILESIZE}, WHITE);
        UnloadImage(images[i]);
    }

    // Load texture map and unload buffers
    textureMap = LoadTextureFromImage(imgmap);
    ExportImage(imgmap, "texmap.png");
    UnloadImage(imgmap);

    // Generate gas map
    gasCount = 3;
    imgmap = GenImageColor(gasCount, 1, BLANK);
    
    ImageDrawPixel(&imgmap, 0, 0, BLANK);
    ImageDrawPixel(&imgmap, 0, 0, BLANK);
    ImageDrawPixel(&imgmap, 1, 0, (Color){193, 247, 255, 255});

    gasMap = LoadTextureFromImage(imgmap);
    UnloadImage(imgmap);
}