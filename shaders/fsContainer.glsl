#version 330 core

in vec3 vsOutCol;
in vec2 vsOutTex;

out vec4 outColor;

uniform sampler2D samplerTex;

void main ()
{
	outColor = texture(samplerTex, vsOutTex);
}
