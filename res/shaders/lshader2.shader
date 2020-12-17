#shader vertex
#version 330 core

layout (location = 0) in vec4 Position;


uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * Position;
}

//Literally the same as the other lshader but I can pass in the lights color as well
#shader fragment
#version 330 core
out vec4 FragColor;

uniform vec3 u_color;

void main()
{
    FragColor = vec4(u_color,1.0f); // set all 4 vector values to 1.0
}

