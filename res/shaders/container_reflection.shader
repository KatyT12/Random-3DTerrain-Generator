#shader vertex
#version 330 core
	    
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;



//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

//mat 3 because we do not move the normal, it is a direction
uniform mat3 norm_model;


out vec3 Position;
out vec3 Normal; 

void main()
{
    Position = vec3(model * vec4(position,1.0)); //The position in world space of the fragment to give to the fragment shader
	gl_Position = proj * view * model * vec4(position,1.0); 
    
	Normal = norm_model * normal;
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 FragColor; 

uniform vec3 viewPos;
uniform samplerCube skybox; //Taking in our skybox


in vec3 Position;
in vec3 Normal;


void main()
{
    

    //Reflect
    vec3 norm = normalize(Normal);
    vec3 I =  normalize(Position - viewPos);
    vec3 R =  reflect(I,norm);
    
    vec3 reflect = 0.6 * vec3(texture(skybox,R));



    //Refract
    R = normalize(refract(I,norm,1.0/1.52));

    FragColor = vec4(texture(skybox,R).rgb*0.4 + reflect,1.0);

   
};

//origin advanced/main6.cpp