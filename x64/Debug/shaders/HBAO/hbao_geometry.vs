#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 NormalForLight;
out vec4 MatMP;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);

    NormalForLight = aNormal;

    TexCoords = aTexCoords; 
    
    MatMP = model * vec4(aPos, 1.0);

    gl_Position = projection * viewPos;
}