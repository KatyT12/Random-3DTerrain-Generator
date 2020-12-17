#shader vertex
#version 330 core


layout (location = 0) in vec2 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 Offset; //added this

out vec3 fColor;

void main()
{
    fColor = Color;
    vec2 pos = Position * (gl_InstanceID / 100.0);
    gl_Position = vec4(pos + Offset,0.0,1.0);
    
}



#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 fColor;

void main()
{
    
    FragColor = vec4(fColor,1.0);
}

