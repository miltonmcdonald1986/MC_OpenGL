#version 330 core

//in vec3 vsOutCol;
in vec2 vsOutTex;

out vec4 outColor;

uniform float mixPercentage;
uniform sampler2D samplerContainer;
uniform sampler2D samplerAwesomeFace;

void main ()
{
	outColor = mix(texture(samplerContainer, vsOutTex), texture(samplerAwesomeFace, vsOutTex), mixPercentage);
}
