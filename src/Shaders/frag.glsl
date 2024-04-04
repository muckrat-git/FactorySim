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

// Input tile constants from CPU
#tiledefs

// Output fragment color
out vec4 finalColor;

vec4 GetTile(vec2 pos) {
    // Get world texture pixel
    return texture(world, pos / WORLDSIZE).rgba * vec4(255, 255, 1, 255);
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

// NOTE: Add here your custom variables
int MarchTile(vec2 pos) {
    return (
        (int(!small(pos.x, pos.y - 1)) << 3) | 
        (int(!small(pos.x + 1, pos.y)) << 2) | 
        (int(!small(pos.x, pos.y + 1)) << 1) | 
        (int(!small(pos.x - 1, pos.y)))
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

vec2 RotateTexCoord(vec2 coord, vec2 resolution, float angle) {
    return (vec2((coord.x - 0.5) * (resolution.x / resolution.y), coord.y - 0.5) * mat2(cos(angle), sin(angle), -sin(angle), cos(angle))) + 0.5;
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
    float maxD = distance(iTilePos, iLightPos);

    for(d = 0; d < maxD && transparent(iLightPos + round(g * d)); ++d);
    if(d >= maxD) return vec3(1, 1, 1);

    float lightFactor = length(d - distance(iLightPos, iTilePos));
    lightFactor = clamp(lightFactor / 3, 0, 1);

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
    vec4 worldData = GetTile(tilePos);
    
    // Get tile color
    vec4 color = texture(textureMap, MapVec(RotateTexCoord(subpos, vec2(16.0, 16.0), worldData.a * 3.1415 / 2), worldData.r, textureCount));

    vec4 mask = vec4(0, 0, 0, 1);

    // Get pixel of ground texture for masking
    vec4 ground = texture(textureMap, MapVec(subpos, 0, textureCount));

    if(!IsSmall(int(worldData.r + 0.5))) {
        // Apply a form of the 'marching squares' algorithm and get tile shape case
        int march = MarchTile(tilePos);
        mask = texture(edgeMask, MapVec(subpos, march, 16.0));
        color *= mix(mask.ggga, vec4(1, 1, 1, 1), 0.5);
        mask.r *= color.a;
        color = mix(ground, color, mask.r);

        // Get gas from closest empty tile
        worldData.gb = texture(world, (tilePos + GetEmptyNeighbor(march)) / WORLDSIZE).gb * vec2(255.0, 1.0);
    }
    else {
        // Apply edge shading on air tiles
        const float step = 8;                   // Step is how many pixels to check
        const float angles = 4;                 // How many angles to check
        const float stepCount = step * angles;  // The total steps taken
        const float opacity = 0.8;              // Final opacity of the shading

        float coverage = 0; // Total surrounding pixels that are solid

        // Check around point offset by 45 degrees to cover tile edges evenly
        for(float angle = radians(45); angle < radians(360 + 45); angle += radians(360 / angles)) {
            // Calculate xy product of angle (gradient)
            vec2 m = vec2(cos(angle), sin(angle));

            // Get coverage in the specified angle
            for(float dist = 0; dist < 0.5; dist += 0.5 / step) {
                if(!small(tilePos - dist * m) && solid(tilePos - dist * m)) coverage++;
                else if(dist < 0.25 && floor(tilePos) == floor(tilePos - dist * m)) {
                    subpos = mod(tilePos - dist * m, 1);
                    coverage += texture(textureMap, MapVec(RotateTexCoord(subpos, vec2(16.0, 16.0), worldData.a * 3.1415 / 2), worldData.r, textureCount)).a * (1-color.a) * 2;
                }
            }
        }

        // Apply shade to color
        mask.r = color.a * clamp(worldData.r * 255, 0, 1);
        color = mix(ground, color, color.a);
        color *= 1 - (coverage * opacity / stepCount);
    }

    // Apply middle layer texture
    vec4 midLayer = texture(texture0, fragTexCoord);
    color = mix(color, vec4(midLayer.rgb, 1), (1-mask.r) * midLayer.a * fragColor.a);
    
    // Get and mix gas tint
    vec4 gasTint = texture(gasTextureMap, vec2((worldData.g + 0.6) / gasCount, 1));
    color = mix(color, vec4(gasTint.rgb, 1), (1-mask.r) * 0.3 * worldData.b); 
    
    // Get and apply lighting
    vec3 light = (GetLight(tilePos, playerPos) * 0.1 + 0.9);
    color = color * vec4(light, 1);

    finalColor = vec4(color.rgb, 1);
}