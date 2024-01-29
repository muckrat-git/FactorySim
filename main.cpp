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
    player.position = {WORLD_SIZE / 2 + 0.1, WORLD_SIZE / 2 + 0.1};

    // Init renderer and gui
    Renderer::Init(&player, "src/Shaders/vert.glsl", "src/Shaders/frag.glsl");
    GUI::Load();

    // Load structure
    int size;
    u8 * bytes = LoadFileData("debug.sav", &size);
    Structure s;
    s.Deserialize(bytes);
    free(bytes);

    // Paste structure into world
    for(int x = 0; x < s.width; ++x) {
        for(int y = 0; y < s.height; ++y) {
            player.world->Set(x + WORLD_SIZE / 2 - 20, y + WORLD_SIZE / 2 - 14, s.get(x, y).Deserialize());
        }
    }

    // Unload
    s.Unload();

    world.Update();

    world.StartUpdater();

    SetExitKey(0);

    float avg = 300;

    while(!WindowShouldClose()) {
        const float deltaT = GetFrameTime();

        // Render everything
        Renderer::Render();

        // Perform updates
        player.Update(deltaT);
        GUI::Update();

        player.world->CheckUpdater(deltaT);
    }

    world.Unload();
    GUI::Unload();

    CloseWindow();
}