#shader vertex
#version 330 core
	    
layout (location = 0) in vec4 position;

layout (location = 1) in vec3 aColor;

out vec3 ourColor;

//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;




void main()
{
	gl_Position = proj * view * model * position; //Multiply the projection matrix by the position
	ourColor = aColor;
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 

in vec3 ourColor;

uniform vec4 u_Color;


void main()
{
	//Getting the color from our texture with the right coord and assigning the pixels on the screen to it
	color = u_Color; 
	color = vec4(ourColor,1.0f);
	
};

