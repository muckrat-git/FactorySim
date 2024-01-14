#include <raylib.h>
#include "Other/Types.cpp"
#include "World/Structure.cpp"
#include "Renderer.cpp"

using namespace Renderer;

// GUI namespace contains all GUI rendering code
namespace GUI {
    enum ACTION_TAB {
        ACTION_TAB_BUILDING,
        ACTION_TAB_LOGIC,
        ACTION_TAB_POWER,
        ACTION_TAB_TRANSPORTATION,
        ACTION_TAB_STRUCTURE
    };

    struct ActionTab {
        Texture2D button;
        Color highlight = BLANK;
        Color border = BLANK;
        void (*update)();
        void (*render)();

        int hover = 0; // Currently hovered action tab button
    };

    const int actions = 5;              // The amount of tabs
    ActionTab tabs[actions];            // Array of action tab buttons
    int activeTab = -1;                 // The index of the active tab (-1 means none)
    Color highlight, border;            // The color of the mouse tile highlight and selection border
    AbsRect<int> absSelection;          // The coords of the player's selection (unordered)
    Rect<int> selection;                // The players rectangular selection (ordered)
    Texture2D tabBackground;
    Rectangle tabBounds;
    Texture2D cursor;

    // Rendering function for building tab
    void _renderBuildTab() {
        const int rowSize = 5; // Tiles per row to display

        tabs[activeTab].hover = -1;

        // Iterate over tiles
        for(int i = 0; i < tileCount; ++i) {
            const float size = tabBounds.width / (rowSize * 1.5 + 0.5);
            const float margin = size/2;
            const float x = i % rowSize;
            const float y = i / rowSize;

            // Calculate position and size
            Rectangle rect = {
                tabBounds.x + x * (size + margin) + margin,
                tabBounds.y + y * (size + margin) + margin,
                size, size
            };

            // Draw tile from texture map
            Color color = (Color){255, 255, 255, 255};
            if(CheckCollisionPointRec(mouse, rect)) {
                tabs[activeTab].hover = i;
                color.a = 255;
            }
            else color.a = 200;
            DrawTexturePro(textureMap, {(float)i * TILESIZE, 0, TILESIZE, TILESIZE}, rect, {0, 0}, 0, color);
        }

        if(player->heldTile != -1) {
            // Draw tiles in selection
            if(selection.size != ivec2(0)) {
                for(int x = selection.position.x; x < selection.position.x + selection.size.x; ++x) {
                    for(int y = selection.position.y; y < selection.position.y + selection.size.y; ++y) {
                        DrawTexturePro(
                            textureMap, 
                            {(float)player->heldTile * TILESIZE, 0, TILESIZE, TILESIZE}, 
                            {
                            (x - player->position.x) * scale + (window.x / 2), 
                            (y - player->position.y) * scale + (window.y / 2), 
                            scale, scale
                            }, {0, 0}, 0, (Color){200, 200, 200, 150}
                        );
                    }
                }
            }
            else {
                // Draw held tile
                DrawTexturePro(
                    textureMap, 
                    {(float)player->heldTile * TILESIZE, 0, TILESIZE, TILESIZE}, 
                    {
                        (mouseWorldPos.x - player->position.x) * scale + (window.x / 2), 
                        (mouseWorldPos.y - player->position.y) * scale + (window.y / 2), 
                        scale, scale
                    }, {0, 0}, 0, (Color){200, 200, 200, 150}
                );
            }
        }

        if(tabs[activeTab].hover == -1) return;

        // Display tile name
        cstr name = tilePrefabs[tabs[activeTab].hover].name;
        const int width = MeasureText(name, ratio * 16);
        DrawText(name, tabBounds.x + tabBounds.width - ratio * 8 - width, tabBounds.y + tabBounds.height - ratio * 24, ratio * 16, WHITE);
    }
    void _updateBuildTab() {
        if(tabs[activeTab].hover != -1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if(player->heldTile == tabs[activeTab].hover) player->heldTile = -1;
            else player->heldTile = tabs[activeTab].hover;
        }
    }

    // Loads all GUI elements
    void Load() {
        tabBackground = LoadTexture("resources/ui/tab.png");

        tabs[ACTION_TAB_BUILDING] = {
            .button = LoadTexture("resources/ui/building_tab.png"),
            .highlight = (Color){255, 255, 255, 20},
            .border = (Color){253, 249, 0, 125},
            .update = &_updateBuildTab,
            .render = &_renderBuildTab
        };
        tabs[ACTION_TAB_LOGIC] = {
            .button = LoadTexture("resources/ui/logic_tab.png")
        };
        tabs[ACTION_TAB_POWER] = {
            .button = LoadTexture("resources/ui/power_tab.png")
        };
        tabs[ACTION_TAB_TRANSPORTATION] = {
            .button = LoadTexture("resources/ui/transportation_tab.png")
        };
        tabs[ACTION_TAB_STRUCTURE] = {
            .button = LoadTexture("resources/ui/structure_tab.png"),
            .highlight = (Color){40, 80, 255, 50},
            .border = (Color){40, 80, 255, 100}
        };
    }
    void Unload() {
        UnloadTexture(tabBackground);
        for(int i = 0; i < actions; ++i) UnloadTexture(tabs[i].button);
    }

    // Render all GUI elements
    void Render() {
        // Calculate absolute texture size
        const float textureSize = ratio * (float)tabs[0].button.width;

        // Draw tile highlight
        DrawRectangle(
            (mouseWorldPos.x - player->position.x) * scale + (window.x / 2), 
            (mouseWorldPos.y - player->position.y) * scale + (window.y / 2), 
            scale, scale, highlight
        );

        // Draw selection zone
        DrawRectangle(
            (selection.position.x - player->position.x) * scale + (window.x / 2), 
            (selection.position.y - player->position.y) * scale + (window.y / 2), 
            selection.size.x * scale, selection.size.y * scale, highlight
        );

        // Draw selection border
        DrawRectangleLinesEx(
            {(selection.position.x - player->position.x) * scale + (window.x / 2), 
            (selection.position.y - player->position.y) * scale + (window.y / 2), 
            selection.size.x * scale, selection.size.y * scale}, scale / 16.0f, border
        );

        Tile hovered = player->world->get(mouseWorldPos);
        DrawText(tilePrefabs[hovered.sprite.index].name, mouse.x + 16, mouse.y, 8, WHITE);
        if(hovered.sprite.gas != GAS_SOLID) {
            DrawText((string(gasNames[hovered.sprite.gas]) + "  " + to_string(int(hovered.data[TILEDATA_GAS_MASS])) + " G").c_str(), mouse.x + 16, mouse.y + 10, 8, WHITE);
        }

        // Get source rectangle for drawing
        const Rectangle source = {
            0, 0,
            (float)tabs[0].button.width, (float)tabs[0].button.height
        };
        
        // Render all the buttons
        for(int i = 1; i < actions + 1; ++i) {
            // Get real index
            const int index = actions - i;

            // Calculate button bounds
            const Rectangle bounds = {
              window.x - ratio - (i * textureSize * 1.2f), window.y - textureSize * 1.2f,
              textureSize, textureSize
            };

            // Get button alpha
            const u8 alpha = (CheckCollisionPointRec(GetMousePosition(), bounds) ? 230 : 180);
            const u8 shade = index == activeTab ? 255 : 200;
            
            DrawTexturePro(tabs[index].button, source, bounds, {0, 0}, 0, (Color){shade, shade, shade, alpha});
        }
 
        // Reset highlight and border color
        highlight = BLANK;
        border = BLANK;

        // Early return if no tab selected
        if(activeTab == -1) return;

        // Draw bg
        tabBounds = {window.x - ratio - (actions * textureSize * 1.2f), window.y - textureSize * 11, ((actions-1) * textureSize * 1.2f) + textureSize, textureSize * 9.5f};
        DrawTextureFast(tabBackground, tabBounds, WHITE);

        // Render selected tab
        if(tabs[activeTab].render != nullptr) tabs[activeTab].render();

        // Set highlight and border color
        highlight = tabs[activeTab].highlight;
        border = tabs[activeTab].border;
    }
    
    void Update() {
        // Update active tab
        if(activeTab != -1 && tabs[activeTab].update != nullptr) tabs[activeTab].update();

        // Ignore other mouse actions if tab is hovered
        if(CheckCollisionPointRec(mouse, tabBounds)) {
            highlight.a = 0;
            return;
        }

        // Calculate absolute texture size
        const float textureSize = ratio * (float)tabs[0].button.width;

        // Right click action
        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            switch(activeTab) {
                case ACTION_TAB_STRUCTURE:
                {
                    // Load structure
                    int size;
                    u8 * bytes = LoadFileData("debug.sav", &size);
                    Structure s;
                    s.Deserialize(bytes);
                    free(bytes);

                    // Paste structure into world
                    for(int x = 0; x < s.width; ++x) {
                        for(int y = 0; y < s.height; ++y) {
                            player->world->set(x + mouseWorldPos.x, y + mouseWorldPos.y, s.get(x, y).Deserialize());
                        }
                    }

                    // Update world
                    player->world->update();

                    // Unload
                    s.Unload();
                    return;
                }
                default:
                    return;
            }
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Start selection
            absSelection.a = mouseWorldPos;
        }

        // Set secondary selection coord if mouse is down and selection is valid
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && absSelection.a != ivec2(0, 0)) {
            absSelection.b = mouseWorldPos;

            // Calculate ordered selection
            selection.position.x = min(absSelection.a.x, absSelection.b.x);
            selection.position.y = min(absSelection.a.y, absSelection.b.y);
            selection.size.x = max(absSelection.a.x, absSelection.b.x) - selection.position.x + 1;
            selection.size.y = max(absSelection.a.y, absSelection.b.y) - selection.position.y + 1;
        }
        
        // Process and clear selection if mouse released
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            // Find hovered button
            for(int i = 1; i < actions + 1; ++i) {
                // Get real index
                const int index = actions - i;

                // Calculate button bounds
                const Rectangle bounds = {
                    window.x - ratio - (i * textureSize * 1.2f), window.y - textureSize * 1.2f,
                    textureSize, textureSize
                };

                // Check if mouse over
                if(CheckCollisionPointRec(mouse, bounds)) {
                    if(index == activeTab)  activeTab = -1;
                    else                    activeTab = index;
                    
                    // Reset selection
                    selection = {0};
                    return;
                }
            }

            switch(activeTab) {
                case ACTION_TAB_STRUCTURE:
                {
                    // Allocate structure data
                    Structure s;
                    s.width = selection.size.x;
                    s.height = selection.size.y;
                    s.tiles = new SerializedTile[s.width * s.height];

                    // Copy structure tiles
                    for(int x = selection.position.x; x < selection.position.x + selection.size.x; ++x) {
                        for(int y = selection.position.y; y < selection.position.y + selection.size.y; ++y) {
                            s.set(x - selection.position.x, y - selection.position.y, player->world->get(x, y));
                        }
                    }

                    // Download structure
                    int size;
                    u8 * bytes = s.Serialize(&size);
                    SaveFileData("debug.sav", bytes, size);
                
                    // Unload structure
                    free(bytes);
                    s.Unload();

                    break;
                }
                case ACTION_TAB_BUILDING:
                {
                    // Skip if no held tile
                    if(player->heldTile == -1) break;

                    // Fill in selection
                    for(int x = selection.position.x; x < selection.position.x + selection.size.x; ++x) {
                        for(int y = selection.position.y; y < selection.position.y + selection.size.y; ++y) {
                            player->world->set(x, y, DefaultTile((TileType)player->heldTile));
                        }
                    }
                    player->world->update();
                    break;
                }
                default:
                    break;
            }

            selection = {ivec2(0), ivec2(0)};
        }
    }
};

void Renderer::RenderGUI() {
    GUI::Render();
}