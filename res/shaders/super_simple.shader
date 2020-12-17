#shader vertex
#version 330 core
	    
layout (location = 0) in vec4 position;

//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;




void main()
{
	gl_Position = proj * view * model* position; //Multiply the projection matrix by the position
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 



void main()
{
	//Getting the color from our texture with the right coord and assigning the pixels on the screen to it
	color = vec4(0.0,1.0,0.0,1.0); 
	
};

