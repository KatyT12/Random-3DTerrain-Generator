#shader vertex
#version 330 core
	    
layout (location = 0) in vec4 position;

layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;


out vec3 ourColor;
out vec3 normal;

//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * position; //Multiply the projection matrix by the position
	ourColor = aColor;
    normal = aNormal;
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 

in vec3 ourColor;
in vec3 normal;

uniform vec4 u_Color;


void main()
{
	//Getting the color from our texture with the right coord and assigning the pixels on the screen to it
	color = vec4(normal,1.0f);
	
	
};