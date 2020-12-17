#shader vertex
#version 420 core //Note the opengl version
	    
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;



//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 v_TexCoord;

void main()
{
	gl_Position = proj * view * model * vec4(position,1.0); //Multiply the projection matrix by the position
	v_TexCoord = texCoord; 
}
   



#shader fragment
#version 420 core

layout (location = 0) out vec4 color; 
layout (depth_greater) out float gl_FragDepth;


uniform sampler2D u_Texture;

in vec2 v_TexCoord;


void main()
{

    
	color = texture(u_Texture,v_TexCoord);
    gl_FragDepth = gl_FragCoord.z + 0.04;

	
};


//Origin: main7.cpp