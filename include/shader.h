#pragma once

#include <unordered_map> //Use this to cache location of uniform
#include <string>
#include <glm/glm.hpp>

/* Note this code is very simple and would normally be more complex */


struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
    std::string GeometrySource;

};


class shader
{
    private:
        std::string m_filepath; //save filepath for debug purposes
        unsigned int m_rendererID;
        //cacheing for uniforms
        std::unordered_map<std::string,int> m_UniformLocationCache;

    public:

        shader();
        shader(const std::string& filepath, bool geometry = false);
        void makeShader(const std::string& filepath, bool geometry = false);
        ~shader();
        //Is actually called glUseProgram but will just call it bind to make the API consistant
        void Bind() const;
        void UnBind() const;

        //Passing in 4 because it is a vec4
        void setUniform4f(const std::string& name,float v0, float v1, float v2, float v3);
        void setUniform1i(const std::string& name, int value); 
        void setUniformMat4f(const std::string& name, const glm::mat4 matrix);
        void setUniform1f(const std::string& name, const float value); 
        
        void setUniformVec3f(const std::string& name, const glm::vec3 &value);
        void setUniformVec3f(const std::string& name, float v0, float v1, float v2);

        void setUniformVec2f(const std::string& name, float v0, float v1);
        void setUniformVec2f(const std::string& name, const glm::vec2 &vector);

        void setUniformMat3f(const std::string& name, const glm::mat3 matrix);

        int getUniformLocation(const std::string& name);
        
        inline unsigned int getShaderID() const {return m_rendererID;}
        
        bool proj_and_view_ubo = false;
        

        /* All these are basically copied from main.cpp*/
        unsigned int CompileShader(unsigned int type, const std::string& source);
        
        ShaderProgramSource ParseShader(const std::string& filepath);
        ShaderProgramSource ParseShader(const std::string& filepath, bool geometry);
   
        unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader);
        unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader,const std::string& GeometryShader);
};