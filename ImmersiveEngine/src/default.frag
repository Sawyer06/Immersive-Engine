#version 330 core
out vec4 FragColor;

in vec3 color; // Get color from default.vert

in vec2 texCoord;

in vec3 normal;
in vec3 worldPos;

uniform sampler2D tex0;
uniform bool textured;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

struct Strength
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
  
uniform Strength strength;

void main()
{
	float ambient = 0.1f;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - worldPos);

	float diffuse = max(dot(norm, lightDir), 0.0f);
	float specularLight = 1.0f;
	vec3 viewDir = normalize(camPos - worldPos);
	vec3 reflectionDir = reflect(-lightDir, norm);
	float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 8);
	float specular = specAmount * specularLight;

	float phong = (ambient + diffuse + specular);

	if (textured)
	{
		vec4 textureColor = texture(tex0, texCoord);
		if (textureColor.a < 0.1) // Do not render transparent pixels.
		{
            discard;
		}
        FragColor = vec4(phong * textureColor.rgb * lightColor, textureColor.a);
	}
	else
	{
		FragColor = vec4(color * phong * lightColor, 1.0f);
		FragColor.a = 1.0f;
	}
}