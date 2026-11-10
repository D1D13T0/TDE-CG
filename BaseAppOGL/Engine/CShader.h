#pragma once

#pragma warning(disable : 4101)

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class CShader
{
public:
    CShader()
    {
		ID = 0;
    }

    ~CShader()
    {
        for (auto& pair : shaderPrograms) {
            glDeleteProgram(pair.second);
        }
        shaderPrograms.clear();
        ID = 0;
    }

    // Constructor reads and builds the shader from source code
    GLuint LoadShader(const std::string& name, const char* vertexSource, const char* fragmentSource, const char* geometrySource = NULL, bool fromFile = true)
    {
		bool alreadyExists = shaderPrograms.find(name) != shaderPrograms.end();
		if (alreadyExists)
		{
			std::cout << "Shader '" << name << "' already exists. Removing old shader before loading new one." << std::endl;
			RemoveShader(name);
		}

        int iVS = 0, iFS = 0, iGS = 0, iProgram = 0;
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;

        if (fromFile)
        {
            // Read from files
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;
            std::ifstream gShaderFile;
            
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            if (geometrySource != NULL)
                gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            
            try
            {
                vShaderFile.open(vertexSource);
                fShaderFile.open(fragmentSource);
                
                if (geometrySource != NULL) 
                    gShaderFile.open(geometrySource);
				else 
                    geometryCode = ""; // No geometry shader provided

                std::stringstream vShaderStream, fShaderStream, gShaderStream;
                
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();
                if (geometrySource != NULL) 
                    gShaderStream << gShaderFile.rdbuf();
                
                vShaderFile.close();
                fShaderFile.close();
                gShaderFile.close();
                
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
                if (geometrySource != NULL)  
                    geometryCode = gShaderStream.str();
            }
            catch (std::ifstream::failure& e)
            {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
            }
        }
        else
        {
            // Use source code directly
            vertexCode = vertexSource;
            fragmentCode = fragmentSource;
			geometryCode = geometrySource;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* gShaderCode = geometryCode.c_str();

        // Compile shaders
        unsigned int vertex, fragment, geometry;

        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        iVS = checkCompileErrors(vertex, "VERTEX");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        iFS = checkCompileErrors(fragment, "FRAGMENT");

		// Geometry Shader (if provided)
        if (geometrySource != NULL)
        {
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            iGS = checkCompileErrors(geometry, "GEOMETRY");
        }

        // Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometrySource != NULL) 
            glAttachShader(ID, geometry);

        glLinkProgram(ID);
        iProgram = checkCompileErrors(ID, "PROGRAM");

        // Delete shaders (they're linked into our program now and no longer necessary)
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometrySource != NULL) 
            glDeleteShader(geometry);

        if (iVS != 0 || iFS != 0 || iGS != 0 || iProgram != 0)
        {
            std::cerr << "ERROR: Failed to load shader: '" << name << "'" << std::endl;
			exit(EXIT_FAILURE);
		}
        else
        {
            // Store the program
            shaderPrograms[name] = ID;
            std::cout << "Successfully loaded shader: '" << name << "'" << std::endl;
        }

        return ID;
    }

    
    // Use/activate the shader
    void Use(const std::string& name)
    {
        auto it = shaderPrograms.find(name);
        if (it != shaderPrograms.end()) 
        {
            ID = it->second;
            glUseProgram(ID);
        }
        else 
        {
            std::cerr << "ERROR: Shader '" << name << "' not found!" << std::endl;
        }
    }


    GLuint GetProgram(const std::string& name) const 
    {
        auto it = shaderPrograms.find(name);
        if (it != shaderPrograms.end()) 
        {
            return it->second;
        }
        std::cerr << "ERROR: Shader '" << name << "' not found!" << std::endl;
        return 0;
    }

    bool HasShader(const std::string& name) const 
    {
        return shaderPrograms.find(name) != shaderPrograms.end();
    }

    void RemoveShader(const std::string& name) 
    {
        auto it = shaderPrograms.find(name);
        if (it != shaderPrograms.end()) 
        {
            glDeleteProgram(it->second);
            shaderPrograms.erase(it);
            std::cout << "Removed shader: '" << name << "'" << std::endl;
        }
    }

    void ClearAllShaders() 
    {
        for (auto& pair : shaderPrograms) 
        {
            glDeleteProgram(pair.second);
        }
        shaderPrograms.clear();
        ID = 0;
    }

    // Utility uniform functions
    void SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void SetVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void SetVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void SetMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void SetMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    GLuint ID;
    std::unordered_map<std::string, GLuint> shaderPrograms;


    // Utility function for checking shader compilation/linking errors
    int checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
                          << infoLog << "\n -- --------------------------------------------------- -- " 
                          << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
                          << infoLog << "\n -- --------------------------------------------------- -- " 
                          << std::endl;
            }
        }

		return success ? 0 : -1;
    }
};
