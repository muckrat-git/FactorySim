#pragma once

#include <raylib.h>
#include "Other/Types.cpp"
#include "TilePrefabs.cpp"

#define TILESIZE 16

int tileCount = 0;
int gasCount = 0;
Texture2D gasMap;
Texture2D textureMap;

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

    // Generate 'IsSmall'
    out += "bool IsSmall(int index) {bool lookup[] = bool[" + to_string(tileCount) + "](";
    for(int i = 0; i < tileCount; ++i) {
        out += tilePrefabs[i].small ? "true" : "false";
        if(i != tileCount - 1) out += ",";
    }
    out += "); return lookup[index];}\n";

    // Generate shorthands
    out += "bool transparent(vec2 pos) {return IsTransparent(int(texture(world, pos / WORLDSIZE).x * 255 + 0.5));}\n";
    out += "bool transparent(float x, float y) {return transparent(vec2(x, y));}\n";
    out += "bool solid(vec2 pos) {return IsSolid(int(texture(world, pos / WORLDSIZE).x * 255 + 0.5));}\n";
    out += "bool solid(float x, float y) {return solid(vec2(x, y));}\n";
    out += "bool small(vec2 pos) {return IsSmall(int(texture(world, pos / WORLDSIZE).x * 255 + 0.5));}\n";
    out += "bool small(float x, float y) {return small(vec2(x, y));}\n";
    out += "bool isair(vec2 pos) {return (int(texture(world, pos / WORLDSIZE).x * 255 + 0.5)) == 0;}\n";
    out += "bool isair(float x, float y) {return isair(vec2(x, y));}\n";
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
    gasCount = 4;
    imgmap = GenImageColor(gasCount, 1, BLANK);
    
    ImageDrawPixel(&imgmap, 0, 0, BLANK);
    ImageDrawPixel(&imgmap, 1, 0, BLANK);
    ImageDrawPixel(&imgmap, 2, 0, (Color){193, 247, 255, 255});
    ImageDrawPixel(&imgmap, 3, 0, (Color){35, 35, 35, 255});

    gasMap = LoadTextureFromImage(imgmap);
    UnloadImage(imgmap);
}