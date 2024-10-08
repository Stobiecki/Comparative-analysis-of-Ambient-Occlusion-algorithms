#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D UV;

void main() 
{
    vec4 textured = texture(UV, TexCoords.xy);
    textured = vec4(textured.r, textured.r, textured.r, textured.a);
    FragColor = textured;
}  