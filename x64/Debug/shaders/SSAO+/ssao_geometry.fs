#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMatMP;
layout (location = 4) out vec3 gNormalForLight;

uniform sampler2D texture_diffuse1;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 MatMP;
in vec3 NormalForLight;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    // and the diffuse per-fragment color
    gAlbedo.rgb = texture(texture_diffuse1, TexCoords).rgb;
    // bonus, modelMat * position, as color
    gMatMP = MatMP;
    // bonus, normalForLight * transpose(inverse(mat3(model))), as color
    gNormalForLight = normalize(NormalForLight);
}