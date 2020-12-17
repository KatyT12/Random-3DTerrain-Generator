#shader vertex
#version 330 core
	    
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 col;

//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out VS_OUT{
    vec3 color;
}vs_out;

void main()
{
	gl_Position = proj * view * model * vec4(position,1.0); //Multiply the projection matrix by the position
    vs_out.color = col;
}
   
#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 

in vec3 fColor;

void main()
{
    color = vec4(fColor,1.0);
};

#shader geometry
#version 330 core

layout(points) in;
layout (triangle_strip,max_vertices=5) out;

out vec3 fColor;  

in VS_OUT {
    vec3 color;
}gs_in[];


void buildHouse(vec4 position)
{
    fColor = vec3(1.0,1.0,1.0);
    gl_Position = position + vec4(0.0,0.4,0.0,0.0); //Top
    EmitVertex();

    fColor = gs_in[0].color;
    gl_Position = position + vec4(-0.2,0.2,0.0,0.0); //Top left
    EmitVertex();
    gl_Position = position + vec4(0.2,0.2,0.0,0.0); //Top right
    EmitVertex();
    gl_Position = position + vec4(-0.2,-0.2,0.0,0.0); //Bottom left
    EmitVertex();
    gl_Position = position + vec4(0.2,-0.2,0.0,0.0); //bottom right
    EmitVertex();
    EndPrimitive();
}



void main()
{
    buildHouse(gl_in[0].gl_Position);

}