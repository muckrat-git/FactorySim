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

vec4 GetColor(vec2 pos) {
    vec2 subpos = mod(pos, 1.0);

    vec4 ground = texture(textureMap, MapVec(subpos, 0, float(textureCount)));

    int tile = int(texture(world, pos / WORLDSIZE) * 255.0);
    if(tile == 0) return ground;

    // Get tile color
    vec4 mask = texture(edgeMask, MapVec(subpos, MarchTile(pos), 16.0));
    vec4 color = texture(textureMap, MapVec(subpos, tile, float(textureCount))) * mask.ggga;
    
    return mix(ground, color, mask.r);
}

void main() {
    vec2 tilePos = (vec2(screenPosition.x - 0.5, (0.5 - screenPosition.y) / windowRatio)) * scale;
    tilePos += playerPos;

    // NOTE: Implement here your fragment shader code
    finalColor = GetColor(tilePos);
}