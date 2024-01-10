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

// Reflection of 'TileSprite' (in src/Tiles/Tile.cpp)
struct tile {
    int index;
    int gas;
    float alpha;
};

tile GetTile(vec2 pos) {
    // Get world texture pixel
    vec3 color = texture(world, pos / WORLDSIZE).rgb;

    // Construct tile
    return tile(
        int(color.r * 255.0 + 0.5),
        int(color.g * 255.0 + 0.5),
        color.b
    );
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

vec2 MapVec(vec2 pix, int index, float size) {
    return vec2((float(index) / size) + (mod(pix.x, 1) / size), mod(pix.y, 1));
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
    float d = 0;

    while(solid(iLightPos + round(g * d)) == 0) {
        if(iLightPos + round(g * d) == iTilePos) return vec3(1, 1, 1);
        d += 0.5;
    }

    float lightFactor = length(d - distance(iLightPos, iTilePos)) - 1;
    lightFactor = clamp(lightFactor / 3.0, 0, 1);

    return vec3(1 - lightFactor);
}

void main() {
    // Predefine temporary color buffer and mask color
    vec4 color, mask;

    // Convert screen position to world space tile position
    vec2 tilePos = (vec2(screenPosition.x - 0.5, (0.5 - screenPosition.y) / windowRatio)) * scale;
    tilePos += playerPos;
    
    // Extract tile-world data from world texture
    tile worldData = GetTile(tilePos);

    // Perform 'marching squares' calculation and get tile shape case
    int march = MarchTile(tilePos);

    // Calculate sub-position (remainder of position rounding) for texture coord
    vec2 subpos = mod(tilePos, 1.0);

    // Ignore masking if tile is ground
    if(worldData.index == 0) {
        color = texture(textureMap, MapVec(subpos, 0, float(textureCount)));
        mask = vec4(0, 0, 0, 1);
    }
    else {
        // Get gas from closest empty tile
        worldData.gas = int(texture(world, (tilePos + GetEmptyNeighbor(march)) / WORLDSIZE).g * 255.0 + 0.5);

        // Get pixel of ground texture for masking
        vec4 ground = texture(textureMap, MapVec(subpos, 0, float(textureCount)));

        // Get tile color
        mask = texture(edgeMask, MapVec(subpos, march, 16.0));
        color = texture(textureMap, MapVec(subpos, worldData.index, float(textureCount))) * mask.ggga;
        color = mix(ground, color, mask.r);
    }
    
    // Get and apply lighting
    vec3 light = (GetLight(tilePos, playerPos) * 0.5 + 0.5);
    color = color * vec4(light, 1);

    // Apply middle layer texture
    vec4 midLayer = texture(texture0, fragTexCoord);
    color = mix(color, vec4(midLayer.rgb, 1), (1-mask.r) * midLayer.a * fragColor.a);
    
    // Get and mix gas tint
    vec4 gasTint = texture(gasTextureMap, vec2(float(worldData.gas) / gasCount, 1));
    color = mix(color, vec4(gasTint.rgb, 1), (1-mask.r) * 0.15);

    finalColor = color;
}