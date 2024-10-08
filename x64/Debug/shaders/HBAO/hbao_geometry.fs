#version 330 core
layout (location = 0) out vec3 gAlbedo;

uniform sampler2D texture_diffuse1;

in vec2 TexCoords;
in vec3 NormalForLight;
in vec4 MatMP;

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
// the one working (simple one)
/////////////////////////////////////////////////////
  	// vec4 color = texture(texture_diffuse1, TexCoords);
  	// gAlbedo.rgb = color.rgb;
/////////////////////////////////////////////////////
  	vec4 texColor = texture(texture_diffuse1, TexCoords);

	vec3 lightCoef = calculatePointLight(MatMP, light, material);

	gAlbedo = lightCoef * texColor.xyz;
}