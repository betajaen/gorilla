#version 120
attribute vec4 vertex;
attribute vec4 colour;
attribute vec4 uv0;

varying	vec4 oUv;
varying	vec4 oColor;
void main()
{
	oUv = uv0;
	oColor = colour;
	gl_Position = vertex;
}