#version 330 core

layout (location = 0) in vec3 attribPos;
//layout (location = 1) in vec3 attribCol;
layout (location = 1) in vec2 attribTex;

out vec3 vsOutCol;
out vec2 vsOutTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main ()
{
	gl_Position = projection*view*model*vec4(attribPos, 1.f);
	//vsOutCol    = attribCol;
	vsOutTex    = attribTex;
}
