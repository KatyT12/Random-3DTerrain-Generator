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

out vec4 FragColor;
  
in vec3 Normal;
in vec3 FragPos;  

uniform vec3 u_objectColor;
uniform vec3 u_lightColor;
uniform vec3 lightPos;   
uniform vec3 u_viewPos;

void main()
{   
    //We do not care about the magnitude of the vector, only the direction when it comes to the normals and light position, and frag pos
    vec3 norm = normalize(Normal); //We normalize because we want them to be unit vectors
    vec3 lightDir = normalize(lightPos - FragPos); //minus the frag position (In world space) vector and the light position vector will get vector between two points
    float diff = max(dot(norm, lightDir), 0.0); //Get the dot product (angle difference. 1 is same angle and 0 is perpendicular) perpendicular will have the strongest light
    vec3 diffuse = diff * u_lightColor;        //Times the light color by the diff to get the appropriate strength pf the light color

    //Note that we negate the lightDir vector. The reflect function expects the first vector to point from the light 
    //source towards the fragment's position, but the lightDir vector is currently pointing the other way around: from the fragment towards the light source 
    //(this depends on the order of subtraction earlier on when we calculated the lightDir vector). 
    //To make sure we get the correct reflect vector we reverse its direction by negating the lightDir vector first. The second argument expects a normal vector so we supply the normalized norm vector. 
    //reflectDir is angle that the light is reflected off of the surface


    float specularStrength = 0.5; //1.0 is a bit too strong. Will look at properly setting these some other time.
    vec3 viewDir = normalize(u_viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir,norm);             
    //We first calculate the dot product between the view direction and the reflect direction (and make sure it's not negative) and then raise it to 
    //the power of 32. This 32 value is the shininess value of the highlight. The higher the shininess value of an object, 
    //the more it properly reflects the light instead of scattering it all around and thus the smaller the highlight becomes. 
    
    float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
    vec3 specular = specularStrength * spec * u_lightColor;


    //Get ambient strength. In an area there is always some light and this is the ambience with the current light in the are. Sort of hard to explain
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength*u_lightColor;
    vec3 result = (ambient + diffuse + specular)*u_objectColor; //add result to the ambience and multiply by the objects actual color
    FragColor = vec4(result,1.0);
    
    
     //vec4 col = vec4(u_lightColor * u_objectColor, 1.0);
     //FragColor = col;
};
