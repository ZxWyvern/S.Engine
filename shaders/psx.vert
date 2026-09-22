#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uSnapScale;
uniform vec3 uLightDir;

out vec3 vColor;
noperspective out vec2 vTexCoord;
out float vDepth;
out float vLighting;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vec4 viewPos = uView * worldPos;
    vec4 clipPos = uProjection * viewPos;

    // PSX vertex snapping: snap after perspective divide, then re-multiply by w
    // Uses named uniform uSnapScale instead of magic number (RULES 2.2)
    vec4 snapped = clipPos;
    snapped.xyz /= snapped.w;
    snapped.xy = floor(snapped.xy * uSnapScale) / uSnapScale;
    snapped.xyz *= snapped.w;
    gl_Position = snapped;

    // Affine-ready: noperspective interpolation plumbing for future textured
    // materials. No texture is sampled in frag yet, so the warp is not
    // observable on flat-colored cubes — documented as affine-ready, not
    // completed affine texturing (see AGENTS §7).
    vTexCoord = aTexCoord;

    // Flat Lambertian per-vertex lighting (RULES: no PBR)
    vec3 worldNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vec3 lightDir = normalize(-uLightDir);
    float lambert = max(dot(normalize(worldNormal), lightDir), 0.0);
    // Keep ambient so unlit faces remain visible (PSX style: flat + ambient)
    vLighting = 0.35 + 0.65 * lambert;

    vColor = aColor;

    // Depth for linear fog (view-space depth)
    vDepth = -viewPos.z;
}
