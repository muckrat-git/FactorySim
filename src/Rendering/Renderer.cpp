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
    float windowRatio;          // Ratio of window.x to window.y
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
    RenderTexture2D renderBuffer;
    
    // Texture sheets
    Texture2D edgeMaskTex;
    Texture2D breakSheetTex;
    Texture2D breakMark;

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
    
    void Init(Player * playerPtr, cstr vertex, cstr fragment) {
        LoadTexmap();

        player = playerPtr;
        char * vertexShaderCode = LoadFileText(vertex);
        char * baseFragCode = LoadFileText(fragment);
        char * fragmentShaderCode = TextReplace(baseFragCode, "#tiledefs", genShaderFunctions().c_str());
        shader = LoadShaderFromMemory(vertexShaderCode, fragmentShaderCode);
        free(vertexShaderCode);
        free(fragmentShaderCode);
        free(baseFragCode);

        shaderWorldLoc = GetShaderLocation(shader, "world");
        shaderScaleLoc = GetShaderLocation(shader, "scale");
        shaderPlayerLoc = GetShaderLocation(shader, "playerPos");
        shaderWindowRLoc = GetShaderLocation(shader, "windowRatio");
        shaderEdgeMaskLoc = GetShaderLocation(shader, "edgeMask");
        shaderTexmapLoc = GetShaderLocation(shader, "textureMap");
        shaderGasmapLoc = GetShaderLocation(shader, "gasTextureMap");

        SetShaderValue(shader, GetShaderLocation(shader, "textureCount"), &tileCount, SHADER_UNIFORM_INT);
        SetShaderValue(shader, GetShaderLocation(shader, "gasCount"), &gasCount, SHADER_UNIFORM_INT);

        UpdateWindow(true);

        // Load texture masks
        edgeMaskTex = LoadTexture("resources/mask/case-sheet.png");
        breakSheetTex = LoadTexture("resources/ui/break-sheet.png");
        breakMark = LoadTexture("resources/ui/break-mark.png");
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
        windowRatio = window.x / window.y;

        // Update mouse pos
        mouse.x = GetMouseX();
        mouse.y = GetMouseY();

        if(IsWindowResized()) UpdateWindow();

        BeginTextureMode(renderBuffer);
        BeginShaderMode(shader);
        {
            SetShaderValue(shader, shaderScaleLoc, &viewSize, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, shaderWindowRLoc, &windowRatio, SHADER_UNIFORM_FLOAT);
            SetShaderValue(shader, shaderPlayerLoc, &player->position, SHADER_UNIFORM_VEC2);
            SetShaderValueTexture(shader, shaderWorldLoc, player->world->GetTexture());
            SetShaderValueTexture(shader, shaderEdgeMaskLoc, edgeMaskTex);
            SetShaderValueTexture(shader, shaderTexmapLoc, textureMap);
            SetShaderValueTexture(shader, shaderGasmapLoc, gasMap);

            DrawRectangle(0, 0, window.x, window.y, BLANK);
            DrawTextureFast(player->GetTexture(), {(window.x - scale) / 2, (window.y - scale) / 2, scale, scale}, WHITE);
        }
        EndShaderMode();

        // Calculate world space mouse position
        mouseWorldPos = ((GetRelative(mouse) * viewSize / vec2(2, windowRatio * 2)) + player->position).Int();

        // Update world interaction data
        player->world->hoverTile = mouseWorldPos;
        
        // Render user interface
        RenderGUI();

        // Get current player dig tile (avoid address boundry error)
        Vec2<int> currentDig = ivec2(-1,-1);
        if(player->digProgress != 0 && player->digQueueIndex >= 0 && player->digQueueIndex < player->digQueue.size()) 
            currentDig = player->digQueue[player->digQueueIndex];

        // Visualize player dig queue
        for(Vec2<int> tile : player->digQueue) {
            // Calculate dest rect
            Rectangle dest = {
                (tile.x - player->position.x) * (scale) + (window.x / 2), 
                (tile.y - player->position.y) * (scale) + (window.y / 2), 
                ceilf(scale), ceilf(scale)
            };

            if(tile == currentDig) {
                // Draw break animation
                DrawTexturePro(breakSheetTex, {floorf(player->digProgress * 29 / DIG_TIME) * 8, 0, 8, 8}, dest, {0}, 0, WHITE);
                //continue;
            }
            DrawTexturePro(breakMark, {0, 0, 16, 16}, dest, {0}, 0, (Color){255, 255, 255, 70});
        }

        EndTextureMode();
        
        BeginDrawing();
        DrawTextureFast(renderBuffer.texture, {0, 0, (float)GetRenderWidth(), (float)GetRenderHeight()}, WHITE, true);
        DrawFPS(5, 5);
        EndDrawing();
    }
};