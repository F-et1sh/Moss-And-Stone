#include "forpch.h"
#include "Shader.h"

void FE2D::Shader::Release() {
	glDeleteProgram(m_Program);
}

void FE2D::Shader::Initialize(const std::wstring& file_path) {

	this->createProgram();

	const std::filesystem::path path = file_path;

	const std::filesystem::path vertex_path = path.wstring() + L".vert";
	const std::filesystem::path fragment_path = path.wstring() + L".frag";

	const bool vert_init = this->loadFromFile(vertex_path, FE2D::Shader::VERTEX);
	const bool frag_init = this->loadFromFile(fragment_path, FE2D::Shader::FRAGMENT);

	if (vert_init && frag_init) {
		this->LinkShader();
		return;
	}

	// if a user will use path with an extension program anyway can work
	// Example : SomeShader.shader ==> SomeShader.shader.vert & SomeShader.shader.frag
	// but if it doesn't wokred throw an error

	this->Release();

	if (path.has_extension())
		FOR_RUNTIME_ERROR(std::string("\nFailed to Initialize a shader") +
			std::string("\nYou tried to use one path for both shaders (vertex and fragment)") +
			std::string("\nDelete the extension of the path") +
			std::string("\nSource Path : ") + path.string() +
			std::string("\nVertex Path : ") + vertex_path.string() +
			std::string("\nFragment Path : ") + fragment_path.string());

}

void FE2D::Shader::Initialize(const std::wstring& vert_path, const std::wstring& frag_path) {

	this->createProgram();

	const std::filesystem::path vertex_path = vert_path;
	const std::filesystem::path fragment_path = frag_path;

	bool vert_init = this->loadFromFile(vertex_path, FE2D::Shader::VERTEX);
	bool frag_init = this->loadFromFile(fragment_path, FE2D::Shader::FRAGMENT);

	if (vert_init && frag_init) {
		this->LinkShader();
		return;
	}

	this->Release();

	FOR_RUNTIME_ERROR(std::string("Failed to Initialize a shader") +
		std::string("\nVertex Path : ") + vertex_path.string() +
		std::string("\nFrament Path : ") + fragment_path.string());
}

void FE2D::Shader::createProgram() noexcept {
	m_Program = glCreateProgram();
}

bool FE2D::Shader::loadFromSource(const char* source, const unsigned int& type) {
	// Vertex or Fragment Shader
	unsigned int shader = glCreateShader(type);

	// Load Source to Shader
	glShaderSource(shader, 1, &source, nullptr);
	// Compile the Shader
	glCompileShader(shader);
	// Bind the Shader to the Program
	glAttachShader(m_Program, shader);
	// Delete the Shader
	glDeleteShader(shader);

	// Check Shader Errors
	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);

		SAY("ERROR : Failed to Compile Shader\n" << message);

		glDeleteShader(shader);

		return false;
	}

	return true;
}

bool FE2D::Shader::loadFromFile(const std::wstring& path, const unsigned int& type) {
	std::ifstream file(path);
	if (!file.good()) {
		SAY(L"ERROR: Failed to open shader file\nPath: " << path.c_str());
		return false;
	}

	std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	file.close();

	if (source.empty()) {
		SAY(L"ERROR: Shader file is empty\nPath: " << path.c_str());
		return false;
	}

	return loadFromSource(source.c_str(), type);
}


void FE2D::Shader::LinkShader() {
	glLinkProgram(m_Program);
	glValidateProgram(m_Program);
}

inline void FE2D::Shader::Use()const noexcept { glUseProgram(m_Program); }
inline void FE2D::Shader::Unbind() const noexcept { glUseProgram(0); }

#pragma region Legacy Uniform Setting

// Methods with Uniform Setting
#pragma region Set Uniforms

void FE2D::Shader::setUniformInt(const char* uniform_name, int   load_uniform)const noexcept { glUniform1i(glGetUniformLocation(this->m_Program, uniform_name), load_uniform); }
void FE2D::Shader::setUniformIntArray(const char* uniform_name, int* load_uniform, unsigned int count)const noexcept { glUniform1iv(glGetUniformLocation(this->m_Program, uniform_name), count, load_uniform); }

void FE2D::Shader::setUniformFloat(const char* uniform_name, float load_uniform)const noexcept { glUniform1f(glGetUniformLocation(this->m_Program, uniform_name), load_uniform); }
void FE2D::Shader::setUniformFloatArray(const char* uniform_name, float* load_uniform, unsigned int count)const noexcept { glUniform1fv(glGetUniformLocation(this->m_Program, uniform_name), count, load_uniform); }

void FE2D::Shader::setUniformVec2(const char* uniform_name, vec2  load_uniform)const noexcept { glUniform2f(glGetUniformLocation(this->m_Program, uniform_name), load_uniform.x, load_uniform.y); }
void FE2D::Shader::setUniformVec2Array(const char* uniform_name, vec2* load_uniform, unsigned int count)const noexcept { glUniform2fv(glGetUniformLocation(this->m_Program, uniform_name), count, &load_uniform[0].x); }

void FE2D::Shader::setUniformVec3(const char* uniform_name, vec3  load_uniform)const noexcept { glUniform3f(glGetUniformLocation(this->m_Program, uniform_name), load_uniform.x, load_uniform.y, load_uniform.z); }
void FE2D::Shader::setUniformVec3Array(const char* uniform_name, vec3* load_uniform, unsigned int count)const noexcept { glUniform3fv(glGetUniformLocation(this->m_Program, uniform_name), count, &load_uniform[0].x); }

void FE2D::Shader::setUniformVec4(const char* uniform_name, vec4  load_uniform)const noexcept { glUniform4f(glGetUniformLocation(this->m_Program, uniform_name), load_uniform.x, load_uniform.y, load_uniform.z, load_uniform.w); }
void FE2D::Shader::setUniformVec4Array(const char* uniform_name, vec4* load_uniform, unsigned int count)const noexcept { glUniform4fv(glGetUniformLocation(this->m_Program, uniform_name), count, &load_uniform[0].x); }

void FE2D::Shader::setUniformMat3(const char* uniform_name, mat3  load_uniform)const noexcept { glUniformMatrix3fv(glGetUniformLocation(this->m_Program, uniform_name), 1, GL_FALSE, glm::value_ptr(load_uniform)); }
void FE2D::Shader::setUniformMat3Array(const char* uniform_name, mat3* load_uniform, unsigned int count)const noexcept { glUniformMatrix3fv(glGetUniformLocation(this->m_Program, uniform_name), count, GL_FALSE, glm::value_ptr(load_uniform[0])); }

void FE2D::Shader::setUniformMat4(const char* uniform_name, mat4  load_uniform)const noexcept { glUniformMatrix4fv(glGetUniformLocation(this->m_Program, uniform_name), 1, GL_FALSE, glm::value_ptr(load_uniform)); }
void FE2D::Shader::setUniformMat4Array(const char* uniform_name, mat4* load_uniform, unsigned int count)const noexcept { glUniformMatrix4fv(glGetUniformLocation(this->m_Program, uniform_name), count, GL_FALSE, glm::value_ptr(load_uniform[0])); }

#pragma endregion

// Static Methods with Uniform Setting
#pragma region Set Uniforms

void FE2D::Shader::setUniformInt(unsigned int shaderRef, const char* uniform_name, int   load_uniform)noexcept { glUniform1i(glGetUniformLocation(shaderRef, uniform_name), load_uniform); }
void FE2D::Shader::setUniformIntArray(unsigned int shaderRef, const char* uniform_name, int* load_uniform, unsigned int count)noexcept { glUniform1iv(glGetUniformLocation(shaderRef, uniform_name), count, load_uniform); }

void FE2D::Shader::setUniformFloat(unsigned int shaderRef, const char* uniform_name, float load_uniform)noexcept { glUniform1f(glGetUniformLocation(shaderRef, uniform_name), load_uniform); }
void FE2D::Shader::setUniformFloatArray(unsigned int shaderRef, const char* uniform_name, float* load_uniform, unsigned int count)noexcept { glUniform1fv(glGetUniformLocation(shaderRef, uniform_name), count, load_uniform); }

void FE2D::Shader::setUniformVec2(unsigned int shaderRef, const char* uniform_name, const vec2& load_uniform)noexcept { glUniform2f(glGetUniformLocation(shaderRef, uniform_name), load_uniform.x, load_uniform.y); }
void FE2D::Shader::setUniformVec2Array(unsigned int shaderRef, const char* uniform_name, vec2* load_uniform, unsigned int count)noexcept { glUniform2fv(glGetUniformLocation(shaderRef, uniform_name), count, &load_uniform[0].x); }

void FE2D::Shader::setUniformVec3(unsigned int shaderRef, const char* uniform_name, const vec3& load_uniform)noexcept { glUniform3f(glGetUniformLocation(shaderRef, uniform_name), load_uniform.x, load_uniform.y, load_uniform.z); }
void FE2D::Shader::setUniformVec3Array(unsigned int shaderRef, const char* uniform_name, vec3* load_uniform, unsigned int count)noexcept { glUniform3fv(glGetUniformLocation(shaderRef, uniform_name), count, &load_uniform[0].x); }

void FE2D::Shader::setUniformVec4(unsigned int shaderRef, const char* uniform_name, const vec4& load_uniform)noexcept { glUniform4f(glGetUniformLocation(shaderRef, uniform_name), load_uniform.x, load_uniform.y, load_uniform.z, load_uniform.w); }
void FE2D::Shader::setUniformVec4Array(unsigned int shaderRef, const char* uniform_name, vec4* load_uniform, unsigned int count)noexcept { glUniform4fv(glGetUniformLocation(shaderRef, uniform_name), count, &load_uniform[0].x); }

void FE2D::Shader::setUniformMat3(unsigned int shaderRef, const char* uniform_name, const mat3& load_uniform)noexcept { glUniformMatrix3fv(glGetUniformLocation(shaderRef, uniform_name), 1, GL_FALSE, glm::value_ptr(load_uniform)); }
void FE2D::Shader::setUniformMat3Array(unsigned int shaderRef, const char* uniform_name, mat3* load_uniform, unsigned int count)noexcept { glUniformMatrix3fv(glGetUniformLocation(shaderRef, uniform_name), count, GL_FALSE, glm::value_ptr(load_uniform[0])); }

void FE2D::Shader::setUniformMat4(unsigned int shaderRef, const char* uniform_name, const mat4& load_uniform)noexcept { glUniformMatrix4fv(glGetUniformLocation(shaderRef, uniform_name), 1, GL_FALSE, glm::value_ptr(load_uniform)); }
void FE2D::Shader::setUniformMat4Array(unsigned int shaderRef, const char* uniform_name, mat4* load_uniform, unsigned int count)noexcept { glUniformMatrix4fv(glGetUniformLocation(shaderRef, uniform_name), count, GL_FALSE, glm::value_ptr(load_uniform[0])); }

#pragma endregion

#pragma endregion