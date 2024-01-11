#pragma once

#include <raylib.h>
#include <rlgl.h>
#include "World/Player.cpp"
#include "Tiles/Tile.cpp"

#define TILESIZE 16

void DrawTextureFast(Texture2D tex1, Rectangle dest, Color tint, bool flipY = false) {
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
            rlTexCoord2f(0, flipY ? 1 : 0);
            rlVertex2f(topLeft.x, topLeft.y);

            // Bottom-left corner for texture and quad
            rlTexCoord2f(0, flipY ? 0 : 1);
            rlVertex2f(bottomLeft.x, bottomLeft.y);

            // Bottom-right corner for texture and quad
            rlTexCoord2f(1, flipY ? 0 : 1);
            rlVertex2f(bottomRight.x, bottomRight.y);

            // Top-right corner for texture and quad
            rlTexCoord2f(1, flipY ? 1 : 0);
            rlVertex2f(topRight.x, topRight.y);

        rlEnd();
        rlSetTexture(0);
}

namespace Renderer {
    float viewSize = 70;        // The size of the player viewport in tiles
    float scale;                // Camera-Tile scale (pixels per tile)
    float ratio;                // Ratio of window.x to window.y
    Vec2<float> window;         // Window width and height
    Vec2<float> mouse;          // Mouse position
    Vec2<int> mouseWorldPos;    // World space mouse position

    // Shader and shader variable locations
    Shader shader;
    int shaderWorldLoc;
    int shaderScaleLoc;
    int shaderPlayerLoc;
    int shaderWindowRLoc;
    int shaderEdgeMaskLoc;
    int shaderTexmapLoc;
    int shaderGasmapLoc;

    // Player pointer
    Player * player;
    Texture2D edgeMaskTex;
    RenderTexture2D renderBuffer;

    void UpdateWindow(bool first = false) {
        // Get window bounds
        window.x = GetRenderWidth();
        window.y = GetRenderHeight();

        if(!first)
            UnloadRenderTexture(renderBuffer);
        renderBuffer = LoadRenderTexture(window.x, window.y);

        window.x = renderBuffer.texture.width;
        window.y = renderBuffer.texture.height;
    }
    
    void Init(Player * playerPtr, Shader nshader) {
        LoadTexmap();

        player = playerPtr;
        shader = nshader;

        shaderWorldLoc = GetShaderLocation(shader, "world");
        shaderScaleLoc = GetShaderLocation(shader, "scale");
        shaderPlayerLoc = GetShaderLocation(shader, "playerPos");
        shaderWindowRLoc = GetShaderLocation(shader, "windowRatio");
        shaderEdgeMaskLoc = GetShaderLocation(shader, "edgeMask");
        shaderTexmapLoc = GetShaderLocation(shader, "textureMap");
        shaderGasmapLoc = GetShaderLocation(shader, "gasTextureMap");

        SetShaderValue(shader, GetShaderLocation(shader, "textureCount"), &tile_count, SHADER_UNIFORM_INT);
        SetShaderValue(shader, GetShaderLocation(shader, "gasCount"), &gas_count, SHADER_UNIFORM_INT);

        UpdateWindow(true);
        edgeMaskTex = LoadTexture("resources/mask/case-sheet.png");
    }

    // Prototype RenderGUI
    void RenderGUI();

    // Returns vec2 from -1 to 1 of the position relative to the window
    Vec2<float> GetRelative(Vec2<float> position) {
        return position * 2 / window - vec2(1);
    }

    void Render() {
        // Calculate screen bounds
        viewSize = player->zoom;
        scale = window.x / viewSize;
        ratio = window.x / window.y;

        // Update mouse pos
        mouse.x = GetMouseX();
        mouse.y = GetMouseY();

        if(IsWindowResized()) UpdateWindow();

        BeginTextureMode(renderBuffer);
        BeginShaderMode(shader);
        {
            SetShaderValue(shader, shaderScaleLoc, &viewSize, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, shaderWindowRLoc, &ratio, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, shaderPlayerLoc, &player->position, SHADER_UNIFORM_VEC2);
            SetShaderValueTexture(shader, shaderWorldLoc, player->world->getTexture());
            SetShaderValueTexture(shader, shaderEdgeMaskLoc, edgeMaskTex);
            SetShaderValueTexture(shader, shaderTexmapLoc, textureMap);
            SetShaderValueTexture(shader, shaderGasmapLoc, gasMap);

            DrawRectangle(0, 0, window.x, window.y, BLANK);
            DrawTextureFast(player->GetTexture(), {(window.x - scale) / 2, (window.y - scale) / 2, scale, scale}, WHITE);
        }
        EndShaderMode();

        // Render user interface
        RenderGUI();
        
        // Calculate world space mouse position
        mouseWorldPos = ((GetRelative(mouse) * viewSize / vec2(2, ratio * 2)) + player->position).Int();

        EndTextureMode();
        
        BeginDrawing();
        DrawTextureFast(renderBuffer.texture, {0, 0, (float)GetRenderWidth(), (float)GetRenderHeight()}, WHITE, true);
        DrawFPS(5, 5);
        EndDrawing();
    }
};