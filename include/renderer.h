#pragma once 

#include <GL/glew.h>
#include <signal.h> //For assert to work with GNU compiler

#include "shader.h"


#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x,__FILE__,__LINE__))

#define ASSERT(x) if(!(x)) raise(SIGTRAP);



void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class renderer 
{
	private:


	public:
		void Clear() const;



};