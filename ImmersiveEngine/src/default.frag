#version 330 core
out vec4 FragColor;

in vec3 color; // Get color from default.vert

in vec2 texCoord;

in vec3 normal;
in vec3 worldPos;

uniform sampler2D tex0;
uniform bool textured;

uniform vec3 camPos;

uniform vec3 lightColor;
uniform vec3 lightPos;

struct LightProperty
{
	vec3 color;
	vec3 intensity;
};
  
uniform LightProperty ambient;
uniform LightProperty diffuse;
uniform LightProperty specular;

uniform float shininess;

void main()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - worldPos);
	vec3 viewDir = normalize(camPos - worldPos);
	vec3 reflectionDir = reflect(-lightDir, norm);

	vec3 amb = ambient.color * ambient.intensity;

	float diffAmount = max(dot(norm, lightDir), 0.0f);
	vec3 diff = diffuse.color * diffuse.intensity * diffAmount * lightColor;
	
	float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), shininess);
	vec3 spec = specular.color * specular.intensity * specAmount * lightColor;

	vec3 phong = (amb + diff + spec);

	if (textured)
	{
		vec4 textureColor = texture(tex0, texCoord);
		if (textureColor.a < 0.1) // Do not render transparent pixels.
		{
            discard;
		}
        FragColor = vec4(phong * textureColor.rgb, textureColor.a);
	}
	else
	{
		FragColor = vec4(color * phong, 1.0f);
	}
}