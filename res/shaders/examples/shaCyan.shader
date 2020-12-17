#shader vertex
#version 330 core
	    
layout (location = 0) in vec3 position;

layout(std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
};

uniform mat4 model;

void main()
{
	gl_Position = proj * view * model * vec4(position,1.0); //Multiply the projection matrix by the position
}
   

#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 

void main()
{
	color = vec4(0.0,0.63,0.55,1.0);	
};
