#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "SDL.h"
#include "SDL_opengl.h"

using namespace std;

class Shader {
public:
	unsigned int programID;

	Shader(string vectorPath, string fragmentPath)
	{
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;

		// ensuring that ifstream objects can throw exceptions
		vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			vShaderFile.open(vectorPath);
			fShaderFile.open(fragmentPath);

			stringstream vShaderStream;
			stringstream fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			fShaderFile.close();
			vShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure error)
		{
			cout << "ERROR: SHADER FILE READING UNSUCCESSFUL" << endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vShader, 1, &vShaderCode, NULL);
		glShaderSource(fShader, 1, &fShaderCode, NULL);

		glCompileShader(vShader);
		glCompileShader(fShader);

		int success;
		char errorLog[512];
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vShader, 512, NULL, errorLog);
			cout << "ERROR: Shader compilation failed" << endl << errorLog << endl;
		}

		programID = glCreateProgram();
		glAttachShader(programID, vShader);
		glAttachShader(programID, fShader);

		glLinkProgram(programID);

		glGetShaderiv(programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(programID, 512, NULL, errorLog);
			cout << "ERROR: Shader to program linking failed" << endl << errorLog << endl;
		}

		glDetachShader(programID, vShader);
		glDetachShader(programID, fShader);
		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}

	void use()
	{
		glUseProgram(programID);
	}

	// bool uniform
	void setUniform(const string& name, bool value)
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
	}

	// int and int vector uniforms
	void setUniform(const string& name, int value)
	{
		glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
	}

	void setUniform(const string& name, int value1, int value2)
	{
		glUniform2i(glGetUniformLocation(programID, name.c_str()), value1, value2);
	}

	void setUniform(const string& name, int value1, int value2, int value3)
	{
		glUniform3i(glGetUniformLocation(programID, name.c_str()), value1, value2, value3);
	}

	void setUniform(const string& name, int value1, int value2, int value3, int value4)
	{
		glUniform4i(glGetUniformLocation(programID, name.c_str()), value1, value2, value3, value4);
	}

	// float and float vector uniforms
	void setUniform(const string& name, float value)
	{
		glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
	}

	void setUniform(const string& name, float value1, float value2)
	{
		glUniform2f(glGetUniformLocation(programID, name.c_str()), value1, value2);
	}

	void setUniform(const string& name, float value1, float value2, float value3)
	{
		glUniform3f(glGetUniformLocation(programID, name.c_str()), value1, value2, value3);
	}

	void setUniform(const string& name, float value1, float value2, float value3, float value4)
	{
		glUniform4f(glGetUniformLocation(programID, name.c_str()), value1, value2, value3, value4);
	}

	void setUniform(const string& name, glm::vec2 vector)
	{
		glUniform2f(glGetUniformLocation(programID, name.c_str()), vector.x, vector.y);
	}

	void setUniform(const string& name, glm::vec3 vector)
	{
		glUniform3f(glGetUniformLocation(programID, name.c_str()), vector.x, vector.y, vector.z);
	}

	void setUniform(const string& name, glm::vec4 vector)
	{
		glUniform4f(glGetUniformLocation(programID, name.c_str()), vector.x, vector.y, vector.z, vector.w);
	}

	// matrices
	void setUniform(const string& name, glm::mat3 matrix)
	{
		glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void setUniform(const string& name, glm::mat4 matrix)
	{
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

};