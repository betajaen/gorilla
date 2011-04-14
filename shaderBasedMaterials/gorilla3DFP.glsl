#version 120

varying	vec4 oUv;
varying	vec4 oColor;

uniform	sampler2D atlas;

void main() 
{
	gl_FragColor = texture2D(atlas, oUv.xy) * oColor;
}