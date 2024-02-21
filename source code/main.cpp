#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 model2;
glm::mat4 model3;
glm::mat4 model4;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

glm::vec3 lightPos1;
glm::vec3 lightPos2;
glm::vec3 lightPos3;
glm::vec3 lightPos4;
glm::vec3 lightPos5;
glm::vec3 lightPos6;
glm::vec3 lightPos7;
glm::vec3 lightPos8;

glm::vec3 lightColor1;

glm::vec3 lightPos0;
glm::vec3 lightColor0;

glm::vec3 flashlightPos;
glm::vec3 flashlightDir;
float cutOff = glm::cos(glm::radians(12.5f));
float outerCutOff = glm::cos(glm::radians(17.5f));
bool on = false;

bool showDepthMap = false;

// shader uniform locations
GLint modelLoc;
GLint modelLoc2;
GLint modelLoc3;
GLint viewLoc;
GLint viewLoc2;
GLint projectionLoc;
GLint projectionLoc2;
GLint normalMatrixLoc;
GLint normalMatrixLoc2;
GLint lightDirLoc;
GLint lightDirLoc2;
GLint lightColorLoc;
GLint lightColorLoc2;

GLint lightPos1Loc;
GLint lightPos2Loc;
GLint lightPos3Loc;
GLint lightPos4Loc;
GLint lightPos5Loc;
GLint lightPos6Loc;
GLint lightPos7Loc;
GLint lightPos8Loc;
GLint lightColor1Loc;

GLint lightPos0Loc;
GLint lightColor0Loc;

GLint flashlightPosLoc;
GLint flashlightDirLoc;
GLint cutOffLoc;
GLint outerCutOffLoc;
GLint onLoc;

GLuint shadowMapFBO;
GLuint depthMapTexture;

// camera
gps::Camera myCamera(
    glm::vec3(-1.71f, 2.09f, 17.38f),
    glm::vec3(-1.71f, 2.09f, 7.38f),
    glm::vec3(0.0f, 1.0f, 0.0f));

float z_initial = 2.09f;

GLfloat cameraSpeed = 0.05f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D sceneInterior;
gps::Model3D door;
gps::Model3D doorKnob;
gps::Model3D doorKnobLock;
gps::Model3D sceneExterior;
gps::Model3D key;
gps::Model3D cat;
gps::Model3D screenQuad;
GLfloat angle;

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader myBasicShaderWithoutFog;
gps::Shader depthMapShader;
gps::Shader screenQuadShader;

// skybox
gps::SkyBox mySkyBox;

GLenum glCheckError_(int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) 
    {
		string error;
		switch (errorCode) 
        {
            case GL_INVALID_ENUM:
            {
                error = "INVALID_ENUM";
                break;
            }
            case GL_INVALID_VALUE:
            {
                error = "INVALID_VALUE";
                break;
            }
            case GL_INVALID_OPERATION:
            {
                error = "INVALID_OPERATION";
                break;
            }
            case GL_OUT_OF_MEMORY:
            {
                error = "OUT_OF_MEMORY";
                break;
            }
            case GL_INVALID_FRAMEBUFFER_OPERATION:
            {
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
            }
        }
		cout << error << " (" << line << ")" << '\n';
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
    cout << "Window resized! New width: " << width << ", and height: " << height << '\n';

    glViewport(0, 0, width, height);

    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 200.0f);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

double computeBinominal(int n, int k)
{
    if (k == 0 || k >= n)
    {
        return 1;
    }

    double value = 1.0;

    int i;
    for (i = 1; i <= k; ++i)
    {
        value *= (double)(((double)n + 1.0 - (double)i) / (double)i);
    }

    return value;
}

double lastTimeStamp0 = glfwGetTime();

bool animateOnce2 = false;
bool firstCurve = true;
bool secondCurve = false;
bool thirdCurve = false;
bool fourthCurve = false;

double xX1[] = { 6.99, -16.20, 1.88, -28.55, -4 };
double yY1[] = { 3.11, 2.84, 2.43, 6.85, 2.64 };
double zZ1[] = { 22.18, 15.49, 3.64, -17.92, -13.51 };
double yaYA1[] = { -130.5, -61.04, -140.1, 18.75, 375.89 };
double pP1[] = { -9.6, -2, -2, -13.99, -0.44 };

double xX2[] = { -4, 6.89 };
double yY2[] = { 2.64, 1.91 };
double zZ2[] = { -13.51, -8.24 };
double yaYA2[] = { 375.89, 268.04 };
double pP2[] = { -0.44, -34.94 };

double xX3[] = { 6.89, 5.9 };
double yY3[] = { 1.91, -7.16 };
double zZ3[] = { -8.24, -32.22 };
double yaYA3[] = { 268.04, 262.49 };
double pP3[] = { -34.94, -28.25 };

double xX4[] = { 5.9, -2.62 };
double yY4[] = { -7.16, -7.52 };
double zZ4[] = { -32.22, -34.54 };
double yaYA4[] = { 262.49, -289.65 };
double pP4[] = { -28.25, -2.59 };

double t;
double bezierSpeed1 = 0.03f;
double bezierSpeed2 = 0.08f;
double bezierSpeed3 = 0.06f;
double bezierSpeed4 = 0.05f;

bool ignoreStairs = false;

void cameraAnimation()
{
    if (animateOnce2 == false)
    {
        return;
    }

    double currentTimeStamp = glfwGetTime();
    double elapsedSeconds = currentTimeStamp - lastTimeStamp0;
    if (firstCurve)
    {
        t += elapsedSeconds * bezierSpeed1;
    }
    else if (secondCurve)
    { 
        t += elapsedSeconds * bezierSpeed2;
    }
    else if (thirdCurve)
    {
        t += elapsedSeconds * bezierSpeed3;
    }
    else if (fourthCurve)
    {
        t += elapsedSeconds * bezierSpeed4;
    }

    lastTimeStamp0 = currentTimeStamp;

    double bCurveXt = 0;
    double bCurveYt = 0;
    double bCurveZt = 0;
    double bCurvePt = 0;
    double bCurveYat = 0;

    int n;
    if (firstCurve)
    {
        n = 5;
    }
    else
    {
        n = 2;
    }
    int i;
    for (i = 0; i < n; ++i)
    {
        if (firstCurve)
        {
            bCurveXt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * xX1[i];
            bCurveYt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * yY1[i];
            bCurveZt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * zZ1[i];
            bCurvePt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * pP1[i];
            bCurveYat += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * yaYA1[i];
        }
        else if (secondCurve)
        {
            bCurveXt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * xX2[i];
            bCurveYt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * yY2[i];
            bCurveZt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * zZ2[i];
            bCurvePt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * pP2[i];
            bCurveYat += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * yaYA2[i];
        }
        else if (thirdCurve)
        {
            bCurveXt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * xX3[i];
            bCurveYt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * yY3[i];
            bCurveZt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * zZ3[i];
            bCurvePt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * pP3[i];
            bCurveYat += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * yaYA3[i];
        }
        else if (fourthCurve)
        {
            bCurveXt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * xX4[i];
            bCurveYt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * yY4[i];
            bCurveZt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * zZ4[i];
            bCurvePt += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * pP4[i];
            bCurveYat += computeBinominal(n - 1, i) * pow((1 - t), (n - 1 - i)) * pow(t, i) * yaYA4[i];
        }
    }
    if (firstCurve && t >= 1)
    {
        firstCurve = false;
        secondCurve = true;

        t = 0;
    }
    else if (secondCurve && t >= 1)
    {
        secondCurve = false;
        thirdCurve = true;

        t = 0;
    }
    else if (thirdCurve && t >= 1)
    {
        thirdCurve = false;
        fourthCurve = true;

        t = 0;
    }
    else if (fourthCurve && t >= 1)
    {
        fourthCurve = false;
        animateOnce2 = false;
        ignoreStairs = false;
        return;
    }

    myCamera.set_x(bCurveXt);
    myCamera.set_y(bCurveYt);
    myCamera.set_z(bCurveZt);

    myCamera.rotate(bCurvePt, bCurveYat);

    // update view matrix
    view = myCamera.getViewMatrix();

    myBasicShaderWithoutFog.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

bool animateOnce = false;
bool wireframeView = false;
bool wireframeView2 = false;
bool foundKey = false;

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        on = !on;
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        showDepthMap = !showDepthMap;
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        float current_x = myCamera.getCameraPosition().x;
        float current_z = myCamera.getCameraPosition().z;
        if (foundKey == false)
        {
            if (current_x >= 10.39 && current_x <= 12.97 && current_z >= -1.86 && current_z <= -0.25)
            {
                foundKey = true;
            }
        }
        else if (foundKey == true && animateOnce == false)
        {
            if (current_x >= -13.63 && current_x <= -10.46 && current_z >= -12.36 && current_z <= -8.5)
            {
                animateOnce = true;
            }
        }
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        if (wireframeView == false && wireframeView2 == false)
        {
            wireframeView = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if (wireframeView == true && wireframeView2 == false)
        {
            wireframeView2 = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        }
        else if (wireframeView == true && wireframeView2 == true)
        {
            wireframeView = false;
            wireframeView2 = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        glm::vec3 cameraPosition = myCamera.getCameraPosition();
        if (cameraPosition.x == -1.71f && cameraPosition.y == 2.09f && cameraPosition.z == 17.38f && angle == 0 && animateOnce2 == false && firstCurve == true)
        {
            animateOnce2 = true;
            ignoreStairs = true;
        }
        else if (firstCurve == false && fourthCurve == false)
        {
            myCamera.set_x(-1.71f);
            myCamera.set_y(2.09f);
            myCamera.set_z(17.38f);

            myCamera.rotate(0, -90);

            // update view matrix
            view = myCamera.getViewMatrix();

            myBasicShaderWithoutFog.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for scene
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

            myBasicShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for scene
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        }
    }

    if (key == GLFW_KEY_LEFT_SHIFT)
    {
        if (action == GLFW_PRESS)
        {
            cameraSpeed = 0.1f;
        }
        else if (action == GLFW_RELEASE)
        {
            cameraSpeed = 0.05f;
        }
    }

	if (key >= 0 && key < 1024) 
    {
        if (action == GLFW_PRESS) 
        {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) 
        {
            pressedKeys[key] = false;
        }
    }
}

float yaw = -90.0;
float pitch = 0.0;
double lastX = 0.0;
double lastY = 0.0;
bool firstMouse = true;
const float sensitivity = 0.15f;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();

    myBasicShaderWithoutFog.useShaderProgram();
    glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    cout << myCamera.getCameraPosition().x << " " << myCamera.getCameraPosition().y << " " << myCamera.getCameraPosition().z << " || " << yaw << " " << pitch << '\n';
}

float houseLimit = -10.45;
bool doorAnimationDone = false;

bool collisionDetection(gps::MOVE_DIRECTION move_dir)
{
    //return false;
    
    if (animateOnce == true && doorAnimationDone == false)
    {
        return true;
    }

    float myY = myCamera.getCameraPosition().y;
    float new_x = 0, new_z = 0;
    switch (move_dir)
    {
        case gps::MOVE_FORWARD:
        {
            new_x = myCamera.getCameraPosition().x + myCamera.getCameraFrontDirection().x * cameraSpeed;
            new_z = myCamera.getCameraPosition().z + myCamera.getCameraFrontDirection().z * cameraSpeed;
            break;
        }
        case gps::MOVE_BACKWARD:
        {
            new_x = myCamera.getCameraPosition().x - myCamera.getCameraFrontDirection().x * cameraSpeed;
            new_z = myCamera.getCameraPosition().z - myCamera.getCameraFrontDirection().z * cameraSpeed;
            break;
        }
        case gps::MOVE_LEFT:
        {
            new_x = myCamera.getCameraPosition().x - myCamera.getCameraRightDirection().x * cameraSpeed;
            new_z = myCamera.getCameraPosition().z - myCamera.getCameraRightDirection().z * cameraSpeed;
            break;
        }
        case gps::MOVE_RIGHT:
        {
            new_x = myCamera.getCameraPosition().x + myCamera.getCameraRightDirection().x * cameraSpeed;
            new_z = myCamera.getCameraPosition().z + myCamera.getCameraRightDirection().z * cameraSpeed;
            break;
        }
        default: break;
    }

    if (myY == z_initial)
    {
        if (new_z >= 0.577 * new_x + 19.714)
        {
            return true;
        }
        if (new_x <= (new_z - 279.2072) / 18.0484)
        {
            return true;
        }
        if (new_z <= -0.855 * new_x - 24.92)
        {
            return true;
        }
        if (new_z <= -0.0635 * new_x - 16.468)
        {
            return true;
        }
        if (new_z <= 3.75 * new_x -58.185)
        {
            return true;
        }
        if (new_x >= (new_z - 72.7315) / -5.6063)
        {
            return true;
        }
        if (new_z >= -0.3459 * new_x + 17.38805)
        {
            return true;
        }
        if (new_x >= 0.92 && new_x <= 11.73 && new_z >= 2.51 && new_z <= 12.96)
        {
            return true;
        }
        if (new_x >= houseLimit && new_x <= 11.72 && new_z >= -15.87 && new_z <= -4.78)
        {
            return true;
        }
    }
    else if (myY == z_initial + 2 * 0.33f)
    {
        if (new_x < -10.18 || new_x > 9.03 || new_z < -14.87 || new_z > -7.6)
        {
            return true;
        }
        if (new_x >= 4 && new_x <= 8.63 && new_z >= -16.13 && new_z <= -10.41)
        {
            return true;
        }
        if (new_x >= -8.89 && new_x <= -7.19 && new_z >= -13.26 && new_z <= -11.61)
        {
            return true;
        }
    }
    else if (myY <= z_initial - 0.01f && myY >= -1.86f)
    {
        if (new_x < 4.8 || new_x > 7.56)
        {
            return true;
        }
    }
    else if (myY <= -1.87 && myY >= -6.48)
    {
        if (new_x < 5.72 || new_x > 7.56)
        {
            return true;
        }
    }
    else if (myY < -6.48)
    {
        if (new_x > 6.71 || new_x < -2.48 || new_z > -18.25 || new_z < -34.99)
        {
            return true;
        }
        if (new_x >= -2.48 && new_x <= -0.27 && new_z >= -30.78 && new_z <= -25.67)
        {
            return true;
        }
        if (new_x >= -2.48 && new_x <= -2.07 && new_z >= -24.58 && new_z <= -23.18)
        {
            return true;
        }
        if (new_x >= -2.48 && new_x <= 0.58 && new_z >= -20.17 && new_z <= -17.15)
        {
            return true;
        }
        if (new_x >= 0.58 && new_x <= 3.6 && new_z >= -18.25 && new_z <= -17.15)
        {
            return true;
        }
        if (new_x >= 3.6 && new_x <= 4.34 && new_z >= -25.93 && new_z <= -17.15)
        {
            return true;
        }
    }

    return false;
}

float translateX, translateY, translateZ;
float scaleSize = 1.0f;

void processMovement() 
{
	if (pressedKeys[GLFW_KEY_W] && collisionDetection(gps::MOVE_FORWARD) == false) 
    {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		// update view matrix
        view = myCamera.getViewMatrix();

        myBasicShaderWithoutFog.useShaderProgram();
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        cout << myCamera.getCameraPosition().x << " " << myCamera.getCameraPosition().y << " " << myCamera.getCameraPosition().z << " || " << yaw << " " << pitch << '\n';
    }

	if (pressedKeys[GLFW_KEY_S] && collisionDetection(gps::MOVE_BACKWARD) == false)
    {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        // update view matrix
        view = myCamera.getViewMatrix();

        myBasicShaderWithoutFog.useShaderProgram();
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        cout << myCamera.getCameraPosition().x << " " << myCamera.getCameraPosition().y << " " << myCamera.getCameraPosition().z << " || " << yaw << " " << pitch << '\n';
    }

	if (pressedKeys[GLFW_KEY_D] && collisionDetection(gps::MOVE_LEFT) == false)
    {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        // update view matrix
        view = myCamera.getViewMatrix();

        myBasicShaderWithoutFog.useShaderProgram();
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        cout << myCamera.getCameraPosition().x << " " << myCamera.getCameraPosition().y << " " << myCamera.getCameraPosition().z << " || " << yaw << " " << pitch << '\n';
    }

	if (pressedKeys[GLFW_KEY_A] && collisionDetection(gps::MOVE_RIGHT) == false)
    {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        // update view matrix
        view = myCamera.getViewMatrix();

        myBasicShaderWithoutFog.useShaderProgram();
        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

        cout << myCamera.getCameraPosition().x << " " << myCamera.getCameraPosition().y << " " << myCamera.getCameraPosition().z << " || " << yaw << " " << pitch << '\n';
    }

    
    if (pressedKeys[GLFW_KEY_Q]) 
    {
        angle -= 1.0f;
        // update model matrix for scene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        model2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        model3 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        model4 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) 
    {
        angle += 1.0f;
        // update model matrix for scene
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        model2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        model3 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        model4 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_I])
    {
        translateZ += 0.5f;
        // update model matrix for scene
        model = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model2 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model3 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model4 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_K])
    {
        translateZ -= 0.5f;
        // update model matrix for scene
        model = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model2 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model3 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model4 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_J])
    {
        translateX += 0.5f;
        // update model matrix for scene
        model = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model2 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model3 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model4 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_L])
    {
        translateX -= 0.5f;
        // update model matrix for scene
        model = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model2 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model3 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model4 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_G])
    {
        translateY += 0.5f;
        // update model matrix for scene
        model = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model2 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model3 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model4 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_H])
    {
        translateY -= 0.5f;
        // update model matrix for scene
        model = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model2 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model3 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        model4 = glm::translate(glm::mat4(1.0f), glm::vec3(translateX, translateY, translateZ));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Z])
    {
        scaleSize += 0.01f;
        // update model matrix for scene
        model = glm::scale(glm::mat4(1.0f), glm::vec3(scaleSize, scaleSize, scaleSize));
        model2 = glm::scale(glm::mat4(1.0f), glm::vec3(scaleSize, scaleSize, scaleSize));
        model3 = glm::scale(glm::mat4(1.0f), glm::vec3(scaleSize, scaleSize, scaleSize));
        model4 = glm::scale(glm::mat4(1.0f), glm::vec3(scaleSize, scaleSize, scaleSize));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_X])
    {
        scaleSize -= 0.01f;
        // update model matrix for scene
        model = glm::scale(glm::mat4(1.0f), glm::vec3(scaleSize, scaleSize, scaleSize));
        model2 = glm::scale(glm::mat4(1.0f), glm::vec3(scaleSize, scaleSize, scaleSize));
        model3 = glm::scale(glm::mat4(1.0f), glm::vec3(scaleSize, scaleSize, scaleSize));
        model4 = glm::scale(glm::mat4(1.0f), glm::vec3(scaleSize, scaleSize, scaleSize));
        // update normal matrix for scene
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
}

void initOpenGLWindow() 
{
    myWindow.Create(1920, 1080, "OpenGL Project");
}

void setWindowCallbacks() 
{
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(myWindow.getWindow());

    glfwSwapInterval(1);
}

void initOpenGLState() 
{
	glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() 
{
    //teapot.LoadModel("models/teapot/teapot20segUT.obj");
    sceneInterior.LoadModel("models/sceneInterior.obj");
    door.LoadModel("models/door.obj");
    doorKnob.LoadModel("models/doorKnob.obj");
    doorKnobLock.LoadModel("models/doorKnobLock.obj");
    sceneExterior.LoadModel("models/sceneExterior.obj");
    key.LoadModel("models/key.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
}

void initShaders()
{
	myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    myBasicShaderWithoutFog.loadShader("shaders/basicWithoutFog.vert", "shaders/basicWithoutFog.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    depthMapShader.loadShader("shaders/depth.vert", "shaders/depth.frag");
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
}

void initSkybox()
{
    vector<const GLchar*> faces;
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/front.tga");
    faces.push_back("skybox/back.tga");
    mySkyBox.Load(faces);
}

const unsigned int SHADOW_WIDTH = 8000;
const unsigned int SHADOW_HEIGHT = 8000;

void initFBO()
{
    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
    glm::vec3 lightPosition = glm::vec3(27.94f, 16.89f, 18.38f);
    glm::vec3 lightTarget = glm::vec3(-2.34f, 0.46f, 3.63f);
    glm::vec3 lightFrontDirection = glm::normalize(lightTarget - lightPosition);
    glm::mat4 lightView = glm::lookAt(lightPosition, lightPosition + lightFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 70.0f;
    glm::mat4 lightProjection = glm::ortho(-30.0f, 40.0f, -20.0f, 20.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

    return lightSpaceTrMatrix;
}

void initUniforms() 
{
	myBasicShader.useShaderProgram();

    // create model matrix for the scene
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for the scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 200.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	// set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	// set light color
	lightColor = glm::vec3(0.3f, 0.3f, 1.0f); //blue-ish light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // set the point light position
    lightPos1 = glm::vec3(-7.25f, 4.09f, 14.72f);
    lightPos1Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos1");
    // send light pos to shader
    glUniform3fv(lightPos1Loc, 1, glm::value_ptr(lightPos1));

    // set the point light position
    lightPos2 = glm::vec3(-7.38f, 4.09f, 14.51f);
    lightPos2Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos2");
    // send light pos to shader
    glUniform3fv(lightPos2Loc, 1, glm::value_ptr(lightPos2));

    // set the point light position
    lightPos3 = glm::vec3(-7.54f, 4.09f, 14.37f);
    lightPos3Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos3");
    // send light pos to shader
    glUniform3fv(lightPos3Loc, 1, glm::value_ptr(lightPos3));

    // set the point light position
    lightPos4 = glm::vec3(-7.77f, 4.09f, 14.45f);
    lightPos4Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos4");
    // send light pos to shader
    glUniform3fv(lightPos4Loc, 1, glm::value_ptr(lightPos4));

    // set the point light position
    lightPos5 = glm::vec3(-7.89f, 4.09f, 14.64f);
    lightPos5Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos5");
    // send light pos to shader
    glUniform3fv(lightPos5Loc, 1, glm::value_ptr(lightPos5));

    // set the point light position
    lightPos6 = glm::vec3(-7.96f, 4.09f, 14.98f);
    lightPos6Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos6");
    // send light pos to shader
    glUniform3fv(lightPos6Loc, 1, glm::value_ptr(lightPos6));

    // set the point light position
    lightPos7 = glm::vec3(-7.68f, 4.09f, 15.14f);
    lightPos7Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos7");
    // send light pos to shader
    glUniform3fv(lightPos7Loc, 1, glm::value_ptr(lightPos7));

    // set the point light position
    lightPos8 = glm::vec3(-7.33f, 4.09f, 15.0f);
    lightPos8Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos8");
    // send light pos to shader
    glUniform3fv(lightPos8Loc, 1, glm::value_ptr(lightPos8));

    // set the point light color
    lightColor1 = glm::vec3(0.9f, 0.35f, 0.0f);
    lightColor1Loc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosColor");
    // send pos light color to shader
    glUniform3fv(lightColor1Loc, 1, glm::value_ptr(lightColor1));
}

void initUniforms2()
{
    myBasicShaderWithoutFog.useShaderProgram();

    // create model matrix for the scene
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model3 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model4 = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc2 = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc2 = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for the scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc2 = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 200.0f);
    projectionLoc2 = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc2, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc2 = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc2, 1, glm::value_ptr(lightDir));

    //set light color
    //lightColor = glm::vec3(0.3f, 0.3f, 1.0f); //blue-ish light
    lightColor = glm::vec3(0.05f, 0.05f, 0.3f); //blue-ish light
    lightColorLoc2 = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor));

    // set the point light position
    lightPos0 = glm::vec3(0.39f, -6.5f, -24.77f);
    lightPos0Loc = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "lightPos");
    // send light pos to shader
    glUniform3fv(lightPos0Loc, 1, glm::value_ptr(lightPos0));

    // set the point light color
    lightColor0 = glm::vec3(0.0f, 0.8f, 0.5f); //green-ish light
    lightColor0Loc = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "lightPosColor");
    // send pos light color to shader
    glUniform3fv(lightColor0Loc, 1, glm::value_ptr(lightColor0));
}

void initUniforms3()
{
    depthMapShader.useShaderProgram();

    modelLoc3 = glGetUniformLocation(depthMapShader.shaderProgram, "model");
}

void flashlightUniforms()
{
    myBasicShader.useShaderProgram();

    // set the flashlight position
    flashlightPos = myCamera.getCameraPosition();
    flashlightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "flashlightPos");
    // send flashlight pos to shader
    glUniform3fv(flashlightPosLoc, 1, glm::value_ptr(flashlightPos));

    // set the flashlight direction
    flashlightDir = myCamera.getCameraFrontDirection();
    flashlightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "flashlightDir");
    // send flashlight dir to shader
    glUniform3fv(flashlightDirLoc, 1, glm::value_ptr(flashlightDir));

    // set the flashlight (inner) cut off
    cutOffLoc = glGetUniformLocation(myBasicShader.shaderProgram, "cutOff");
    // send the flashlight (inner) cut off to shader
    glUniform1f(cutOffLoc, cutOff);

    // set the flashlight outer cut off
    outerCutOffLoc = glGetUniformLocation(myBasicShader.shaderProgram, "outerCutOff");
    // send the flashlight outer cut off to shader
    glUniform1f(outerCutOffLoc, outerCutOff);

    // set the on component of the flashlight
    onLoc = glGetUniformLocation(myBasicShader.shaderProgram, "on");
    // send the on component of the flashlight to shader
    glUniform1i(onLoc, on ? 1 : 0);



    myBasicShaderWithoutFog.useShaderProgram();

    // set the flashlight position
    flashlightPos = myCamera.getCameraPosition();
    flashlightPosLoc = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "flashlightPos");
    // send flashlight pos to shader
    glUniform3fv(flashlightPosLoc, 1, glm::value_ptr(flashlightPos));

    // set the flashlight direction
    flashlightDir = myCamera.getCameraFrontDirection();
    flashlightDirLoc = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "flashlightDir");
    // send flashlight dir to shader
    glUniform3fv(flashlightDirLoc, 1, glm::value_ptr(flashlightDir));

    // set the flashlight (inner) cut off
    cutOffLoc = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "cutOff");
    // send the flashlight (inner) cut off to shader
    glUniform1f(cutOffLoc, cutOff);

    // set the flashlight outer cut off
    outerCutOffLoc = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "outerCutOff");
    // send the flashlight outer cut off to shader
    glUniform1f(outerCutOffLoc, outerCutOff);

    // set the on component of the flashlight
    onLoc = glGetUniformLocation(myBasicShaderWithoutFog.shaderProgram, "on");
    // send the on component of the flashlight to shader
    glUniform1i(onLoc, on ? 1 : 0);
}

float x_step = 0.3f;
float y_step = 0.5f;
float z_step = 0.33f;

void stairs()
{
    if (ignoreStairs)
    {
        return;
    }

    float X = myCamera.getCameraPosition().x;
    float Y = myCamera.getCameraPosition().z;
    float Z = myCamera.getCameraPosition().y;

    // stairs at the front of the house
    if (Y > -11.63f && Y < -9.38f)
    {
        if (X > -10.01f && X < -10.01f + x_step)
        {
            myCamera.setZ(z_initial + z_step);
        }
        else if (X > -10.01f + x_step && X < -10.01f + 2 * x_step)
        {
            myCamera.setZ(z_initial + 2 * z_step);
        }
    }

    // first flight of stairs to the basement
    if (X > 4.92f && X < 7.97f)
    {
        if (Y < -10 && Y > -10 - y_step)
        {
            myCamera.setZ(z_initial + z_step);
        }
        else if (Y < -10 - y_step && Y > -10 - 2 * y_step)
        {
            myCamera.setZ(z_initial - 0.01);
        }
        else if (Y < -10 - 2 * y_step && Y > -10 - 3 * y_step)
        {
            myCamera.setZ(z_initial - z_step);
        }
        else if (Y < -10 - 3 * y_step && Y > -10 - 4 * y_step)
        {
            myCamera.setZ(z_initial - 2 * z_step);
        }
        else if (Y < -10 - 4 * y_step && Y > -10 - 5 * y_step)
        {
            myCamera.setZ(z_initial - 3 * z_step);
        }
        else if (Y < -10 - 5 * y_step && Y > -10 - 6 * y_step)
        {
            myCamera.setZ(z_initial - 4 * z_step);
        }
        else if (Y < -10 - 6 * y_step && Y > -10 - 7 * y_step)
        {
            myCamera.setZ(z_initial - 5 * z_step);
        }
        else if (Y < -10 - 7 * y_step && Y > -10 - 8 * y_step)
        {
            myCamera.setZ(z_initial - 6 * z_step);
        }
        else if (Y < -10 - 8 * y_step && Y > -10 - 9 * y_step)
        {
            myCamera.setZ(z_initial - 7 * z_step);
        }
        else if (Y < -10 - 9 * y_step && Y > -10 - 10 * y_step)
        {
            myCamera.setZ(z_initial - 8 * z_step);
        }
        else if (Y < -10 - 10 * y_step && Y > -10 - 11 * y_step)
        {
            myCamera.setZ(z_initial - 9 * z_step);
        }
        else if (Y < -10 - 11 * y_step && Y > -10 - 12 * y_step)
        {
            myCamera.setZ(z_initial - 10 * z_step);
        }
        else if (Y < -10 - 12 * y_step && Y > -10 - 13 * y_step)
        {
            myCamera.setZ(z_initial - 11 * z_step);
        }
        else if (Y < -10 - 13 * y_step && Y > -10 - 14 * y_step)
        {
            myCamera.setZ(z_initial - 12 * z_step);
        }
    }

    // second flight of stairs to the basement
    if (X > 5.05f && X < 7.59f)
    {
        if (Y < -20.53 && Y > -20.53 - y_step)
        {
            myCamera.setZ(z_initial - 13 * z_step);
        }
        else if (Y < -20.53 - y_step && Y > -20.53 - 2 * y_step)
        {
            myCamera.setZ(z_initial - 14 * z_step);
        }
        else if (Y < -20.53 - 2 * y_step && Y > -20.53 - 3 * y_step)
        {
            myCamera.setZ(z_initial - 15 * z_step);
        }
        else if (Y < -20.53 - 3 * y_step && Y > -20.53 - 4 * y_step)
        {
            myCamera.setZ(z_initial - 16 * z_step);
        }
        else if (Y < -20.53 - 4 * y_step && Y > -20.53 - 5 * y_step)
        {
            myCamera.setZ(z_initial - 17 * z_step);
        }
        else if (Y < -20.53 - 5 * y_step && Y > -20.53 - 6 * y_step)
        {
            myCamera.setZ(z_initial - 18 * z_step);
        }
        else if (Y < -20.53 - 6 * y_step && Y > -20.53 - 7 * y_step)
        {
            myCamera.setZ(z_initial - 19 * z_step);
        }
        else if (Y < -20.53 - 7 * y_step && Y > -20.53 - 8 * y_step)
        {
            myCamera.setZ(z_initial - 20 * z_step);
        }
        else if (Y < -20.53 - 8 * y_step && Y > -20.53 - 9 * y_step)
        {
            myCamera.setZ(z_initial - 21 * z_step);
        }
        else if (Y < -20.53 - 9 * y_step && Y > -20.53 - 10 * y_step)
        {
            myCamera.setZ(z_initial - 22 * z_step);
        }
        else if (Y < -20.53 - 10 * y_step && Y > -20.53 - 11 * y_step)
        {
            myCamera.setZ(z_initial - 23 * z_step);
        }
        else if (Y < -20.53 - 11 * y_step && Y > -20.53 - 12 * y_step)
        {
            myCamera.setZ(z_initial - 24 * z_step);
        }
        else if (Y < -20.53 - 12 * y_step && Y > -20.53 - 13 * y_step)
        {
            myCamera.setZ(z_initial - 25 * z_step);
        }
        else if (Y < -20.53 - 13 * y_step && Y > -20.53 - 14 * y_step)
        {
            myCamera.setZ(z_initial - 26 * z_step);
        }
    }

    view = myCamera.getViewMatrix();

    myBasicShaderWithoutFog.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // compute normal matrix for scene
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void renderTeapot(gps::Shader shader, bool depthPass) 
{
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    if (!depthPass)
    {
        // send exterior scene normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw teapot
    teapot.Draw(shader);
}

void renderExteriorScene(gps::Shader shader, bool depthPass)
{
    // select active shader program
    shader.useShaderProgram();

    // send exterior scene model matrix data to shader
    if (depthPass)
    {
        glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model));
    }
    else
    {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    }

    if (!depthPass)
    {
        // send exterior scene normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw exterior scene
    sceneExterior.Draw(shader);
    if (foundKey == false)
    {
        key.Draw(shader);
    }
}

float angle1;
float angle2;
float movement;

float total_angle1;
float total_angle2;
float total_movement;

float movementSpeed1 = 0.02f; // units per second
float movementSpeed2 = 0.05f; // units per second
float movementSpeed3 = 0.00003f; // units per second

double lastTimeStamp = glfwGetTime();

float sign = -1.0f;
float sign2 = -1.0f;

void renderInteriorScene(gps::Shader shader, bool depthPass)
{
    if ((total_angle1 <= 120.0f || total_angle2 <= 120.0f || total_movement <= 0.08f) && animateOnce == true)
    {
        double currentTimeStamp = glfwGetTime();
        double elapsedSeconds = currentTimeStamp - lastTimeStamp;

        angle1 += movementSpeed1 * elapsedSeconds;
        angle2 += movementSpeed2 * elapsedSeconds;
        movement += movementSpeed3 * elapsedSeconds;

        lastTimeStamp = currentTimeStamp;
    }

    if (total_angle1 > 120.0f && total_angle2 > 120.0f && total_movement > 0.08f && animateOnce == true)
    {
        houseLimit = -8.13f;
        doorAnimationDone = true;
    }

    // select active shader program
    shader.useShaderProgram();

    // send interior scene model matrix data to shader
    if (depthPass)
    { 
        glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model));
    }
    else
    {
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
    }

    if (!depthPass)
    {
        // send interior scene normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc2, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw interior scene
    sceneInterior.Draw(shader);

    // send door model matrix data to shader
    if (total_angle1 <= 120.0f && animateOnce == true)
    {
        model2 = glm::translate(model2, glm::vec3(-9.2f, 2.88f, -11.67f));
        model2 = glm::rotate(model2, glm::radians(angle1), glm::vec3(0.0f, 1.0f, 0.0f));
        model2 = glm::translate(model2, glm::vec3(9.2f, -2.88f, 11.67f));
    }

    if (depthPass)
    {
        glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model2));
    }
    else
    {
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
    }

    if (!depthPass)
    {
        // send door normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc2, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * model2))));
    }

    // draw door
    door.Draw(shader);

    // send door knob model matrix data to shader
    if (animateOnce == true)
    {
        if (total_angle1 <= 120.0f)
        {
            model3 = glm::translate(model3, glm::vec3(-9.2f, 2.88f, -11.67f));
            model3 = glm::rotate(model3, glm::radians(angle1), glm::vec3(0.0f, 1.0f, 0.0f));
            model3 = glm::translate(model3, glm::vec3(9.2f, -2.88f, 11.67f));
        }

        if (total_angle2 <= 120.0f)
        {
            float new_x = -9.2 + 2.09 * sin(glm::radians(total_angle1));
            float new_z = -11.67 + 2.09 * cos(glm::radians(total_angle1));
            model3 = glm::translate(model3, glm::vec3(new_x, 2.88f, new_z));
            //model3 = glm::translate(model3, glm::vec3(-9.0f, 2.88f, -9.58f));
            model3 = glm::rotate(model3, glm::radians(sign * angle2), glm::vec3(1.0f, 0.0f, 0.0f));
            total_angle2 += angle2;
            //model3 = glm::translate(model3, glm::vec3(9.0f, -2.88f, 9.58f));
            model3 = glm::translate(model3, glm::vec3(-new_x, -2.88f, -new_z));

            if (total_angle2 >= 60.0f && sign == -1.0f)
            {
                sign = 1.0f;
            }
        }
    }

    if (depthPass)
    {
        glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model3));
    }
    else
    {
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model3));
    }

    if (!depthPass)
    {
        // send door knob normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc2, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * model3))));
    }

    // draw door knob
    doorKnob.Draw(shader);

    // send door knob lock model matrix data to shader
    if (animateOnce == true)
    {
        if (total_angle1 <= 120.0f)
        {
            model4 = glm::translate(model4, glm::vec3(-9.2f, 2.88f, -11.67f));
            model4 = glm::rotate(model4, glm::radians(angle1), glm::vec3(0.0f, 1.0f, 0.0f));
            model4 = glm::translate(model4, glm::vec3(9.2f, -2.88f, 11.67f));

            total_angle1 += angle1;
        }

        if (total_movement <= 0.08)
        {
            model4 = glm::translate(model4, glm::vec3(0, 0, sign2 * movement));
            total_movement += movement;

            if (total_movement >= 0.04 && sign2 == -1.0f)
            {
                sign2 = 1.0f;
            }
        }
    }

    if (depthPass)
    {
        glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model4));
    }
    else
    {
        glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model4));
    }

    if (!depthPass)
    {
        // send door knob lock normal matrix data to shader
        glUniformMatrix3fv(normalMatrixLoc2, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * model4))));
    }

    // draw door knob lock
    doorKnobLock.Draw(shader);
}

void render() 
{
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

	// render the scenes
    renderInteriorScene(depthMapShader, true);
    renderExteriorScene(depthMapShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, (float)myWindow.getWindowDimensions().width, (float)myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT);

    if (showDepthMap)
    {
        screenQuadShader.useShaderProgram();

        // bind the depth map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 3);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        //view = myCamera.getViewMatrix();
        //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

        // render the scenes with shadows
        renderInteriorScene(myBasicShaderWithoutFog, false);
        renderExteriorScene(myBasicShader, false);
    }

	// render the teapot
	//renderTeapot(myBasicShader);

    mySkyBox.Draw(skyboxShader, view, projection);
}

void cleanup() 
{
    myWindow.Delete();
    //cleanup code for your own data
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    glDeleteTextures(1, &depthMapTexture);
    glfwTerminate();
}

int main(int argc, const char* argv[]) 
{
    try 
    {
        initOpenGLWindow();
    }
    catch (const exception& e) 
    {
        cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    /*
    glm::vec4 vv(0.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 mm(1.0f);

    cout << glm::to_string(mm) << '\n';
    
    mm = glm::rotate(mm, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    cout << glm::to_string(mm) << '\n';

    mm = glm::translate(mm, glm::vec3(1.0f, 2.0f, 0.0f));

    cout << glm::to_string(mm) << '\n';

    cout << glm::to_string(mm * vv);

    return 0;
    */

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    initUniforms2();
    initUniforms3();
    initSkybox();
    initFBO();
    setWindowCallbacks();

	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) 
    {
        processMovement();
	    render();
        stairs();
        flashlightUniforms();
        cameraAnimation();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}