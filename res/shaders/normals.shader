#shader vertex
#version 330 core

layout (location = 0) in vec4 aPos;



uniform mat4 model;
uniform mat4 view;


void main()
{
    gl_Position = view * model * aPos;
}
   



#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0,0.0,1.0,1.0);
}


#shader geometry
#version 330 core
layout(triangles) in;
layout (line_strip,max_vertices=6) out;

uniform mat4 proj;


const float MAGNITUDE = 0.2;

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
}



void GenLine(int index)
{
    gl_Position = proj * gl_in[index].gl_Position; //First just the point that we are at with no modifictations
    EmitVertex();
    //Second point on the line we add the normal multiplied by an appropriate magnitude so it is a good length
    gl_Position =  normalize(proj * gl_in[index].gl_Position + proj*vec4(GetNormal() * MAGNITUDE,1.0));
    EmitVertex();
    EndPrimitive();

}


void main()
{
    GenLine(0);
    GenLine(1);
    GenLine(2);
}

