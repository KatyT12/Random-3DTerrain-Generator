#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Display{
    public:
        Display(){};
        
        bool initialize(int width, int height, std::string name)
        {
            this->width = width;
            this->height = height;
            this->name = name;

        /* Initialize the library */
            if (!glfwInit())
                return false;

            glfwWindowHint(GLFW_SAMPLES, 4);

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);  
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);   
            glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
            glViewport(0,0,width,height);

            window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
        
            if (!window)
            {
                glfwTerminate();
                return -1;
            }

            glfwMakeContextCurrent(window);

            glfwSwapInterval(1);
            

            if(glewInit() != GLEW_OK)
            {
                std::cout << "Error!" << std::endl;
            }	    

            printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));            
            return true;
        }

        void Clear(int options)
        {
            glClear(options);
        }

        bool shouldClose()
        {
            return glfwWindowShouldClose(window);
        }

        GLFWwindow* getWindow()
        {
            return window;
        }

        void SwapBuffers()
        {
            glfwSwapBuffers(window);
        }

        void PollEvents()
        {
            glfwPollEvents();
        }
        void Clean()
        {
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        inline int getHeight(){return height;}
        inline int getWidth(){return width;}
    private:
        int width;
        int height;
        std::string name;
        GLFWwindow* window;

};
