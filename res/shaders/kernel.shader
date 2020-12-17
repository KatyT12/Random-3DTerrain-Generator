#shader vertex
#version 330 core
	    
//The position is a vec2 because the frame buffer is 2D
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

//We are not doing NDC because we want the quad we are drawing to always be a the camera (atleast for this)


out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(position.x,position.y,0.0,1.0);
	v_TexCoord = texCoord; 
}
   

#shader fragment
#version 330 core

layout (location = 0) out vec4 Fragcolor; 

uniform sampler2D screenTexture;

in vec2 v_TexCoord;

const float offset = 1.0/300.0;



void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,offset),  //top left
        vec2(0.0f,offset),     //top center
        vec2(offset,offset),   //top right
        vec2(-offset,0.0f),    //center left
        vec2(0.0f,0.0f),       //center center
        vec2(offset,0.0f),     //center right 
        vec2(-offset,-offset), //bottom  left
        vec2(0.0f, -offset),    //bottom center
        vec2(offset,-offset)   //bottom right
    );

    float sharpenKernel[9] = float[](
        -1,-1,-1,
        -1, 9,-1,
        -1,-1,-1
    );

    float blurKernel[9] = float[](
        1.0/16, 2.0/16, 1.0/16,
        2.0/16, 4.0/16, 2.0/16,
        1.0/16, 2.0/16, 1.0/16 
    );

    float edgeKernel[9] = float[](
        1.0,1.0,1.0,
        1.0,-8.0,1.0,
        1.0,1.0,1.0
    );


    vec3 sampleTex[9];
    for(int i=0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture,v_TexCoord.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for(int i=0;i<9;i++)
    {
        col += sampleTex[i] * sharpenKernel[i];
    }
	
	Fragcolor = vec4(col,1.0);

	
};

//origin: advanced/main5.cpp