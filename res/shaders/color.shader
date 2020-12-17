#shader vertex
#version 330 core
	    
layout (location = 0) in vec4 position;

//Defining uniform for our projection matrix
uniform mat4 u_MVP;


void main()
{
	gl_Position = u_MVP * position; //Multiply the projection matrix by the position
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 

uniform vec4 u_Color;


void main()
{
	//Getting the color from our texture with the right coord and assigning the pixels on the screen to it
	color = u_Color; 
	
};

