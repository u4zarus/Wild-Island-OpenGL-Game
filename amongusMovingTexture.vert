#version 140

uniform mat4 PVMmatrix;
uniform float currentTime;

const float SLOWER = 0.5;
const vec2 OFFSET = vec2(1.0, 0.0);

in vec3 position;
in vec2 textureCoord;

smooth out vec2 textureCoord_v;

void main()
{
	gl_Position = PVMmatrix * vec4(position, 1.0);

	float slowedTime = currentTime * SLOWER;
	vec2 offset = mod(slowedTime, 2.0) * OFFSET - OFFSET;

	textureCoord_v = textureCoord + offset;
}
