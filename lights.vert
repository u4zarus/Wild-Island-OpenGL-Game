#version 140

in vec3 position;
in vec3 normal;
in vec2 textureCoord;

out vec2 textureCoord_v;
out vec3 normal_v;
out vec3 position_v;

uniform mat4 normalMatrix;
uniform mat4 PVMmatrix;
uniform mat4 Vmatrix;
uniform mat4 Mmatrix;

invariant gl_Position;

void main()
{
	vec3 eyeNormal = normalize((normalMatrix * vec4(normal, 0.0)).xyz);
	normal_v = eyeNormal;

	vec3 worldPos = (Vmatrix * Mmatrix * vec4(position, 1)).xyz;
	position_v = worldPos;

	gl_Position = PVMmatrix * vec4(position, 1);

	textureCoord_v = textureCoord;
}
