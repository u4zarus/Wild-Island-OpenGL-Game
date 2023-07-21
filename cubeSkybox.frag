#version 140

uniform samplerCube skyboxSampler;
smooth in vec3 textureCoord_v;
out vec4 color_f;

void main()
{
	vec4 texel = texture(skyboxSampler, textureCoord_v);
	color_f = vec4(texel.xyz, 1.0);
}
