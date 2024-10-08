#version 330 core

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMatMP;
uniform sampler2D gNormalForLight;
uniform sampler2D ssao;

out vec4 FragColor;

in vec2 TexCoords;

struct Light 
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Position;
};

struct Material
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};

uniform Light light;
uniform Material material;
uniform vec3 cameraPos;

// retrieve data from gbuffer
vec3 FragPos = texture(gPosition, TexCoords).rgb;
vec3 Normal = texture(gNormal, TexCoords).rgb;
vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
vec4 matMP = texture(gMatMP, TexCoords);
vec3 NormalForLight = texture(gNormalForLight, TexCoords).rgb;
float AmbientOcclusion = texture(ssao, TexCoords).r;

vec3 calculateBlinnPhongLight(vec4 pos, Light light, Material material)
{
	vec3 lightCoef;

	vec3 AmbientPart;
	vec3 DiffusePart; 
	vec3 SpecularPart;

    vec3 E = normalize(cameraPos - pos.xyz); // viewDir
    vec3 normal = normalize(NormalForLight);
    vec3 L = normalize(light.Position - pos.xyz); // LightDir
    vec3 H = normalize(L + E);

    float diff = max(dot(L, normal), 0);
    float spec = pow(max(dot(normal, H), 0), material.Shininess);
    float LV = distance(pos.xyz, light.Position);
    float latt = 1.0 / (light.Attenuation.x + light.Attenuation.y * LV + light.Attenuation.z * LV * LV);

    AmbientPart = light.Ambient * material.Ambient;
	DiffusePart = diff * light.Diffuse * material.Diffuse;
	SpecularPart = spec * light.Specular * material.Specular;

    lightCoef = AmbientPart + latt * (DiffusePart + SpecularPart);

    return lightCoef;
}

vec3 calculatePointLight(vec4 pos, Light light, Material material)
{
	vec3 L = normalize(light.Position - pos.xyz);
	vec3 E = normalize(cameraPos - pos.xyz);
	vec3 R = reflect(-E, NormalForLight);	

	float diff = dot(L, NormalForLight);
	float spec = pow(max(dot(R,L), 0), material.Shininess);
	float LV = distance(pos.xyz, light.Position);
	float Latt = 1.0f/(light.Attenuation.x + (light.Attenuation.y * LV) + (light.Attenuation.z * LV * LV));

	vec3 AmbientPart = light.Ambient * material.Ambient;
	vec3 DiffusePart = diff * light.Diffuse * material.Diffuse;
	vec3 SpecularPart = spec * light.Specular * material.Specular;

	vec3 lightCoef = AmbientPart + Latt * (DiffusePart + SpecularPart);

	return lightCoef;
}

void main()
{     
// poprawione ////////////////////////////////
    vec3 lightCoef = calculatePointLight(matMP, light, material);
	FragColor = vec4(lightCoef * Diffuse * AmbientOcclusion, 0.0f);
//////////////////////////////////////////////
}
