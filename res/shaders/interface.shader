#shader vertex
#version 330 core
	    
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;



//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


out VS_OUT {
    vec2 v_TexCoord;
} vs_out;


void main()
{
	gl_Position = proj * view * model * vec4(position,1.0); //Multiply the projection matrix by the position
	vs_out.v_TexCoord = texCoord; 
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 

in VS_OUT {
    vec2 v_TexCoord;
} fs_in;                //Can give it a different name

uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture,fs_in.v_TexCoord);
	color = texColor;	
};

//origin: advanced/main7.cpp