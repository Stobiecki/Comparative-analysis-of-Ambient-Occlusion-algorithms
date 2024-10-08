#version 430

//layout(location = 0) uniform vec4 clipInfo;
uniform vec4 clipInfo;
layout(binding = 0)  uniform sampler2D inputTexture;

layout(location = 0, index = 0) out float out_Color;

float reconstructCSZ(float d, vec4 clipInfo) 
{
  if (clipInfo[3] != 0) 
  {
    return (clipInfo[0] / (clipInfo[1] * d + clipInfo[2]));
  }
  else 
  {
    return (clipInfo[1]+clipInfo[2] - d * clipInfo[1]);
  }
}

void main()
{    
    float depth = texelFetch(inputTexture, ivec2(gl_FragCoord.xy), 0).x;

    out_Color = reconstructCSZ(depth, clipInfo);
    //out_Color /= 255.0f;
}