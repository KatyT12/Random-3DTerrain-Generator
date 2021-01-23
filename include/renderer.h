#pragma once 

#include <GL/glew.h>
#include <signal.h> //For assert to work with GNU compiler
#include <string>


void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x,__FILE__,__LINE__))

#define ASSERT(x) if(!(x)) raise(SIGTRAP);

class renderer
{
	public:
		void Clear() const;
};