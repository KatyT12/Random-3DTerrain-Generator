#shader vertex
#version 330 core

layout (location = 0) in vec4 aPos;

layout (location = 2) in vec2 aTexCoords;

out VS_OUT{
     vec2 TexCoords;
}vs_out;



uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main()
{
    vs_out.TexCoords = aTexCoords;    
    gl_Position = proj * view * model * aPos;
}
   



#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

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

    if(tex >= 1)
    {
         FragColor = texture(myTexture.diffuse,TexCoords);
    }
    else
    {
         
         FragColor = vec4(material.diffuse,1.0);
         //FragColor = vec4(1.0,0.0,0.0,1.0);
    }
    
    
   
}


#shader geometry
#version 330 core
layout(triangles) in;
layout (triangle_strip,max_vertices=3) out;

in VS_OUT
{
    vec2 TexCoords;

}gs_in[];

uniform float time;


out vec2 TexCoords;


vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
}

vec4 Explode(vec3 normal, vec4 position)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) +1.0)/2.0) * magnitude;
    return position + vec4(direction,0.0);
}


void main()
{

    vec3 norm = GetNormal();

    gl_Position = Explode(norm,gl_in[0].gl_Position);
    
    TexCoords = gs_in[0].TexCoords;
    

    EmitVertex();

    gl_Position = Explode(norm,gl_in[1].gl_Position);
    
    TexCoords = gs_in[1].TexCoords;


    EmitVertex();

    gl_Position = Explode(norm,gl_in[2].gl_Position);

    TexCoords = gs_in[2].TexCoords;
  
    EmitVertex();
    EndPrimitive();



}




//origin: advanced/main9.cpp