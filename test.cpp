#include <iostream>
#include <stdlib.h>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>

extern "C" {

GLint width = 800;
GLint height = 800;
GLuint vboIds[2];
GLint bgPosLoc;
GLint rotLoc;
GLint posLoc;
GLfloat rotation = 0.0f;

// programs
GLuint backgroundProgram;
GLuint simpleProgram;

GLfloat backgroundVertices[] = {
    1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
};
GLubyte backgroundIndices[] = {0, 1, 2, 0, 2, 3};

void windowSizeCallback(GLFWwindow* window, int w, int h)
{
    width = static_cast<GLint>(w);
    height = static_cast<GLint>(h);
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // update rotation
    rotation = (float)(xpos / (double)width) - 0.5f;
}

GLuint loadShader(GLenum type, const GLchar *shaderSrc)
{
    // maybe support binary shaders one day, we have the shader source separate anyway
    GLuint shader = glCreateShader(type);
    if(shader == 0)
        return 0;

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if(!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            printf("Error compiling shader:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
// Initialize shader and program object
int init()
{
    GLchar backgroundVShaderStr[] =
        #include "background.vert.h"
        ;

    GLchar backgroundFShaderStr[] =
        #include "background.frag.h"
        ;

    GLchar simpleVShaderStr[] =
        #include "simple.vert.h"
        ;

    GLchar simpleFShaderStr[] =
        #include "simple.frag.h"
        ;

    GLuint vShader;
    GLuint fShader;
    GLuint programObject;
    GLint linked;

    // Load shaders
    vShader = loadShader(GL_VERTEX_SHADER, backgroundVShaderStr);
    fShader = loadShader(GL_FRAGMENT_SHADER, backgroundFShaderStr);
    programObject = glCreateProgram();
    if(programObject == 0 )
        return 0;
    glAttachShader(programObject, vShader);
    glAttachShader(programObject, fShader);
    glLinkProgram(programObject);
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if(!linked) {
        printf("not linked!\n");
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            printf("Error linking background program\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(programObject);
        return GL_FALSE;
    }
    backgroundProgram = programObject;
    bgPosLoc = glGetAttribLocation(backgroundProgram, "vPosition");
    printf("backgroundProgram vPosition = %d\n", bgPosLoc);
    
    vShader = loadShader(GL_VERTEX_SHADER, simpleVShaderStr);
    fShader = loadShader(GL_FRAGMENT_SHADER, simpleFShaderStr);
    programObject = glCreateProgram();
    if(programObject == 0 )
        return 0;
    glAttachShader(programObject, vShader);
    glAttachShader(programObject, fShader);
    glLinkProgram(programObject);
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if(!linked) {
        printf("not linked!\n");
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            printf("Error linking simple program\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(programObject);
        return GL_FALSE;
    }
    simpleProgram = programObject;
    posLoc = glGetAttribLocation(simpleProgram, "vPosition");
    rotLoc = glGetUniformLocation(simpleProgram, "vRotation");
    printf("simpleProgram vPosition = %d\n", posLoc);
    printf("simpleProgram vRotation = %d\n", rotLoc);

    // create VBO
    static const GLfloat vboData[] = {
        0.0f, 0.5f, 0.0f,
        -0.15f, -0.5f, 0.0f,
        0.15f, -0.5f, 0.0f,
    };
    glGenBuffers(1, vboIds);
    // glBindBuffer happens in draw()
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vboData), vboData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return GL_TRUE;
}

void draw()
{
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // reset ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // draw background
    // FIXME: client-side vertex arrays (like backgroundVertices) are
    // not supported on some platforms i.e. WebGL
    glUseProgram(backgroundProgram);
    glEnableVertexAttribArray(bgPosLoc);
    glVertexAttribPointer(bgPosLoc, 3, GL_FLOAT, GL_FALSE, 0, backgroundVertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, backgroundIndices);
    glDisableVertexAttribArray(bgPosLoc);

    // draw objects
    glUseProgram(simpleProgram);
    //glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glEnableVertexAttribArray(posLoc);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glUniform1f(rotLoc, rotation);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(posLoc);
    
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main(int argc, char **argv) {
    GLFWwindow* window;
    if(!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(width, height, "GLES2 test", NULL, NULL);
    if(!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if(!init())
        exit(EXIT_FAILURE);

    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    while(!glfwWindowShouldClose(window)) {
        draw();
        glfwSwapBuffers(window);
        //glfwPollEvents();
        glfwWaitEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

} // extern "C"
