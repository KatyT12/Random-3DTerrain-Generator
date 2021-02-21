#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "include/renderer.h"
#include "include/shader.h"
#include "include/Texture.h"
#include "include/camera.h"
#include "include/UniformBuffer.h"
#include "include/sceneRenderer.h"
#include "include/Cubemap.h"
#include "include/model.h"
#include "include/Display.h"

#include "include/random_terrain.h"
#include "include/Water.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void processInput(GLFWwindow *window);
void resetColors(std::vector<glm::vec3>& oldColors);
void setNonUboShaders(std::vector<shader*>& shaders,glm::mat4& proj,glm::mat4& view);
void setLightingProperties(std::vector<shader*> shaders);
GLFWwindow* setupContext(Display& display);

bool spacePressed = false;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f),glm::vec3(0.0f,1.0f,0.0f),-90.0f,0.0f,10.0f);

float lastX = 960 / 2.0f;
float lastY = 540 / 2.0f;
bool firstMouse = true;


bool mousePressed = false;

//timing
float deltaTime = 0.0f; //change in time since last frame
float lastFrame = 0.0f; //The time of the last frame

glm::vec3 light_direction(0.6f,-0.6f,0.3f);
glm::vec3 light_ambient(0.2f,0.2f,0.2f);
glm::vec3 light_diffuse(0.5f,0.5f,0.5f);
glm::vec3 light_specular(0.4f,0.4f,0.4f);


void mouse_callback(GLFWwindow* window, double mouseX, double mouseY)
{
    if(!mousePressed)
    {
        return;
    }

 
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
      
        mousePressed = true;
        firstMouse = true;
    }
    else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        mousePressed = false;
       
    }

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
     camera.processScroll(yoffset);
}

int main(int argc, char* argv[])
{
    Display display;
    GLFWwindow* window = setupContext(display);

    {
   
    std::vector<glm::vec3> colors;
    resetColors(colors);

    std::vector<std::string> faces{
        "res/textures/skybox/wrath_ft.jpg",
        "res/textures/skybox/wrath_bk.jpg",
        "res/textures/skybox/wrath_up.jpg",
        "res/textures/skybox/wrath_dn.jpg",
        "res/textures/skybox/wrath_rt.jpg",
        "res/textures/skybox/wrath_lf.jpg",

    };

    Cubemap cubemap(GL_TEXTURE_2D,DISPLAY_WIDTH/2,DISPLAY_HEIGHT/2,faces,GL_CLAMP_TO_EDGE);
    cubemap.initBuffer();


    Terrain terrain = Terrain("test.json");
    terrain.init();
    //Telling the camera to try and detect collisions. This is very rough and only detects if there is a collision when the forward button is pressed (not any other button) but it is getting there
    camera.setTerrain(&terrain);

    Model playerModel("res/models/planeCentered.obj");
    shader playerModelShader("res/shaders/backpack.shader");

    setLightingProperties({&terrain.terrainShader,&playerModelShader});

    glm::mat4 model;
    glm::mat4 view = glm::lookAt(camera.Position,camera.Position + camera.Front ,camera.Up);
    glm::mat4 proj = glm::perspective(glm::radians(camera.fov),960/(float)540,0.1f,200.0f);

    UniformBuffer proj_and_view(2*sizeof(glm::mat4));
    std::string name = "proj_and_view";
    
    std::vector<uint32_t> shaderids;
    for(unsigned int s : terrain.uboShaders)
    {
        shaderids.push_back(s);
    }
    shaderids.push_back(playerModelShader.getShaderID());
    proj_and_view.BindShaders(shaderids,0,name,2*sizeof(glm::mat4));
    proj_and_view.UpdateBufferPoint(0,&proj[0][0],sizeof(glm::mat4),0);
    proj_and_view.UpdateBufferPoint(0,&view[0][0],sizeof(glm::mat4),sizeof(glm::mat4));

    sceneRenderer renderer;
    renderer.setCameraPosition(camera.Position);
    renderer.setUniformBuffer(&proj_and_view);
    renderer.setTerrain(&terrain);
    renderer.setCubeMap(&cubemap);
    renderer.setCamera(&camera);
    
    renderer.setPlayerModel(&playerModel);
    renderer.setPlayerModelShader(&playerModelShader);

    while(!display.shouldClose()){
        display.Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        
        model = glm::mat4(1.0f);
        view = glm::lookAt(camera.Position,camera.Position + camera.Front ,camera.Up);
		proj = glm::perspective(glm::radians(camera.fov),960/(float)540,0.1f,200.0f);
		
        glm::vec3 look = camera.Front;
        look.y = 0.0f;
        glm::mat4 rotation = inverse(glm::lookAt(glm::vec3(camera.boxPos), glm::vec3(camera.boxPos) + look, glm::vec3(0.0f,1.0f,0.0f)));
        rotation = rotate(rotation, 0.5f * (float) glm::radians(-180.0f), glm::vec3(0.0f,1.0f,0.0f));
        model = rotation;
        
        playerModelShader.Bind();
        playerModelShader.setUniformMat4f("model",model);
        
        renderer.setproj(proj);
        renderer.setView(view);
        renderer.DrawScene();
  
        if(spacePressed)
        {
            resetColors(colors);
            terrain.newColors(colors);
            spacePressed = false;
        }

        display.SwapBuffers();
        display.PollEvents();
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
        camera.moveObj(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveObj(BACK, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveObj(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveObj(RIGHT, deltaTime);
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.moveObj(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        camera.moveObj(DOWN, deltaTime);

};

 void resetColors(std::vector<glm::vec3>& oldColors)
    {
        for(int i=0; i < 15; i++)
        {
            oldColors.push_back(glm::vec3(randdouble(0,1),randdouble(0,1),randdouble(0,1)));
        }
    }

 void setNonUboShaders(std::vector<shader*>& shaders,glm::mat4& proj,glm::mat4& view)
 {
     for(int i=0;i<shaders.size();i++)
     {
        shaders[i]->Bind();
        shaders[i]->setUniformMat4f("proj",proj);
        shaders[i]->setUniformMat4f("view",view);
        shaders[i]->UnBind();
     }
 }


void setLightingProperties(std::vector<shader*> shaders)
{
    for(shader* s : shaders)
    {
        s->Bind();
        s->setUniformVec3f("light.direction",glm::vec3(light_direction));
        s->setUniformVec3f("light.ambient",light_ambient);
        s->setUniformVec3f("light.diffuse",light_diffuse);
        s->setUniformVec3f("light.specular",light_specular);
        s->UnBind();

    }
}


GLFWwindow* setupContext(Display& display)
{
    display.initialize(DISPLAY_WIDTH,DISPLAY_HEIGHT,"Random Terrain");
    GLFWwindow* window = display.getWindow();
 
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetScrollCallback(window,scroll_callback);
    glfwSetMouseButtonCallback(window,mouse_button_callback);

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LESS));

    glEnable(GL_MULTISAMPLE);
    
    
	GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND)); 

    return window;
}