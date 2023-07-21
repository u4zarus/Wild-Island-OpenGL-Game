#version 140

uniform mat4 PVMmatrix;
in vec3 position;
in vec2 textureCoord;
invariant gl_Position;

out vec2 textureCoord_v;

void main()
{
	gl_Position = PVMmatrix * vec4(position, 1.0);
	textureCoord_v = textureCoord;
}
