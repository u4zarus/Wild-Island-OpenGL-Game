#version 140

uniform mat4 inversePVmatrix;
in vec2 screenCoord;
invariant gl_Position;
smooth out vec3 textureCoord_v;

void main()
{
	vec4 clipCoord = vec4(screenCoord.xy, 0.9999, 1.0);
	vec4 viewCoord = inversePVmatrix * clipCoord;
	vec3 worldCoord = viewCoord.xyz / viewCoord.w;
	textureCoord_v = worldCoord;
	gl_Position = clipCoord;
}
