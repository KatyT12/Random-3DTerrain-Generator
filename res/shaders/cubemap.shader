#shader vertex
#version 330 core
	    
layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 proj;

out vec3 v_TexCoords;

void main()
{
   
    v_TexCoords = position; 
	vec4 pos = proj * view * vec4(position,1.0);
    //Replacing the z with w so that it will be 1,0 (right at the end by the far plane) at the depth test because perspective division is after the vertex shader
    gl_Position = pos.xyww; 
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 FragColor; 

uniform samplerCube cubemap;

in vec3 v_TexCoords;


void main()
{
	vec4 texColor = texture(cubemap,v_TexCoords);
	
    FragColor = texColor;
	//FragColor = vec4(1.0,0,0.0,1.0);


};

//origin advanced/main6.cpp