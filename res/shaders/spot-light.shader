#shader vertex
#version 330 core

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform mat3 u_norm_model;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
    gl_Position = proj * view * model * Position;
    FragPos = vec3(model * Position); //Fragment position in world space
    Normal = u_norm_model * a_normal;
    TexCoords = tex_coords;
};


#shader fragment
#version 330 core


out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;  //We want both the position and the direction
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float CutOff; //Our cut off angle phi
    float outerCutOff;
};


in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

uniform Material material;
uniform vec3 u_viewPos;
uniform Light light;


void main()
{   
    //Here we calculate the attenuation
    float distance = length(light.position - FragPos); //Magnitude of the vector of fragment to light position. So the distance.
    float attenuation = 1.0 / (light.constant + (light.linear*distance) + (light.quadratic*distance*distance));

    //ambient
    vec3 ambient = vec3(texture(material.diffuse,TexCoords))*light.ambient;
   
    //diffuse
    vec3 norm = normalize(Normal); 
    vec3 lightDir = normalize(light.position - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0); 
    vec3 diffuse = (diff*vec3(texture(material.diffuse,TexCoords))) * light.diffuse;  

    //specular
    vec3 viewDir = normalize(u_viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir,norm);             
    
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular = vec3(texture(material.specular,TexCoords)) * spec * light.specular;
   

    //Calculate theta and if it is inbetween give it appropriate value
    float theta = dot(lightDir,normalize(-light.direction));
    //Theta should be smaller but because we are representing the cosine the bigger the angle(up to 90 degrees) the smaller cos is
    
    //epilson is the what theta - outer angle is divided by
    float epilson = light.CutOff - light.outerCutOff;
    //Don't need if else if we properly clamp the values
    float intensity = clamp((theta - light.outerCutOff) / epilson, 0.0,1.0); 

    diffuse *= intensity;
    specular *= intensity;

    

    //alltogether
        
    //Will leave ambient unnafected so we always have a little light
    diffuse *= attenuation;
    specular *= attenuation;
    ambient *= attenuation;
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result,1.0);
        
        
    
};
