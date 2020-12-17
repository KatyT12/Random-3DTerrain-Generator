#shader vertex
#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

out vec3 fColor;

uniform vec2 offsets[100];

void main()
{
    vec2 offset = offsets[gl_InstanceID]; //gl_InstanceID is a built in GLSL variable
    gl_Position = vec4(position + offset,0.0,1.0);
    fColor = color;
}


#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 fColor;

void main()
{
    
    FragColor = vec4(fColor,1.0);
}

