#version 330 core

in vec3 vColor;
in vec3 vNormal;
noperspective in vec2 vTexCoord;
in float vDepth;
in float vLighting;

out vec4 FragColor;

uniform float uColorLevels;
uniform float uFogNear;
uniform float uFogFar;
uniform vec3 uFogColor;
uniform vec3 uObjectColor;

float Bayer4x4(vec2 pos) {
    // 4x4 Bayer matrix normalized to [0,1)
    int x = int(mod(pos.x, 4.0));
    int y = int(mod(pos.y, 4.0));
    int index = y * 4 + x;
    // Bayer 4x4 values
    float bayer[16] = float[16](
        0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
        12.0/16.0, 4.0/16.0, 14.0/16.0, 6.0/16.0,
        3.0/16.0, 11.0/16.0, 1.0/16.0,  9.0/16.0,
        15.0/16.0, 7.0/16.0, 13.0/16.0, 5.0/16.0
    );
    return bayer[index];
}

void main() {
    // Base color: vertex color * object color * flat lighting
    vec3 color = vColor * uObjectColor * vLighting;

    // Ordered dithering + color quantization (15-bit style)
    // Uses named uniform uColorLevels instead of raw magic number
    float dither = Bayer4x4(gl_FragCoord.xy);
    // Push dither into quantization to break banding
    vec3 dithered = color + (dither - 0.5) / uColorLevels;
    vec3 quantized = floor(dithered * uColorLevels) / uColorLevels;

    // Short linear fog (PSX short draw distance)
    float fogFactor = clamp((uFogFar - vDepth) / (uFogFar - uFogNear), 0.0, 1.0);
    vec3 finalColor = mix(uFogColor, quantized, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
