#shader vertex
#version 330 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;


uniform mat4 model;

out vec3 v_Color;

layout(std140) uniform proj_and_view
{
    mat4 proj;
    mat4 view;
};

void main()
{
    gl_Position = proj * view * model * vec4(position,1.0f);
    v_Color = color;
}


#shader fragment
#version 330 core

layout (location = 0) out vec4 Color; 
in vec3 v_Color;

void main()
{
    Color = vec4(v_Color,1.0f);
}