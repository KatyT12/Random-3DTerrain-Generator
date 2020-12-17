#include "shader.h"
#include <iostream>
#include <fstream> 
#include <string> 
#include <sstream> 
#include "renderer.h"
#include <vector>

shader::shader()
:m_rendererID(0)
{}


shader::shader(const std::string& filepath, bool geometry)
	: m_filepath(filepath), m_rendererID(0) //m_filepath member is just for debugging purposes
{
	makeShader(filepath,geometry);
}	



shader::~shader()
{
    GLCall(glDeleteProgram(m_rendererID));
}

void shader::makeShader(const std::string& filepath, bool geometry)
{
	m_filepath = filepath;
	if(geometry)
	{
		ShaderProgramSource source = ParseShader(filepath,geometry);
 	   m_rendererID  = CreateShader(source.VertexSource,source.FragmentSource,source.GeometrySource);
	}
	else
	{
		ShaderProgramSource source = ParseShader(filepath);
    	m_rendererID  = CreateShader(source.VertexSource,source.FragmentSource);
	}
	

}
/* Took this from main.cpp. Don't need file path because that is a member but lets have it anyway*/
ShaderProgramSource shader::ParseShader(const std::string& filepath)
{

	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE =-1,VERTEX=0,FRAGMENT=1	
	
	};
	
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE; 
	
	while(getline(stream,line))
	{
		if(line.find("#shader") != std::string::npos) 
		{

			if(line.find("vertex") != std::string::npos) 
			{
				type = ShaderType::VERTEX;
			}

			else if(line.find("fragment") != std::string::npos) //If "fragment" in line
			{
				//set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n"; //Just adding the line to the end


		}
	}
	return { ss[0].str(),ss[1].str() }; //Returning in format of the struct
	
}


ShaderProgramSource shader::ParseShader(const std::string& filepath,bool geometry)
{

	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE =-1,VERTEX=0,FRAGMENT=1,GEOMETRY=2
	
	};
	
	std::string line;
	std::stringstream ss[3];
	ShaderType type = ShaderType::NONE; 
	
	while(getline(stream,line))
	{
		if(line.find("#shader") != std::string::npos) 
		{

			if(line.find("vertex") != std::string::npos) 
			{
				type = ShaderType::VERTEX;
			}

			else if(line.find("fragment") != std::string::npos) //If "fragment" in line
			{
				//set mode to fragment
				type = ShaderType::FRAGMENT;
			}
			else if(line.find("geometry") != std::string::npos)
			{
				type = ShaderType::GEOMETRY;
			}
		}
		else
		{
			ss[(int)type] << line << "\n"; //Just adding the line to the end


		}
	}
	return { ss[0].str(),ss[1].str(),ss[2].str() }; //Returning in format of the struct
	
}




unsigned int shader::CompileShader(unsigned int type,const std::string& source)
{
	unsigned int id = glCreateShader(type);
	
	
	const char* src = source.c_str(); //Could have also written &source[0]
	
	glShaderSource(id,1,&src,nullptr);
	glCompileShader(id);
	
	int result;
	glGetShaderiv(id,GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id,GL_INFO_LOG_LENGTH, &length);
	
		/* alloca returns void pointer so cast back to char**/
	
		char* message = (char*) alloca(length * sizeof(char));
	
		glGetShaderInfoLog(id,length, &length,message);
		
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex":"fragment" ) 
			<< "shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id); 
		return 0; 
	}
	
	
	return id;
};



/* Took this from main.cpp. */
unsigned int shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram(); 
	unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);

	glAttachShader(program,vs);
	glAttachShader(program,fs);
	glLinkProgram(program);
	glValidateProgram(program);

	//We get obj files which we don't need, because it is compiled now
	glDeleteShader(vs);
	glDeleteShader(fs);

	//Returns identifier for our program
	return program;

}

unsigned int shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
{
	unsigned int program = glCreateProgram(); 
	unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);
	unsigned int gs = CompileShader(GL_GEOMETRY_SHADER,geometryShader);


	glAttachShader(program,vs);
	glAttachShader(program,fs);
	glAttachShader(program,gs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);

	return program;


	
}


void shader::setUniform1i(const std::string &name, int value)
{
	GLCall(glUniform1i(getUniformLocation(name),value));

}

void shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{

    GLCall(glUniform4f(getUniformLocation(name),v0,v1,v2,v3));

}

void shader::setUniformMat4f(const std::string& name, const glm::mat4 matrix)
{
	//v means we are passing in an array so a float array
	//location, count, need to transpose? we do not since our matrix is column major, pointer to the float array
	GLCall(glUniformMatrix4fv(getUniformLocation(name),1,GL_FALSE,&matrix[0][0]));
}

void shader::setUniform1f(const std::string& name, const float value)
{
	GLCall(glUniform1f(getUniformLocation(name),value));
}

void shader::setUniformVec3f(const std::string& name,const glm::vec3 &value)
{
	GLCall(glUniform3fv(getUniformLocation(name),1,&value[0])); 
}

void shader::setUniformMat3f(const std::string& name, const glm::mat3 matrix)
{
	GLCall(glUniformMatrix3fv(getUniformLocation(name),1,GL_FALSE,&matrix[0][0]));
}

void shader::setUniformVec3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(getUniformLocation(name),v0,v1,v2));
}


void shader::setUniformVec2f(const std::string& name, float v0, float v1)
{
	GLCall(glUniform2f(getUniformLocation(name),v0,v1));
}

void shader::setUniformVec2f(const std::string& name,const glm::vec2 &vector)
{
	glUniform2fv(getUniformLocation(name),1,&vector[0]);
}

int shader::getUniformLocation(const std::string& name)
{
    //If find actually finds the name in it and does not reach the end
    if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }
    //Else actually retrieve the location
    
    GLCall(int location = glGetUniformLocation(m_rendererID,name.c_str()));
    if(location == -1)
    {
        //Sometimes it is valid to have a -1 as our location so not good to assert
        std::cout << "Warning uniform " << name << " does not exist" << std::endl;
    }
    m_UniformLocationCache[name] = location;
    
    return location;
}

void shader::Bind() const
{
    GLCall(glUseProgram(m_rendererID));
}

void shader::UnBind() const
{
    GLCall(glUseProgram(0));
}