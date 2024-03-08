#include "Other/Types.cpp"
#include "Rendering/Renderer.cpp"

enum EntityType {
    ENTITY_TYPE_ITEM
};

class Entity {
    public:
    EntityType type;
    Texture2D * texture;
    float scale;
    Vec2<float> position;

    void Render() {
        // Calculate dest rect
        Rectangle dest = {
            (position.x - Renderer::player->position.x) * (Renderer::scale) + (Renderer::window.x / 2) - 0.5f, 
            (position.y - Renderer::player->position.y) * (Renderer::scale) + (Renderer::window.y / 2) - 0.5f, 
            ceilf(Renderer::scale) * scale, ceilf(Renderer::scale) * scale
        };
        DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height}, dest, {Renderer::scale / 2, Renderer::scale / 2}, 0, WHITE);
    }

    void Update(float deltaT) {

    }
    void TileUpdate() {

    }
};

vector<Entity> entities;

void Renderer::RenderEntities() {
    for(int i = 0; i < entities.size(); ++i) {
        entities[i].Render();
    }
}