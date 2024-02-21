#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

#include "ShaderHelper.h"
#include "Vertex.h"

// Control points and path inside the -1 to 1 range
std::vector<Vertex2D> controlParabolicPoints = {
    {-1.f, -0.8f, 1, 1, 1}, {-0.5f, 0.4f, 1, 1, 1}, {0.f, 0.5f, 1, 1, 1}, {0.2f, 0.3f, 1, 1, 1}, {0.5f, -0.2f, 1, 1, 1}, {1.f, 0.2f, 1, 1, 1} // White color
};
std::vector<Vertex2D> controlCubicPoints = {
    {-1.f, -0.8f, 1, 1, 1}, {0.f, 0.5f, 1, 1, 1}, {0.5f, -0.2f, 1, 1, 1}, {1.f, 0.2f, 1, 1, 1} // White color
};

// Calculates the coefficients of the parabolic fit
glm::vec3 calculateParabolicCoefficients(std::vector<Vertex2D>& points) {
    glm::mat3 A(0.0f);
    glm::vec3 b(0.0f), x(0.0f);

    for (auto& point : points) { // Sum matrix and vector with normal equation
        float x_i = point.x;
        float y_i = point.y;

        A[0][0] += x_i * x_i * x_i * x_i;
        A[0][1] += x_i * x_i * x_i;
        A[0][2] += x_i * x_i;
        A[1][0] += x_i * x_i * x_i;
        A[1][1] += x_i * x_i;
        A[1][2] += x_i;
        A[2][0] += x_i * x_i;
        A[2][1] += x_i;
        A[2][2] += 1;

        b[0] += x_i * x_i * y_i;
        b[1] += x_i * y_i;
        b[2] += y_i;
    }

    x = glm::inverse(A) * b;
    return x;
}

std::vector<Vertex2D> generateParabolaPoints(const glm::vec3& coefficients, float minX, float maxX, float step) {
    std::vector<Vertex2D> parabolaPoints;
    for (float x = minX; x <= maxX; x += step) {
        float y = coefficients.x * x * x + coefficients.y * x + coefficients.z; // ax^2 + bx + c
        parabolaPoints.push_back(Vertex2D{ x, y, 1.0f, 0.0f, 0.0f }); // Red color
    }
    return parabolaPoints;
}

std::vector<Vertex2D> generateBezierCubicPoints(const std::vector<Vertex2D>& points, float step) {
    std::vector<Vertex2D> bezierPoints;
    for (float t = 0.0f; t <= 1.0f; t += step) { // Calculate cubic bezier function
        float u = 1 - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        glm::vec2 P = uuu * glm::vec2(points[0].x, points[0].y); // P0
        P += 3 * uu * t * glm::vec2(points[1].x, points[1].y); // P1
        P += 3 * u * tt * glm::vec2(points[2].x, points[2].y); // P2
        P += ttt * glm::vec2(points[3].x, points[3].y); // P3

        bezierPoints.push_back(Vertex2D{ P.x, P.y, 1.0f, 0.0f, 0.0f }); // Using red color for curve // Also where the points are stored (in array)
    }
    return bezierPoints;
}

int main(int argc, char** argv)
{
    bool performCubicInterpolation = false; // Default to parabolic fit
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-cubic") {
            performCubicInterpolation = true;
        }
    }

    //      GLFW CONFIGURE       //
    GLFWwindow* window;
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(480, 480, "Compulsory 2", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to get process address\n";
        glfwTerminate();
        return -1;
    }

    //      SHADER & MATH SETUP        //
    // Load shader source -- reads from current folder
    std::cout << "Remember to run directly via executable in project!" << std::endl; // Reminder

    std::string vertexShaderSourceString2D = std::string(ShaderHelper::readShader(TYPE_VERTEX, DIMENSION_2D));
    std::string fragmentShaderSourceString = std::string(ShaderHelper::readShader(TYPE_FRAGMENT));

    const char* vertexShaderSource2D = vertexShaderSourceString2D.c_str();
    const char* fragmentShaderSource = fragmentShaderSourceString.c_str();

    // Create both shader programs with their compiled shaders then prepare objects
    GLuint shaderProgram = ShaderHelper::createProgram(vertexShaderSource2D, fragmentShaderSource);

    GLuint VAO, VBO; // Control points
    GLuint curveVAO, curveVBO; // Either parabolic or cubic
    std::vector<Vertex2D> curvePoints;

    if (performCubicInterpolation) {
        ShaderHelper::setupObjects(VAO, VBO, controlCubicPoints, 2); // Setup VAO and VBO for control points
        curvePoints = generateBezierCubicPoints(controlCubicPoints, 0.01f); // Generate cubic Bezier curve points
        ShaderHelper::setupObjects(curveVAO, curveVBO, curvePoints, 2); // Setup VAO and VBO for curve points
    }
    else {
        ShaderHelper::setupObjects(VAO, VBO, controlParabolicPoints, 2);
        glm::vec3 parabolicCoefficients = calculateParabolicCoefficients(controlParabolicPoints);
        std::cout << "Parabolic Fit Coefficients: a=" << parabolicCoefficients.x << ", b=" << parabolicCoefficients.y << ", c=" << parabolicCoefficients.z << std::endl;
        curvePoints = generateParabolaPoints(parabolicCoefficients, -1.0f, 1.0f, 0.01f);
    }

    ShaderHelper::setupObjects(curveVAO, curveVBO, curvePoints, 2);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Render original control points larger and in white
        glBindVertexArray(VAO);
        glPointSize(10.0f);
        if (performCubicInterpolation)
            glDrawArrays(GL_POINTS, 0, controlCubicPoints.size());
        else
            glDrawArrays(GL_POINTS, 0, controlParabolicPoints.size());

        // Draw the fitted curve with lines in red
        glBindVertexArray(curveVAO);
        glPointSize(1.0f);
        glDrawArrays(GL_LINE_STRIP, 0, curvePoints.size());

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}