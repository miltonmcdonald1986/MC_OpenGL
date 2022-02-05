#version 330 core

layout (location = 0) in vec3 attribPos;
layout (location = 1) in vec3 attribCol;
layout (location = 2) in vec2 attribTex;

out vec3 vsOutCol;
out vec2 vsOutTex;

uniform mat4 transform;

void main ()
{
	gl_Position = transform*vec4 (attribPos, 1.f);
	vsOutCol    = attribCol;
	vsOutTex    = attribTex;
}
