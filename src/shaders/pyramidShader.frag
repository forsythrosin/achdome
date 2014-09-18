#version 330 core

uniform float alpha;
uniform float time;
out vec4 color;

void main()
{
	color = vec4(1.0, (1+sin(time))/2, 0.5, alpha);
}
