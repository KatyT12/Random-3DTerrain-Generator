#shader vertex
#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 modelMatrix;

out vec2 TexCoords;
out vec3 norm;

layout(std140) uniform proj_and_view
{
    mat4 proj;
    mat4 view;
};



uniform mat4 norm_model;


void main()
{
    TexCoords = aTexCoords;    
    
    gl_Position = proj * view * modelMatrix  * aPos;
    norm = vec3(norm_model * vec4(aNormal,1.0));
}
   



#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 norm;



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


uniform Material material;
uniform Texture myTexture;


uniform int tex;

//Lets just say there is a light at
void main()
{    

    if(tex == 1)
    {
        FragColor = texture(myTexture.diffuse,TexCoords);
  
    }
    else if(tex == 0)
    {
         FragColor = vec4(material.diffuse,1.0);

         
    }
    
    //FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = texture(texture_diffuse1, TexCoords);
    //FragColor = vec4(1.0,1.0,1.0,1.0);
}

//origin: lab/main2.cpp