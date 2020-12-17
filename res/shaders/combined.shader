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


in vec3 Normal;
in vec3 FragPos;  
in vec2 TexCoords;

//First we need to set the required variables that we minimally need for a directional light

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight 
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;

};


//Here we define the number of point lights in our scene for our array of point lights. This is a preprocessor statement


#define NR_POINT_LIGHTS 4

uniform DirLight dirlight;
uniform PointLight pointlights[NR_POINT_LIGHTS];
uniform SpotLight spotlight;

uniform Material material;
uniform vec3 u_viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{   
   vec3 norm = normalize(Normal);
   vec3 viewDir = normalize(u_viewPos - FragPos);

   vec3 result = CalcDirLight(dirlight,norm,viewDir);
   for(int i=0;i<NR_POINT_LIGHTS;i++)
   {
       result += CalcPointLight(pointlights[i],norm,FragPos,viewDir);
   };
   result += CalcSpotLight(spotlight,norm,FragPos,viewDir);

   FragColor = vec4(result,1.0);
    
};

//We can then pass it to a function. Returns a vec3
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir,normal),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir,normal),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

    float distance = length(light.position - fragPos); //Can't use lightDir because it is normalized
    float attenuation = 1.0/(light.constant + (light.linear * distance) + (light.quadratic * distance * distance));
   
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir,normal),0.0);

    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

    float distance = length(light.position-fragPos);
    float attenuation = 1.0/ (light.constant + (light.linear * distance) + (light.quadratic * distance * distance));
   
    float theta = dot(lightDir,normalize(-light.direction));
    float epilson = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epilson,0.0,1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular,TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;


    return (ambient + diffuse + specular);
}


//origin lab/main9.cpp