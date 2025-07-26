#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, TexCoords);
	//vec4 inverted = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
	float grayScale = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
	FragColor = vec4(grayScale, grayScale, grayScale, 1.0);
}