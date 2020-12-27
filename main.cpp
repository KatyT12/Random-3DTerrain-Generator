#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "include/renderer.h"
#include "include/shader.h"
#include "include/Texture.h"
#include "include/camera.h"

#include "include/random_terrain.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



void processInput(GLFWwindow *window);
void fillVBO();


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f),glm::vec3(0.0f,1.0f,0.0f),-90.0f,0.0f,10.0f);
float lastX = 960 / 2.0f;
float lastY = 540 / 2.0f;
bool firstMouse = true;


//timing
float deltaTime = 0.0f; 
float lastFrame = 0.0f; 


bool spacePressed = false;



void mouse_callback(GLFWwindow* window, double mouseX, double mouseY)
{
	if (firstMouse)
    {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    float xoffset = mouseX - lastX;
    float yoffset = lastY - mouseY; //reversed since y-coordinates go from bottom to top

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

    if (!glfwInit())
        return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);  
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);   
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
    //Antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);
	
    window = glfwCreateWindow(960, 540, "Depth testing", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }



    glfwMakeContextCurrent(window);
    
    //This should fit our monitors refresh rate or whatever
    glfwSwapInterval(1);
 
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,scroll_callback);


    if(glewInit() != GLEW_OK)
    {
	    std::cout << "Error!" << std::endl;
    }	    
	

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));

    glEnable(GL_MULTISAMPLE);


    
    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
    

	GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND)); 

    {
   
    Terrain terrain = Terrain("test.json");
    terrain.init();





   // shader NormalsShader("res/shaders/normals.shader",true);

   // shader terrainShader("res/shaders/basic.shader");

 
	glm::mat4 view = glm::lookAt(camera.Position,camera.Position + camera.Front ,camera.Up);	

	glm::mat4 proj = glm::perspective(glm::radians(camera.fov),960/(float)540,0.1f,200.0f);
	glm::mat4 model = glm::mat4(1.0f);	
    	

    //Telling the camera to try and detect collisions. This is very rough and only detects if there is a collision when the forward button is pressed (not any other button) but it is getting there
    
    
    camera.setTerrain(&terrain);



    while(!glfwWindowShouldClose(window)){


	    GLCall(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f),(float)glfwGetTime(),glm::vec3(1.0f,0.5f,.0f));

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(camera.Position,camera.Position + camera.Front ,camera.Up);
		glm::mat4 proj = glm::perspective(glm::radians(camera.fov),960/(float)540,0.1f,200.0f);
		
        
        terrain.terrainShader.Bind();
        terrain.terrainShader.setUniformMat4f("model",model);
        terrain.terrainShader.setUniformMat4f("proj",proj);
        terrain.terrainShader.setUniformMat4f("view",view);
        
        terrain.treeShader.Bind();
        terrain.treeShader.setUniformMat4f("view",view);
        terrain.treeShader.setUniformMat4f("proj",proj);
        
        
        terrain.Draw();

        if(spacePressed)
        {
            std::vector<glm::vec3> colors = {glm::vec3(1.0f,0.0f,1.0f),glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,0.0f,1.0f),glm::vec3(1.0f,0.0f,0.0f)};
            terrain.newColors(colors);
            spacePressed = false;
        }

        glfwSwapBuffers(window);

        glfwPollEvents();
	    processInput(window);


     }

	}


    

    glfwTerminate();
    return 0;
}

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
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {

        spacePressed = true;
       
    }



    //Just for debugging
    if(glfwGetKey(window,GLFW_KEY_0)==GLFW_PRESS)
    {
        std::cout << camera.Position.y << "\n";
    }
    

};


