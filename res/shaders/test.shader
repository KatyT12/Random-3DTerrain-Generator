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
	gl_Position = proj *  model * vec4(position,1.0); //Multiply the projection matrix by the position
	v_TexCoord = texCoord; 
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 

uniform sampler2D u_Texture;

in vec2 v_TexCoord;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	vec4 texColor = texture(u_Texture,v_TexCoord);
	

    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    color = vec4(vec3(depth), 1.0);
	
	if(texColor.a < 0.1)
	{
		discard;
	}



	color = texColor;

	//color = vec4(vec3(gl_FragCoord.z),1.0);
	
	//color = texColor; 
	
};
