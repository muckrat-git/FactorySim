#version 330

#define WORLDSIZE 600

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec2 screenPosition;
in vec4 glPosition;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float scale;
uniform float windowRatio;
uniform vec2 playerPos;
uniform int textureCount;

uniform sampler2D world;
uniform sampler2D edgeMask;
uniform sampler2D textureMap;

// Output fragment color
out vec4 finalColor;

int solid(float x, float y) {
    return texture(world, vec2(x, y) / WORLDSIZE).r != 0.0 ? 1 : 0;
}

// NOTE: Add here your custom variables
int MarchTile(vec2 pos) {
    return (
        (solid(pos.x, pos.y - 1) << 3) | 
        (solid(pos.x + 1, pos.y) << 2) | 
        (solid(pos.x, pos.y + 1) << 1) | 
        (solid(pos.x - 1, pos.y))
    );
}

vec2 MapVec(vec2 pix, int index, float size) {
    return vec2((float(index) / size) + (mod(pix.x, 1) / size), mod(pix.y, 1));
}

vec4 GetColor(vec2 tilePos, int march) {
    vec2 subpos = mod(tilePos, 1.0);

    vec4 ground = texture(textureMap, MapVec(subpos, 0, float(textureCount)));

    int tile = int(texture(world, tilePos / WORLDSIZE).r * 255.0);
    if(tile == 0) return ground;

    // Get tile color
    vec4 mask = texture(edgeMask, MapVec(subpos, march, 16.0));
    vec4 color = texture(textureMap, MapVec(subpos, tile, float(textureCount))) * mask.ggga;
    return mix(ground, color, mask.r);
}

float atan2(float x, float y) {
    float val = atan(y, x);
    return val;
}

void main() {
    vec4 color, mask;

    vec2 tilePos = (vec2(screenPosition.x - 0.5, (0.5 - screenPosition.y) / windowRatio)) * scale;
    tilePos += playerPos;

    int march = MarchTile(tilePos);
    vec2 subpos = mod(tilePos, 1.0);

    vec4 ground = texture(textureMap, MapVec(subpos, 0, float(textureCount)));

    int tile = int(texture(world, tilePos / WORLDSIZE).r * 256.0);
    if(tile == 0) {
        color = ground;
        mask = vec4(0, 0, 0, 1.0);
    }
    else {
        // Get tile color
        mask = texture(edgeMask, MapVec(subpos, march, 16.0));
        color = texture(textureMap, MapVec(subpos, tile, float(textureCount))) * mask.ggga;
        color = mix(ground, color, mask.r);
    }
    
    // NOTE: Implement here your fragment shader code
    finalColor = color;
}