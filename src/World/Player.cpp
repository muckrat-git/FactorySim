#include "Other/Types.cpp"
#include "World/World.cpp"

class Player {
    private:
    const int texFrames = 8;
    
    RenderTexture2D renderTex;
    
    public:
    const int texSize = 16;

    World * world;

    const float maxSpeed = 10;
    float speed = 0;

    float zoom = 70;        // The viewport zoom
    float zoomVelocity = 0; // Viewport zoom increment factor

    float rotation = 0;
    Vec2<float> movementAxis = {0, 0}; // Vector representing movement input
    Vec2<float> position = {0, 0};

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
        
        // Clamp zoom
        if(zoom < 5) zoom = 5;
        //if(zoom > 200) zoom = 200;

        Vec2<float> newAxis;
        newAxis.x = IsKeyDown(KEY_D);
        newAxis.x -= IsKeyDown(KEY_A);
        newAxis.y = IsKeyDown(KEY_S);
        newAxis.y -= IsKeyDown(KEY_W);

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
        //if(!world->AreaIsAir(position, 0.5)) position.x -= cosf(rotation) * speed * deltaT;
        position.y += sinf(rotation) * speed * deltaT;
        //if(!world->AreaIsAir(position, 0.5)) position.y -= sinf(rotation) * speed * deltaT;
        
        BeginTextureMode(renderTex);
        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
        ClearBackground(BLANK);
        DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height}, {(float)texSize / 2, (float)texSize / 2, (float)texSize, (float)texSize}, {(float)texSize / 2, (float)texSize / 2}, -rotation * RAD2DEG + 90, WHITE);
        EndTextureMode();
    }

    Texture2D GetTexture() {
        return renderTex.texture;
    }

    void Unload() {
        UnloadTexture(texture);
        UnloadRenderTexture(renderTex);
    }
};