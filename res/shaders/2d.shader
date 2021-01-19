#shader vertex
#version 330 core


layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main()
{
    gl_Position = proj * view * model * vec4(position,1.0f);
}


#shader fragment
#version 330 core

out vec4 Color;

void main()
{
    Color = vec4(0.0f,0.3f,1.0f,1.0f);
}