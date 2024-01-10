#include <raylib.h>
#include <iostream>

#include <string.h>

#include "Other/Types.cpp"
#include "Rendering/Renderer.cpp"
#include "World/World.cpp"

using namespace std;

World world;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(700, 400, "game");
    
    // Init world
    Player player;
    world = World(0);
    player.world = &world;
    player.position = {WORLD_SIZE / 2, WORLD_SIZE / 2};

    Renderer::Init(&player, LoadShader("src/Shaders/vert.glsl", "src/Shaders/frag.glsl"));

    for(int i = 0; i < 16; ++i) {
        tileMasks[i] = LoadTexture(("resources/mask/case" + to_string(i) + ".png").c_str());
    }

    while(!WindowShouldClose()) {
        Renderer::RenderWorld();

        player.Update(GetFrameTime());
    }

    world.Unload();

    CloseWindow();
}