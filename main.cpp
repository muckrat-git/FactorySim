#include <raylib.h>
#include <iostream>

#include "Other/Types.cpp"
#include "Rendering/Renderer.cpp"
#include "World/World.cpp"

using namespace std;

Player player;
World world;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(700, 400, "game");
    //SetTargetFPS(120);
    
    // Init world
    world = World(0);
    world.player = &player;
    player.position = {WORLD_SIZE / 2, WORLD_SIZE / 2};

    Renderer::Init(&world, LoadShader("src/Shaders/vert.glsl", "src/Shaders/frag.glsl"));

    for(int i = 0; i < 16; ++i) {
        tileMasks[i] = LoadTexture(("resources/mask/case" + to_string(i) + ".png").c_str());
    }

    while(!WindowShouldClose()) {
        Renderer::RenderWorld();

        player.Update(GetFrameTime());
    }

    CloseWindow();
}