// implementation based on https://github.com/shabaazxh/Real-Time-Ambient-Occlusion/blob/master/Shaders/Postprocessing/SSAO.frag

#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[256];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
uniform int kernelSize = 64;
uniform float radius = 0.5;
uniform float bias = 0.025;
uniform float noiseSize = 4.0f;
uniform float aoAmplification = 1.0f;

uniform float windowWidth = 800.0;
uniform float windowHight = 600.0;


// tile noise texture over screen based on screen dimensions divided by noise size
vec2 noiseScale = vec2(windowWidth/noiseSize, windowHight/noiseSize); 

uniform mat4 projection;

void main()
{
    // get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    //(Joey De Vries, 2020) Create a TBN matrix to convert the sample from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal.xyz * dot(randomVec, normal.xyz));
    vec3 bitangent = cross(normal.xyz, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal.xyz);

    vec3 plane = texture(texNoise, TexCoords * noiseScale).xyz - vec3(1.0);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = reflect(samples[i].xyz, plane); //reflect the sample 
        samplePos = TBN * samplePos; // convert sample to view-space
        samplePos = fragPos + samplePos * radius; //offset current position with sample pos

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; //convert .to clip space
        offset.xyz /= offset.w; // perspective divide 
        offset.xy = offset.xy * 0.5 + 0.5; // convert to texture coordinate (0,1)

        float sampleDepth = texture(gPosition, offset.xy).z; // obtain sample pos depth value
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth)); // range check to ensure within radius
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck; // check if depth > z pos to determine occlusion         
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    if(aoAmplification != 1.0f)
        occlusion = pow(occlusion, aoAmplification);

    FragColor = occlusion;
}
