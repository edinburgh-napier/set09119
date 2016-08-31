#version 440
uniform vec4 colour_override;
layout (location = 0) out vec4 out_colour;

void main()
{
	out_colour = colour_override;
}