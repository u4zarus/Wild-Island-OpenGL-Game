#version 140

uniform float currentTime; 
uniform mat4 Vmatrix;
uniform sampler2D textureSampler;
in vec3 position_v;
in vec2 textureCoord_v;
out vec4 color_f;

uniform vec2 spritesheetLayout = vec2(8.0, 2.0);
uniform float frameDuration;

vec4 sampleTexture(int frame)
{
	vec2 offset = vec2(1.0) / spritesheetLayout;
	vec2 texureCoord = textureCoord_v / spritesheetLayout + vec2(mod(float(frame), spritesheetLayout.x), floor(float(frame) / spritesheetLayout.x)) * offset;
	return texture(textureSampler, texureCoord);
}

void main()
{
	int frame = int(floor(currentTime / frameDuration + 0.5));
	color_f = sampleTexture(frame);
}
