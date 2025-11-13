#version 330 core

#define MAX_LIGHTS 8

out vec4 FragColor;

in vec3 color;

in vec2 texCoord;

in vec3 normal;
in vec3 worldPos;

uniform sampler2D tex0;
uniform bool textured;

uniform vec3 camPos;

uniform int activeLights;

struct LightProperty
{
	vec3 color;
	float intensity;
};

struct Light
{
	vec3 position;

	// attenuation
	float constant;
	float linear;
	float quadratic;

	LightProperty ambient;
	LightProperty diffuse;
	LightProperty specular;
};
uniform Light lights[MAX_LIGHTS];

uniform float shininess;

void main()
{
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(camPos - worldPos);

	vec3 lightSum;
	for (int i = 0; i < activeLights; ++i)
	{
		vec3 lightDir = normalize(lights[i].position - worldPos);
		vec3 reflectionDir = reflect(-lightDir, norm);

		float distance = length(lights[i].position - worldPos);
		float attenuationFactor = 1.0 / (lights[i].constant + lights[i].linear * distance + lights[i].quadratic * (distance * distance)); // Light attenuation formula.

		vec3 amb = lights[i].ambient.color * lights[i].ambient.intensity * attenuationFactor;

		float diffAmount = max(dot(norm, lightDir), 0.0);
		vec3 diff = lights[i].diffuse.color * lights[i].diffuse.intensity * diffAmount * attenuationFactor;
	
		float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0), shininess);
		vec3 spec = lights[i].specular.color * lights[i].specular.intensity * specAmount * attenuationFactor;
	
		vec3 phong = (amb + diff + spec);
		lightSum += phong;
	}


	if (textured)
	{
		vec4 textureColor = texture(tex0, texCoord);
		if (textureColor.a < 0.1) // Do not render transparent pixels.
		{
            discard;
		}
        FragColor = vec4(lightSum * textureColor.rgb, textureColor.a);
	}
	else
	{
		FragColor = vec4(color * lightSum, 1.0);
	}
}