#shader vertex
#version 330 core

layout (location = 0) in vec4 Position;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * Position;
}


#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.04, 0.28, 0.26, 1.0); // set all 4 vector values to 1.0
}

//origin advanced/main2.cpp