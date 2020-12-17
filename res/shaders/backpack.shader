#shader vertex
#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 norm;
out vec3 FragPos;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 norm_model;

void main()
{
    TexCoords = aTexCoords;    
    FragPos = vec3(model * aPos);
    gl_Position = proj * view * model  * aPos;
    norm = vec3(norm_model * vec4(aNormal,1.0));
}
   



#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 norm;
in vec3 FragPos;


//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Texture {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;

};

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
};


uniform Material material;
uniform Texture myTexture;
uniform Light light; //Going to have a directional light
uniform vec3 u_viewPos;

uniform int tex;

//Lets just say there is a light at
void main()
{    

    if(tex >= 1)
    {
         vec3 ambient = vec3(texture(myTexture.diffuse,TexCoords))*light.ambient;
   
         vec3 norm = normalize(norm); 
       
         vec3 lightDir = normalize(-light.direction); 
         float diff = max(dot(norm, lightDir), 0.0); 
         vec3 diffuse = (diff*vec3(texture(myTexture.diffuse,TexCoords))) * light.diffuse;    

         vec3 viewDir = normalize(u_viewPos-FragPos);
         vec3 reflectDir = reflect(-lightDir,norm);             
    
         float spec = pow(max(dot(viewDir,reflectDir),0.0),myTexture.shininess);
         vec3 specular = vec3(texture(myTexture.specular,TexCoords)) * spec * light.specular;

         vec3 result = (ambient + diffuse + specular); 
         FragColor = vec4(result,1.0);
         //FragColor = texture(myTexture.diffuse, TexCoords);
    }
    else
    {
         vec3 ambient = material.diffuse*light.ambient;
   
         vec3 norm = normalize(norm); 
       
         vec3 lightDir = normalize(-light.direction); 
         float diff = max(dot(norm, lightDir), 0.0); 
         vec3 diffuse = diff * material.diffuse * light.diffuse;    

         vec3 viewDir = normalize(u_viewPos-FragPos);
         vec3 reflectDir = reflect(-lightDir,norm);             
    
         float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
         vec3 specular = material.specular * spec * light.specular;

         vec3 result = (ambient + diffuse + specular); 
         
         //FragColor = vec4(norm,1.0);
         //FragColor = vec4(normalize(FragPos),1.0);
         FragColor = vec4(result,1.0);
         //FragColor = vec4(material.diffuse,1.0);
    }
    
    //FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(1.0,1.0,1.0,1.0);
}

//origin: lab/main2.cpp