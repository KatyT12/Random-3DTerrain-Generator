
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include "renderer.h" //Need the macro and debugging funcitons
#include "vertexBuffer.h"
#include "indexBuffer.h"
#include "vertexBufferLayout.h"
#include "vertexArray.h"
#include "shader.h"
#include "Texture.h"
#include "camera.h"

/* Maths library we are using */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//include imgui
#include "vender/imgui/imgui.h"
#include "vender/imgui/imgui_impl_opengl3.h"
#include "vender/imgui/imgui_impl_glfw.h"

//https://learnopengl.com/Lighting/Basic-Lighting This link is helpful for explaining ambient, diffuse and specular light

/*Here i have created another va using the same cube positions but just translating and scaling it. This will be the light. It also has it's own shader because the fragment shader is slightly different.
 I may not need a seperate va because it literally uses the same vbo, layout and ebo but whatever. The positions also have texture coordinates which are not used at all by anything
 (but still set in layout so there are no errors with the stride and all that) I just can't be bothered to remove them :/ maybe I will want to use them later
 
 Update: the old UV coordinates I have replaced with normals which are basically vectors which will be perpendicular to the face of the cube. These have 3 components and are the same for each face
 so maybe i could do some sort of abstraction or somwthing rather than giving the same normals to each 4 vertices. I had to change the index buffer as well and the vertex buffer has increased in size
 
 
 The model matrix does not always work on normals. To transform the normal vectors we use a normal matrix. The normal matrix is 'the transpose of the inverse of the upper-left 3x3 part of the model matrix' 
 go to the link above where it sort of explains why (and it provides links to articles that have more explanation
 
 
 We chose to do the lighting calculations in world space, but most people tend to prefer doing lighting in view space. 
 An advantage of view space is that the viewer's position is always at (0,0,0) so you already got the position of the viewer for free. However, I find calculating lighting in world space more 
 intuitive for learning purposes. If you still want to calculate lighting in view space you want to transform all the relevant vectors with the view matrix as well
(don't forget to change the normal matrix too). */



void processInput(GLFWwindow *window);

/* Global variables */

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 960 / 2.0f;
float lastY = 540 / 2.0f;
bool firstMouse = true;



//timing
float deltaTime = 0.0f; //change in time since last frame
float lastFrame = 0.0f; //The time of the last frame




void mouse_callback(GLFWwindow* window, double mouseX, double mouseY)
{
	if (firstMouse)
    {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    float xoffset = mouseX - lastX;
    float yoffset = lastY - mouseY; // reversed since y-coordinates go from bottom to top

    lastX = mouseX;
    lastY = mouseY;

    camera.processMouse(xoffset, yoffset);
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
     camera.processScroll(yoffset);
}



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);  //major version 3 opengl 3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);   
        //opengl profile to be core rather tha compatibility OPENGL_COMPAT_PROFILE//minor version 3 opengl 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
	
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "Basic lighting", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }


    //glfwSetKeyCallback(window,key_callback);




    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    //This should fit our monitors refresh rate or whatever
    glfwSwapInterval(1);
 
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,scroll_callback);


    //Remeber that glewInit must be done after glfwInit and after a valid glfw rendering context
    if(glewInit() != GLEW_OK)
    {
	std::cout << "Error!" << std::endl;
    }	    
	
    GLCall(glEnable(GL_DEPTH_TEST));


    //Print version of opengl
    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
    

    {
   
    float positions[] = {
	
	 //square 1
    -0.5f,-0.5f, -0.5f, 0.0f, 0.0f,-1.0f,
	 0.5f,-0.5f, -0.5f, 0.0f, 0.0f,-1.0f,
	 0.5f, 0.5f, -0.5f, 0.0f, 0.0f,-1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,-1.0f,

	//square 2
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f,0.0f,//bottom right
	0.5f,  0.5f, -0.5f, 1.0f, 0.0f,0.0f,//top right
	0.5f, -0.5f,  0.5f, 1.0f, 0.0f,0.0f, //bottom left
	0.5f,  0.5f,  0.5f, 1.0f, 0.0f,0.0f,//top left	
    
    //square 3
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,1.0f,
	 0.5f, -0.5f, 0.5f, 0.0f, 0.0f,1.0f,
	 0.5f,  0.5f, 0.5f, 0.0f, 0.0f,1.0f,
	-0.5f,  0.5f, 0.5f, 0.0f, 0.0f,1.0f,

    //sqare on right
	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f,0.0f, //bottom right
	-0.5f,  0.5f, 0.5f, -1.0f, 0.0f,0.0f, //top right
    -0.5f, -0.5f,-0.5f, -1.0f, 0.0f,0.0f, //bottom left
    -0.5f,  0.5f,-0.5f, -1.0f, 0.0f,0.0f, //top left

	//square 5 (bottom square)
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,0.0f, //top left
 	 0.5f, -0.5f, -0.5f,0.0f,-1.0f,0.0f,//top right
     0.5f, -0.5f, 0.5f, 0.0f,-1.0f,0.0f, //bottom right
     -0.5f, -0.5f, 0.5f,0.0f,-1.0f,0.0f,//bottom left

	//square 5 (top square)
	 -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,0.0f, //bottom left
	  0.5f, 0.5f, 0.5f, 0.0f, 1.0f,0.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f,0.0f, //top right
      -0.5f, 0.5f,-0.5f, 0.0f, 1.0f,0.0f, //top left
    }; 
    
    
    unsigned int indices[] = 
    {
    	0,1,2,
	    2,3,0,

	    5,4,6,
	    5,7,6,
	
	    8,9,10,
	   10,11,8,

   	12,13,15,
	15,14,12,

	16,17,18,
	18,19,16,

	20,21,22,
	22,23,20

    
    };



	GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND)); //Also need to enable it




	/* Create vertex array object, create vertex buffer, add vertex buffer to vertex array object. Note you can have multiple buffers in  a vertex array*/
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    
    vertexArray va;    
	vertexBuffer vb(positions,6*24*sizeof(float));
	vertexBufferLayout layout;
	
	
	layout.Push<float>(3);
	layout.Push<float>(3);

	
	//Binding everything and actually setting the layout
	va.AddBuffer(vb,layout);


	/* Create a shader and set a uniform. Make sure you bind it before setting the uniform*/
	shader sha = shader("res/shaders/basic_light.shader");

	sha.Bind();
    
    sha.setUniformVec3f("lightPos",lightPos);

    glm::vec3 obj_col = glm::vec3(1.0f, 0.5f, 0.31f);
    glm::vec3 light_col = glm::vec3(1.0f, 1.0f, 1.0f);


    indexBuffer ib(indices,36);
    va.UnBind();
	vb.UnBind();   
    ib.UnBind();
	
    sha.UnBind();

    
    vertexArray lightVA;
    lightVA.Bind();
   
    lightVA.AddBuffer(vb,layout);
    
    
    //Two different shaders
    shader l_sha("res/shaders/lshader.shader");
    l_sha.Bind();
    ib.Bind();

    lightVA.UnBind();
    vb.UnBind();
    ib.UnBind();



    l_sha.UnBind();
    
	
	
	//Declaring the renderer
	renderer renderer;

	
	float perspect = 60.0f;


	/* The third parameter sort of determines the rotation of the camera. It is saying that for the camera
	 * y is up so it will be rotated how you would expect.  Anyways in this example the camera is not
	 * necessarily going to be pointed at the origin so we add camera front to the position and that is
	 * where it is pointing*/
	
	glm::mat4 view = glm::lookAt(camera.Position,camera.Position + camera.Front ,camera.Up);	

	glm::mat4 proj = glm::perspective(glm::radians(camera.fov),960/(float)540,0.1f,100.0f);
	glm::mat4 model = glm::mat4(1.0f);	
    	

    while(!glfwWindowShouldClose(window)){

	    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
		renderer.Clear();

		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),(float)glfwGetTime(),glm::vec3(1.0f,0.5f,.0f));

        glm::mat4 model = glm::mat4(1.0f);
		//model = model * rotation;
		glm::mat4 view = glm::lookAt(camera.Position,camera.Position + camera.Front ,camera.Up);
		glm::mat4 proj = glm::perspective(glm::radians(camera.fov),960/(float)540,0.1f,100.0f);
		
        glm::mat3 normal_model = glm::mat3(transpose(inverse(model)));
		
		
		sha.Bind();   
        
        sha.setUniformMat3f("u_norm_model",normal_model);

        sha.setUniformMat4f("model",model);
        sha.setUniformMat4f("view",view);
        sha.setUniformMat4f("proj",proj);
        sha.setUniformVec3f("u_viewPos",camera.Position);

		
       
		//glm::mat4 mvp = proj * view * model;
			
		


		//sha.setUniformMat4f("u_MVP",mvp);
        sha.setUniformVec3f("u_objectColor", obj_col);
        sha.setUniformVec3f("u_lightColor",  light_col); 
		renderer.Draw(va,ib,sha);

        sha.UnBind();

		
        model = glm::translate(glm::mat4(1.0f), lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); 


        glm::mat4 mvp = proj * view * model;
        l_sha.Bind();
        l_sha.setUniformMat4f("u_MVP",mvp);
        renderer.Draw(lightVA,ib,l_sha);
        l_sha.UnBind();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
	processInput(window);


     }
    //Shader will delete itself once the scope ends
	}

    glfwTerminate();
    return 0;
}

//May or may not be used
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACK, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);

};