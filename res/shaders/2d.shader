#shader vertex
#version 330 core


layout (location = 0) in vec3 position;

uniform mat4 model;

layout(std140) uniform proj_and_view
{
    mat4 proj;
    mat4 view;
};

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