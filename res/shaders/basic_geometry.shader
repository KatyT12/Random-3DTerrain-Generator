#shader vertex
#version 330 core
	    
layout (location = 0) in vec3 position;


//Defining uniform for our projection matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main()
{
	gl_Position = proj * view * model * vec4(position,1.0); //Multiply the projection matrix by the position
}
   



#shader fragment
#version 330 core

layout (location = 0) out vec4 color; 


void main()
{
    color = vec4(1.0,0.0,0.0,1.0);
};

#shader geometry
#version 330 core

layout(points) in;
layout (line_strip,max_vertices=2) out;

void main()
{
    gl_Position = gl_in[0].gl_Position + vec4(-0.1,0.0,0.0,0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.1,0.0,0.0,0.0);
    EmitVertex();

    EndPrimitive(); 

}

//At the start of the geometry shader we need to declare the type primitive input we are recieveing from the vertex shader
//We do this by declaring a layout specifier in front of the in keyword

//The input layout qualifier can take any of the following values

//points when drawing GL_POINTS(1)
//Lines when drawing GL_LINES(2)
//lines adjacency GL_LINES_ADJANCENCY or GL_STRIP_AJACENCY(4)
//triangles: GL_TRIANGLES,  GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN(3)
//triangles_adjacency: GL_TRIANGLES_ADJACENCY OR GL_TRIANGLE_STRIP_ADJACENCY(6)






