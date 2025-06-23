#version 330 core
out vec4 FragColor;

in vec3 color; // Get color from default.vert

in vec2 texCoord;

//in normal;

uniform sampler2D tex0;
uniform bool textured;

uniform vec4 lightColor;
uniform vec3 lightPos;

void main()
{
	if (textured)
	{
		vec4 textureColor = texture(tex0, texCoord);
		if (textureColor.a < 0.1) // Do not render transparent pixels.
		{
            discard;
		}
        FragColor = textureColor * lightColor;
	}
	else
	{
		FragColor = vec4(color, 1.0f) * lightColor;
	}
}