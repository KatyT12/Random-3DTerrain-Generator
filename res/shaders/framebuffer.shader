#shader vertex
#version 330 core
	    
//The position is a vec2 because the frame buffer is 2D
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

//We are not doing NDC because we want the quad we are drawing to always be a the camera (atleast for this)


out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(position.x,position.y,0.0,1.0);
	v_TexCoord = texCoord; 
}
   
//The fragment shader is even more basic


#shader fragment
#version 330 core

layout (location = 0) out vec4 Fragcolor; 

uniform sampler2D screenTexture;
uniform int a;

in vec2 v_TexCoord;


void main()
{
	vec4 texColor = texture(screenTexture,v_TexCoord);
	
	Fragcolor = texColor;

	
};

//origin: advanced/main5.cpp
