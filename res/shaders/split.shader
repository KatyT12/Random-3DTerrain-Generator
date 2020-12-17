#shader vertex
#version 330 core
	    
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
#version 330 core

layout (location = 0) out vec4 color; 

uniform sampler2D u_Texture;

in vec2 v_TexCoord;



void main()
{
    vec3 texCol = vec3(texture(u_Texture,v_TexCoord));

    if(gl_FragCoord.x < 480)
    {
        color= vec4(texCol * vec3(0.5,0.0,0.0),1.0);
    }
    else
    {
        color= vec4(texCol * vec3(0.0,0.5,0.0),1.0);
    }


	
};
