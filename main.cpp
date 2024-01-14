#include <raylib.h>
#include <iostream>

#include <string.h>

#include "Other/Types.cpp"
#include "Rendering/Renderer.cpp"
#include "Rendering/GUI.cpp"
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

    // Init renderer and gui
    Renderer::Init(&player, "src/Shaders/vert.glsl", "src/Shaders/frag.glsl");
    GUI::Load();

    while(!WindowShouldClose()) {
        // Render everything
        Renderer::Render();

        // Perform updates
        player.Update(GetFrameTime());
        GUI::Update();
    }

    world.Unload();
    GUI::Unload();

    CloseWindow();
}