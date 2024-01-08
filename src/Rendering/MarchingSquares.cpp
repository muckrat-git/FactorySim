#include <raylib.h>
#include "World/World.cpp"

const Rectangle tileMaskRect = {0, 0, 16, 16};
Texture2D tileMasks[16];

u8 MarchTile(Vec2<int> pos, World * world) {
    return (
        (world->isair(pos.x, pos.y - 1) << 3) | 
        (world->isair(pos.x + 1, pos.y) << 2) | 
        (world->isair(pos.x, pos.y + 1) << 1) | 
        (world->isair(pos.x - 1, pos.y))
    );
}