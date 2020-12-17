#shader vertex
#version 330 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 a_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform mat3 u_norm_model;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = proj * view * model * Position;
    FragPos = vec3(model * Position); //Fragment position in world space
    Normal = u_norm_model * a_normal;
};


#shader fragment
#version 330 core


//We make a material struct with the ambient,diffuse and specular. Also add a shinyness value. Last time we talked about specular strength a little.
//using a struct keeps it more organised
//We define a color vector for each of Phongs lighting components. The ambient defines what color the surface reflects under ambient light
//The ambient material vector defines what color the surface reflects under ambient lighting; this is usually the same as the surface's color. 
//The diffuse material vector defines the color of the surface under diffuse lighting. The diffuse color is (just like ambient lighting) 
//set to the desired surface's color. The specular material vector sets the color of the specular highlight on the surface (or possibly even reflect a surface-specific color). 
//Lastly, the shininess impacts the scattering/radius of the specular highlight

//Figuring out the right material 
//settings for an object is a difficult feat that mostly requires experimentation and a lot of experience. It's not that uncommon to completely destroy the visual quality of an object by a misplaced material. 

//Because we provided a material we don't need to multiply the objectColor uniform at the end because we are doing it (except for each seperate material
//component with each step in the phong model)

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
};


in vec3 Normal;
in vec3 FragPos;  

uniform Material material;
uniform vec3 u_viewPos;
uniform Light light;

//Some of this stuff is explained in main4.cpp
//Instead of multiplying by light color we use the set individual numbers for each component (ambient,diffuse,specular). Also don't need a strength to multiply it with aswell
//Instead of multiplying by the object color at the end, for each stage we multiply the individial set properties of it's material.

void main()
{   
    vec3 ambient = material.ambient*light.ambient;
   
    vec3 norm = normalize(Normal); 
    vec3 lightDir = normalize(light.position - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0); 
    vec3 diffuse = (diff*material.diffuse) * light.diffuse;  //multiply the diffuse by the material.diffuse    

    vec3 viewDir = normalize(u_viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir,norm);             
    
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = (ambient + diffuse + specular); 
    FragColor = vec4(result,1.0);
    
};
