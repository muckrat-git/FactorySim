#include <raylib.h>
#include <rlgl.h>
#include "World/World.cpp"
#include "MarchingSquares.cpp"
#include "World/Tiles.cpp"

#define TILESIZE 16

void DrawTextureFast(Texture2D tex1, Rectangle dest, Color tint) {
        float width = (float)tex1.width;
        float height = (float)tex1.height;
        
        float x = dest.x;
        float y = dest.y;

        Vector2 topLeft = {x, y};
        Vector2 topRight = {x + dest.width, y};
        Vector2 bottomLeft = {x, y + dest.height};
        Vector2 bottomRight = {x + dest.width, y + dest.height};

        rlSetTexture(tex1.id);
        rlBegin(RL_QUADS);

            rlColor4ub(tint.r, tint.g, tint.b, tint.a);
            rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

            // Top-left corner for texture and quad
            rlTexCoord2f(0, 0);
            rlVertex2f(topLeft.x, topLeft.y);

            // Bottom-left corner for texture and quad
            rlTexCoord2f(0, 1);
            rlVertex2f(bottomLeft.x, bottomLeft.y);

            // Bottom-right corner for texture and quad
            rlTexCoord2f(1, 1);
            rlVertex2f(bottomRight.x, bottomRight.y);

            // Top-right corner for texture and quad
            rlTexCoord2f(1, 0);
            rlVertex2f(topRight.x, topRight.y);

        rlEnd();
        rlSetTexture(0);
}


namespace Renderer {
    float viewSize = 70;    // The size of the player viewport in tiles
    float scale;            // Camera-Tile scale (pixels per tile)
    Vec2<float> window;
    Rect<int> viewport;     // Tile space bounds
    Vec2<int> windowOffset;

    Shader shader;
    int shaderWorldLoc;
    int shaderScaleLoc;
    int shaderPlayerLoc;
    int shaderWindowRLoc;
    int shaderEdgeMaskLoc;
    int shaderTexmapLoc;

    // World pointer
    World * world;
    Texture2D stone, edgeMaskTex;
    RenderTexture2D textureMask;

    void UpdateWindow(bool first = false) {
        // Get window bounds
        window.x = GetRenderWidth();
        window.y = GetRenderHeight();

        if(!first)
            UnloadRenderTexture(textureMask);
        textureMask = LoadRenderTexture(window.x, window.y);
    }
    
    void Init(World * worldPtr, Shader nshader) {
        LoadTexmap();

        world = worldPtr;
        shader = nshader;

        shaderWorldLoc = GetShaderLocation(shader, "world");
        shaderScaleLoc = GetShaderLocation(shader, "scale");
        shaderPlayerLoc = GetShaderLocation(shader, "playerPos");
        shaderWindowRLoc = GetShaderLocation(shader, "windowRatio");
        shaderEdgeMaskLoc = GetShaderLocation(shader, "edgeMask");
        shaderTexmapLoc = GetShaderLocation(shader, "textureMap");

        SetShaderValue(shader, GetShaderLocation(shader, "textureCount"), &tile_count, SHADER_UNIFORM_INT);

        UpdateWindow(true);
        stone = LoadTexture("resources/tiles/stone.png");
        edgeMaskTex = LoadTexture("resources/mask/case-sheet.png");
    }

    void RenderWorld() {
        // Calculate screen bounds
        viewSize = world->player->zoom;
        scale = window.x / viewSize;
        float ratio = window.x / window.y;

        if(IsWindowResized()) UpdateWindow();

        BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(shader);
        {
            SetShaderValue(shader, shaderScaleLoc, &viewSize, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, shaderWindowRLoc, &ratio, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, shaderPlayerLoc, &world->player->position, SHADER_UNIFORM_VEC2);
            SetShaderValueTexture(shader, shaderWorldLoc, world->getTexture());
            SetShaderValueTexture(shader, shaderEdgeMaskLoc, edgeMaskTex);
            SetShaderValueTexture(shader, shaderTexmapLoc, textureMap);

            DrawRectangle(0, 0, window.x, window.y, WHITE);
        }
        EndShaderMode();
        DrawFPS(5, 5);
        EndDrawing();
    }
};