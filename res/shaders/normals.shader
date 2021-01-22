#shader vertex
#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 2) in vec3 aNormal;


out VS_OUT{
     vec3 normal;
}vs_out;



uniform mat4 model;
uniform mat4 view;



void main()
{
    vs_out.normal = normalize(aNormal);
    gl_Position =  view * model * aPos;
}
   



#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0,1.0,0.0,1.0);
}


#shader geometry
#version 330 core
layout(triangles) in;
layout (line_strip,max_vertices=6) out;

in VS_OUT
{
    vec3 normal;

}gs_in[];


const float MAGNITUDE = 0.4;

uniform mat4 proj;


void GenLine(int index)
{
    gl_Position = proj * gl_in[index].gl_Position; //First just the point that we are at with no modifictations
    EmitVertex();
    //Second point on the line we add the normal multiplied by an appropriate magnitude so it is a good length
    gl_Position = normalize(proj * (gl_in[index].gl_Position + (vec4(gs_in[index].normal * MAGNITUDE,0.0))));
    EmitVertex();
    EndPrimitive();

}


void main()
{
    GenLine(0);
    GenLine(1);
    GenLine(2);
}