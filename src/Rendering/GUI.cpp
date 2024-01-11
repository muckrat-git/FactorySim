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
        bool (*update)();
        void (*render)();
    };

    const int actions = 5;              // The amount of tabs
    ActionTab tabs[actions];            // Array of action tab buttons
    int activeTab = -1;                 // The index of the active tab (-1 means none)
    Color highlight;                    // The color of the mouse tile highlight
    AbsRect<int> absSelection;          // The coords of the player's selection (unordered)
    Rect<int> selection;                // The players rectangular selection (ordered)
    Texture2D tabBackground;

    // Rendering function for building tab
    void _renderBuildTab() {

    }
    void _updateBuildTab() {}

    // Loads all GUI elements
    void Load() {
        tabBackground = LoadTexture("resources/ui/tab.png");

        tabs[ACTION_TAB_BUILDING] = {
            .button = LoadTexture("resources/ui/building_tab.png"),
            .highlight = (Color){230, 230, 230, 70}
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
            .highlight = (Color){40, 80, 255, 90}
        };
    }

    // Render all GUI elements
    void Render() {
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

        // Calculate absolute texture size
        const float textureSize = ratio * (float)tabs[0].button.width;

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

        // Reset highlight color
        highlight = BLANK;

        // Early return if no tab selected
        if(activeTab == -1) return;

        // Draw bg
        DrawTextureFast(tabBackground, {window.x - ratio - (actions * textureSize * 1.2f), window.y - textureSize * 11, ((actions-1) * textureSize * 1.2f) + textureSize, textureSize * 9.5f}, WHITE);

        // Render selected tab
        if(tabs[activeTab].render != nullptr) tabs[activeTab].render();

        // Set highlight color
        highlight = tabs[activeTab].highlight;
    }
    
    void Update() {
        // Update active tab (early return if tab chooses)
        if(tabs[activeTab].render != nullptr && tabs[activeTab].update()) return;

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
                if(CheckCollisionPointRec(GetMousePosition(), bounds)) {
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
                    for(int x = selection.position.x; x < selection.position.x + selection.size.x; ++x) {
                        for(int y = selection.position.y; y < selection.position.y + selection.size.y; ++y) {
                            player->world->set(x, y, DefaultTile(TILE_DENSE_WALL));
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