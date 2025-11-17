#include "ShaderManager.h"

GLuint ShaderManager::CompileShader(const char* path, GLenum shaderType)
{
    GLchar* shaderSource = filetobuf(path);
    if (!shaderSource) {
        std::cerr << "ERROR: " << path << " 파일을 읽지 못했습니다." << std::endl;
        return 0;
    }

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const GLchar**)&shaderSource, NULL);
    glCompileShader(shader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(shader, 512, NULL, errorLog);
        const char* shaderTypeStr = (shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        std::cerr << "ERROR: " << shaderTypeStr << " shader 컴파일 실패\n" << errorLog << std::endl;
        free(shaderSource);
        return 0;
    }
    
    free(shaderSource);
    return shader;
}

GLuint ShaderManager::LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderID;
}

GLuint ShaderManager::CreateShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    GLuint vertexShader = CompileShader(vertexPath, GL_VERTEX_SHADER);
    if (vertexShader == 0) return 0;
    
    GLuint fragmentShader = CompileShader(fragmentPath, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }
    
    GLuint program = LinkProgram(vertexShader, fragmentShader);
    if (program != 0) {
        glUseProgram(program);
    }
    
    return program;
}

GLuint ShaderManager::CreateSkyboxShaderProgram()
{
    GLuint vertexShader = CompileShader("skyboxVertex.glsl", GL_VERTEX_SHADER);
    if (vertexShader == 0) return 0;
    
    GLuint fragmentShader = CompileShader("skyboxFrag.glsl", GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }
    
    return LinkProgram(vertexShader, fragmentShader);
}

GLuint ShaderManager::LoadCubemap(const std::vector<std::string>& faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
            std::cout << "큐브맵 텍스처 로드 성공: " << faces[i] << std::endl;
        }
        else
        {
            std::cerr << "큐브맵 텍스처 로드 실패: " << faces[i] << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &textureID);
            return 0;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(true);
    return textureID;
}
