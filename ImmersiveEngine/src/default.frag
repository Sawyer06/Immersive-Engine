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

struct LightProperty
{
	vec3 color;
	vec3 intensity;
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
	for (int i = 0; i < lights.MAX_SIZE; ++i)
	{
		vec3 lightDir = normalize(lights[i].position - worldPos);
		vec3 reflectionDir = reflect(-lightDir, norm);

		float distance = length(lights[i].position - worldPos);
		float attenuationFactor = 1.0f / (lights[i].constant + lights[i].linear * distance * lights[i].quadratic * (distance * distance)); // Light attenuation formula.

		vec3 amb = lights[i].ambient.color * lights[i].ambient.intensity * attenuationFactor;;

		float diffAmount = max(dot(norm, lightDir), 0.0f);
		vec3 diff = lights[i].diffuse.color * lights[i].diffuse.intensity * diffAmount * lights[i].color * attenuationFactor;;
	
		float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), shininess);
		vec3 spec = lights[i].specular.color * lights[i].specular.intensity * specAmount * lights[i].color * attenuationFactor;
	
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
		FragColor = vec4(color * lightSum, 1.0f);
	}
}