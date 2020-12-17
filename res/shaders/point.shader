#shader vertex
#version 330 core
	    
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;



//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;



void main()
{
	gl_Position = proj * view * model * vec4(position, 1.0);    
    gl_PointSize = 1.0/gl_Position.z;    
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 

uniform sampler2D u_Texture;


void main()
{


	color = vec4(1.0f,0.0f,0.0f,1.0f);

	//color = vec4(vec3(gl_FragCoord.z),1.0);
	
	//color = texColor; 
	
};
