#include <raylib.h>

#define TILESIZE 16

int tile_count = 0;
Texture2D textureMap;

void LoadTexmap() {
    Image images[255];
    images[tile_count++] = LoadImage("resources/tiles/ground.png");
    images[tile_count++] = LoadImage("resources/tiles/stone.png");

    Image imgmap = GenImageColor(TILESIZE * tile_count, TILESIZE, WHITE);
    for(int i = 0; i < tile_count; ++i) {
        ImageDraw(&imgmap, images[i], {0, 0, (float)images[i].width, (float)images[i].height}, {(float)(TILESIZE * i), 0, TILESIZE, TILESIZE}, WHITE);
        UnloadImage(images[i]);
    }
    textureMap = LoadTextureFromImage(imgmap);
    ExportImage(imgmap, "texmap.png");
    UnloadImage(imgmap);
}