#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <vector>

enum ShaderEnums {
    DIMENSION_3D = true,
    DIMENSION_2D = false,
    TYPE_VERTEX = true,
    TYPE_FRAGMENT = false
};

class ShaderHelper {
public:
    static GLuint compileShader(const GLchar* source, GLenum type) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        return shader;
    }

    static GLuint createProgram(const GLchar* vertexSource, const GLchar* fragmentSource) {
        GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);

        if (!vertexShader || !fragmentShader) {
            return 0; // Failed to compile
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        // Clean up
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }
    template <typename T>
    static void setupObjects(GLuint& VAO, GLuint& VBO, std::vector<T>& vertices, int numPosVar) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, numPosVar, GL_FLOAT, GL_FALSE, sizeof(T), (void*)0); // Positions

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(numPosVar * sizeof(float))); // Colors

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // reads from current folder
    static std::string readShader(bool vertex, bool dimension = DIMENSION_2D) {
        std::ifstream shaderFile;
        if (vertex)
        {
            if (dimension == DIMENSION_2D)
                shaderFile.open("vertexShaderSource2D.vert");
            else
                shaderFile.open("vertexShaderSource3D.vert");
        }
        else
            shaderFile.open("fragmentShaderSource.frag");

        std::string str((std::istreambuf_iterator<char>(shaderFile)),
            std::istreambuf_iterator<char>());
        return str;
    }
};
