#version 140

uniform sampler2D textureSampler;

smooth in vec2 textureCoord_v;
out vec4 color_f;

void main()
{
	color_f = texture(textureSampler, textureCoord_v);
}
