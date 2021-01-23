#include "renderer.h"
#include <iostream> 


void GLClearError()
{
	while(glGetError() != GL_NO_ERROR); 
}


bool GLLogCall(const char* function, const char* file, int line)
{
	
	while(GLenum error = glGetError()) //Will run as long as error is not 0
	{
		std::cout << "[OpenGL Error] (0x" << std::hex << error <<  "):" << function << " " << line 
			<<std::endl;
		return false; 
	}
	
	return true;
}



void renderer::Clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

