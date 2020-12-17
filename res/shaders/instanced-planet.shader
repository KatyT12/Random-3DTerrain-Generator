#shader vertex
#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix; //Our Instance array

out vec2 TexCoords;
out vec3 norm;

uniform mat4 view;
uniform mat4 proj;


void main()
{
    TexCoords = aTexCoords;    
    
    gl_Position = proj * view * instanceMatrix * aPos;
    norm = aNormal;    
}
   



#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 norm;

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

void main()
{    

    if(tex >= 1)
    {
        FragColor = texture(myTexture.diffuse,TexCoords);
   
  
    }
    else
    {
         FragColor = vec4(material.diffuse,1.0);

         
    }

}

//origin: advanced/asteroid.cpp