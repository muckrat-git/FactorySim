#include "Other/Types.cpp"

class Player {
    public:
    const float maxSpeed = 20;
    float speed = 0;

    float zoom = 70;        // The viewport zoom
    float zoomVelocity = 0; // Viewport zoom increment factor

    float rotation = 0;
    Vec2<float> movementAxis = {0, 0}; // Vector representing movement input
    Vec2<float> position = {0, 0};

    void Update(float deltaT) {
        zoomVelocity -= GetMouseWheelMove() * deltaT * 20;
        zoom += zoomVelocity;
        zoomVelocity = Lerp(zoomVelocity, 0, deltaT * 10);
        if(abs(zoomVelocity) < deltaT * 5) zoomVelocity = 0;
        
        // Clamp zoom
        if(zoom < 5) zoom = 5;
        //if(zoom > 200) zoom = 200;

        movementAxis.x = IsKeyDown(KEY_D);
        movementAxis.x -= IsKeyDown(KEY_A);
        movementAxis.y = IsKeyDown(KEY_S);
        movementAxis.y -= IsKeyDown(KEY_W);

        if(movementAxis.x || movementAxis.y) {
            rotation = atan2(movementAxis.y, movementAxis.x);
            speed = Lerp(speed, maxSpeed, deltaT * 5);
        }
        else speed = Lerp(speed, 0, deltaT * 5);

        position.x += cosf(rotation) * speed * deltaT;
        position.y += sinf(rotation) * speed * deltaT;
    }
};