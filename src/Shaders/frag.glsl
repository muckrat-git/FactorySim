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
uniform int gasCount;

uniform sampler2D world;
uniform sampler2D edgeMask;
uniform sampler2D textureMap;
uniform sampler2D gasTextureMap;
uniform sampler2D player;

// Output fragment color
out vec4 finalColor;

vec3 GetTile(vec2 pos) {
    // Get world texture pixel
    return texture(world, pos / WORLDSIZE).rgb * vec3(255, 255, 1);
}

// Returns the relative position of an empty neighbor tile from a marching square case ('c')
vec2 GetEmptyNeighbor(int c) {
    const vec2 airNeighbor[] = vec2[16](
        vec2(0, -1),
        vec2(1, 0),
        vec2(0, -1),
        vec2(0, -1),
        vec2(-1, 0),
        vec2(0, -1),
        vec2(0, -1),
        vec2(0, -1),
        vec2(0, 1),
        vec2(0, 1),
        vec2(1, 0),
        vec2(1, 0),
        vec2(-1, 0),
        vec2(0, 1),
        vec2(-1, 0),
        vec2(0, 0)
    );
    return airNeighbor[c];
}

bool ApplyMarching(int index) {
    const bool applyMarching[] = bool[](
        false,
        true,
        true,
        true,
        true,
        false,
        false
    );
    return applyMarching[index];
}

int solid(float x, float y) {
    return texture(world, vec2(x, y) / WORLDSIZE).x != 0.0 ? 1 : 0;
}
int solid(vec2 pos) {
    return solid(pos.x, pos.y);
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

vec2 MapVec(vec2 pix, float index, float size) {
    return vec2((index + mod(pix.x, 1)) / size, mod(pix.y, 1));
}
vec2 MapVec(vec2 pix, vec2 index, vec2 size) {
    return (index + mod(pix, 1)) / size;
}

float atan2(vec2 pos) {
    return atan(pos.y, pos.x);
}

vec3 GetLight(vec2 tilePos, vec2 lightPos) {
    vec2 iTilePos = floor(tilePos);
    vec2 iLightPos = floor(lightPos);

    // Get the direction from light to tile
    float r = atan2(iTilePos - iLightPos);
    // The x and y gradient 
    vec2 g = vec2(cos(r), sin(r));
    // Distance from light to tile
    float d = 0.5;

    while(solid(iLightPos + round(g * d)) == 0 && iLightPos + round(g*d) != iTilePos) {
        d += 0.5;
    }
    if(iLightPos + round(g * d) == iTilePos) return vec3(1, 1, 1);

    float lightFactor = length(d - distance(iLightPos, iTilePos)) - 1;
    lightFactor = clamp(lightFactor / 3.0, 0, 1);

    return vec3(1 - lightFactor);
}

void main() {
    // Convert screen position to relative tile position
    vec2 tilePos = vec2(screenPosition.x - 0.5, (0.5 - screenPosition.y) / windowRatio) * scale;

    // Calculate mod with seperate tile and player pos to avoid large floating point errors
    vec2 subpos = mod(tilePos + mod(playerPos, 1), 1);

    // Add player pos to get world-space tile position
    tilePos += playerPos;
    
    // Extract tile-world data from world texture
    vec3 worldData = GetTile(tilePos);
    
    // Get tile color
    vec4 color = texture(textureMap, MapVec(subpos, worldData.r, textureCount));

    vec4 mask = vec4(0, 0, 0, 1);

    if(worldData.r != 0) {
        // Get pixel of ground texture for masking
        vec4 ground = texture(textureMap, MapVec(subpos, 0, textureCount));

        // Apply a form of the 'marching squares' calculation and get tile shape case
        int march = MarchTile(tilePos);
        mask = texture(edgeMask, MapVec(subpos, vec2(march, !ApplyMarching(int(worldData.r + 0.5))), vec2(16, 2)));
        color = mix(ground, color * mask.ggga, mask.r);

        // Get gas from closest empty tile
        worldData.gb = texture(world, (tilePos + GetEmptyNeighbor(march)) / WORLDSIZE).gb * vec2(255.0, 1.0);
    }
    
    // Get and apply lighting
    vec3 light = (GetLight(tilePos, playerPos) * 0.5 + 0.5);
    color = color * vec4(light, 1);

    // Apply middle layer texture
    vec4 midLayer = texture(texture0, fragTexCoord);
    color = mix(color, vec4(midLayer.rgb, 1), (1-mask.r) * midLayer.a * fragColor.a);
    
    // Get and mix gas tint
    vec4 gasTint = texture(gasTextureMap, vec2(worldData.g / gasCount, 1));
    color = mix(color, vec4(gasTint.rgb, 1), (1-mask.r) * 0.15 * worldData.b);

    finalColor = color;
}