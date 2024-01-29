#include "Other/Types.cpp"
#include "World/World.cpp"

#include <vector>

using namespace std;

#define DIG_PER_SECOND 7.0f
#define DIG_TIME (1.0f/DIG_PER_SECOND)

class Player {
    private:
    const int texFrames = 8;
    
    RenderTexture2D renderTex;
    
    public:
    const int texSize = 16;

    World * world;

    const float maxSpeed = 10;
    float speed = 0;

    float zoom = 40;        // The viewport zoom
    float zoomVelocity = 0; // Viewport zoom increment factor

    float rotation = 0;
    Vec2<float> movementAxis = {0, 0}; // Vector representing movement input
    Vec2<float> position = {0, 0};

    int heldTile = -1;
    u8 heldRotation = 0;

    vector<Vec2<int>> digQueue;
    int digQueueIndex;
    float digProgress = 0;

    Texture2D texture;

    Player() {
        renderTex = LoadRenderTexture(texSize, texSize);
        texture = LoadTexture("resources/entities/player0.png");
    }

    void Update(float deltaT) {
        zoomVelocity -= GetMouseWheelMove() * deltaT * 20;
        zoom += zoomVelocity;
        zoomVelocity = Lerp(zoomVelocity, 0, deltaT * 10);
        if(abs(zoomVelocity) < deltaT * 5) zoomVelocity = 0;
        
        // Clamp zoom and round zoom to avoid rendering bugs
        if(zoom < 10) zoom = 10;

        Vec2<float> newAxis;
        newAxis.x = IsKeyDown(KEY_D);
        newAxis.x -= IsKeyDown(KEY_A);
        newAxis.y = IsKeyDown(KEY_S);
        newAxis.y -= IsKeyDown(KEY_W);

        if(IsKeyPressed(KEY_O)) ++heldRotation;

        if(heldTile != -1) {
            if(tilePrefabs[heldTile].rotation == 0) heldRotation = 0;
            else if(tilePrefabs[heldTile].rotation == 1) heldRotation = ((heldRotation+1) / 2) * 2;
            heldRotation = heldRotation % 4;
        }

        if((newAxis.x || newAxis.y) && !(movementAxis.x || movementAxis.y))
            rotation = atan2(newAxis.y, newAxis.x);
        movementAxis = newAxis;

        if(movementAxis.x || movementAxis.y) {
            rotation = RotaryLerp(rotation, atan2(movementAxis.y, movementAxis.x), deltaT * 10);
            speed = Lerp(speed, maxSpeed, deltaT * 5);
        }
        else speed = Lerp(speed, 0, deltaT * 5);

        // Update positions
        position.x += cosf(rotation) * speed * deltaT;
        if(!world->AreaIsAir(position, 0.45)) position.x -= cosf(rotation) * speed * deltaT;
        position.y += sinf(rotation) * speed * deltaT;
        if(!world->AreaIsAir(position, 0.45)) position.y -= sinf(rotation) * speed * deltaT;
        
        // Update player texture
        BeginTextureMode(renderTex);
        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
        ClearBackground(BLANK);
        DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height}, {(float)texSize / 2, (float)texSize / 2, (float)texSize, (float)texSize}, {(float)texSize / 2, (float)texSize / 2}, -rotation * RAD2DEG + 90, WHITE);
        EndTextureMode();

        // Execute dig queue
        if(digQueue.size()) {
            if(digProgress == 0) {
                // Get starting distance
                float distance = digQueue[0].Distance(position.Int());

                // Find closest next dig
                int i = 0;
                for(Vec2<int> tile : digQueue) {
                    const float next = tile.Distance(position.Int());
                    if(next <= distance) {
                        distance = next;
                        digQueueIndex = i;
                    }
                    ++i;
                }
            }

            if(digProgress > DIG_TIME) {
                // Delete target tile
                world->Set(digQueue[digQueueIndex].x, digQueue[digQueueIndex].y, DefaultTile(TILE_EMPTY));
                world->Update();
                digQueue.erase(next(digQueue.begin(), digQueueIndex));
                digProgress = 0;
            }
            else digProgress += deltaT;
        }
    }

    Texture2D GetTexture() {
        return renderTex.texture;
    }

    void Unload() {
        UnloadTexture(texture);
        UnloadRenderTexture(renderTex);
    }
};