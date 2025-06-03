#version 330 core
out vec4 FragColor;

in vec3 color; // Get color from default.vert

in vec2 texCoord;

uniform sampler2D tex0;
uniform bool textured;

void main()
{
	if (textured)
		FragColor = texture(tex0, texCoord);
	else
		FragColor = vec4(color, 1.0f);
}