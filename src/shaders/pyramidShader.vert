#version 330 core

layout(location = 0) in vec3 vertPosition;

uniform mat4 MVP;
uniform float time;

void main()
{
    // Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertPosition+sin(time), 1.0);
}
