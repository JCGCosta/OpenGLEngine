// Lab 03 - Parte 2 de Computação Grafica
// Aluno: Júlio César Guimarães Costa
// RA: 2203049
// Para a execução é necessaria a inclusão de 3 bibliotecas externas bem como a configuração (GLEW, GLFW, GLM).
// Existe tambem a necessidade da existencia de 1 biblioteca interna (math).
// VEJA O CONSOLE - para as mensagens de colisão
#include <iostream>

//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

// GLM header file
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
using namespace glm;

// Math and Time Libraries
#define _USE_MATH_DEFINES // -> To get the value from PI
#include <math.h> // -> To get the value from PI
#include <random> // -> To use the function srand to generate random floats
#include <ctime>  // -> To set seed randominess based on the computer time

//Global Variables
const int gWindowWidth = 800; // Tamanho da janela na horizontal
const int gWindowHeight = 800; // Tamanho da janela na vertical
const char* TITLE = { "Júlio César - 2203049" }; // Titulo da Janela
GLFWwindow* pWindow = NULL; // GLFW window
unsigned int seed;

// object definitions
#define numOBJ 2 // Number of Objects
#define SquarePlayerID 0
#define SquareBallID 1

GLuint vao[numOBJ], vbo[numOBJ];
GLuint program[numOBJ];
GLfloat xCenter[numOBJ], yCenter[numOBJ], xSize[numOBJ], ySize[numOBJ], movSpeed[numOBJ], rotation[numOBJ], xScale[numOBJ], yScale[numOBJ];
GLboolean gWireframe = false;
GLfloat xScreenI = -40, xScreenF = 40;
GLfloat yScreenI = -40, yScreenF = 40;
GLboolean startGame = GL_FALSE;

// ballMV = 1 (esquerda, baixo)
// ballMV = 2 (esquerda, cima)
// ballMV = 3 (direita, baixo)
// ballMV = 4 (direita, cima)
GLint ballMv;

// Keys and OpenGL functions

void glfw_key(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(pWindow, GL_TRUE);

    // Cardinals
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        yCenter[SquarePlayerID] += movSpeed[SquarePlayerID];
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        yCenter[SquarePlayerID] -= movSpeed[SquarePlayerID];
    }

    if (key == GLFW_KEY_S && (action == GLFW_PRESS))
    {
        startGame = GL_TRUE;
    }
    
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        gWireframe = !gWireframe;
        if (gWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

bool initOpenGL(GLfloat r, GLfloat g, GLfloat b, GLfloat o) //Inicialização GLFW e OpenGL
{
    /* Initialize the library */
    if (!glfwInit()) // Verifica a inicialização e configuração da biblioteca glfw
    {
        std::cerr << "GLFW initialization failed" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Configura o tipo de janela
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Configura o tipo de janela
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Configura o tipo de janela
    //only to support MAC
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Configura o tipo de janela

    /* Create a windowed mode window and its OpenGL context */
    pWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, TITLE, NULL, NULL); // Comando que cria a tela com os parametros definidos nas variaveis globais
    if (pWindow == NULL) // Verifica se ocorreu tudo corretamente na execução da função glfwCreateWindow
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // Caso não tenha ocorrido tudo bem, termina o glfw
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(pWindow);


    //Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    //Entre 0 e 1 - 0 -> preto, 1 - Branco (0 a 255, 0 -> preto, 255 - Branco);
    glClearColor(r, g, b, o); // Define a cor do interior do background da janela no formato RGB e opacidade
    glfwSetKeyCallback(pWindow, glfw_key);
    return true;
}

void shutdown()
{
    glDeleteVertexArrays(numOBJ, vao);
    glDeleteBuffers(numOBJ, vbo);
}

void attachShaders(GLint numProgram, const GLchar* vsSrc, const GLchar* fsSrc) {
    //Create vertex shader
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSrc, NULL);
    glCompileShader(vs);

    //Create fragment shader
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSrc, NULL);
    glCompileShader(fs);

    //Create program shader
    program[numProgram] = glCreateProgram();
    glAttachShader(program[numProgram], vs);
    glAttachShader(program[numProgram], fs);
    glLinkProgram(program[numProgram]);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void RenderObj(GLint objIND, GLint objType, GLint numVertices)
{
    glBindVertexArray(vao[objIND]);
    if (objType == 1) glDrawArrays(GL_POINTS, 0, numVertices);
    if (objType == 2) glDrawArrays(GL_LINE, 0, numVertices);
    if (objType == 3) glDrawArrays(GL_LINE_STRIP, 0, numVertices);
    if (objType == 4) glDrawArrays(GL_TRIANGLES, 0, numVertices);
    if (objType == 5) glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
    glBindVertexArray(0);
}

// Obj Config functions

void configRotating(GLint numProgram, GLfloat radians, GLfloat x, GLfloat y, GLfloat z) {
    //scaling
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(radians), glm::vec3(x, y, z));
    GLuint matrix = glGetUniformLocation(program[numProgram], "rotating");
    glUniformMatrix4fv(matrix, 1, GL_FALSE, glm::value_ptr(trans));
}

void configScaling(GLint numProgram, GLfloat x, GLfloat y, GLfloat z) {
    //scaling
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::scale(trans, glm::vec3(x, y, z));
    GLuint matrix = glGetUniformLocation(program[numProgram], "scaling");
    glUniformMatrix4fv(matrix, 1, GL_FALSE, glm::value_ptr(trans));
}

void configTranslation(GLint numProgram, GLfloat x, GLfloat y, GLfloat z) {
    //translation
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(x, y, z));
    GLuint matrix = glGetUniformLocation(program[numProgram], "translation");
    glUniformMatrix4fv(matrix, 1, GL_FALSE, glm::value_ptr(trans));
}

void configProjection(GLint numProgram, GLfloat xi, GLfloat xf, GLfloat yi, GLfloat yf, GLfloat zi, GLfloat zf) {
    //projection
    glm::mat4 projection = glm::ortho(xi, xf, yi, yf, zi, zf);
    GLint modelLoc2 = glGetUniformLocation(program[numProgram], "projection");
    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(projection));
}

void ConfigObj(GLint objIND, GLfloat Pxi, GLfloat Pxf, GLfloat Pyi, GLfloat Pyf, GLfloat Pzi, GLfloat Pzf, GLfloat Tx, GLfloat Ty, GLfloat Tz, GLfloat Sx, GLfloat Sy, GLfloat Sz, GLfloat Rradians, GLfloat Rx, GLfloat Ry, GLfloat Rz) {
    glUseProgram(program[objIND]);
    configProjection(objIND, Pxi, Pxf, Pyi, Pyf, Pzi, Pzf);
    configScaling(objIND, Sx, Sy, Sz);
    configTranslation(objIND, Tx, Ty, Tz);
    configRotating(objIND, Rradians, Rx, Ry, Rz);
}

int checkCollisionToScreenBounds(GLint objIND)
{
    // check collision with the right corner
    if (xCenter[objIND] + (xSize[objIND] / 2) > xScreenF)
    {
        xCenter[objIND] = xScreenF - (xSize[objIND] / 2);
        return 1;
    }
    // check collision with the left corner
    if (xCenter[objIND] - (xSize[objIND] / 2) < xScreenI)
    {
        xCenter[objIND] = xScreenI + (xSize[objIND] / 2);
        return 2;
    }
    // check collision with the up corner
    if (yCenter[objIND] + (ySize[objIND] / 2) > yScreenF)
    {
        yCenter[objIND] = yScreenF - (ySize[objIND] / 2);
        return 3;
    }
    // check collision with the down corner
    if (yCenter[objIND] - (ySize[objIND] / 2) < yScreenI)
    {
        yCenter[objIND] = yScreenI + (ySize[objIND] / 2);
        return 4;
    }
    return 0;
}

GLboolean checkCollisionBetweenObj(GLint objIND1, GLint objIND2, GLboolean solidCollision)
{
    // check if collision happen
    if (xCenter[objIND1] + (xSize[objIND1] / 2) > xCenter[objIND2] - (xSize[objIND2] / 2)
        && xCenter[objIND1] - (xSize[objIND1] / 2) < xCenter[objIND2] + (xSize[objIND2] / 2)
        && yCenter[objIND1] + (ySize[objIND1] / 2) > yCenter[objIND2] - (ySize[objIND2] / 2)
        && yCenter[objIND1] - (ySize[objIND1] / 2) < yCenter[objIND2] + (ySize[objIND2] / 2))
    {
        std::cout << "Log: " << objIND1 << " and " << objIND2 << " are colliding.\n";
        if (solidCollision == GL_TRUE)
        {
            // if collision is from right1 to left2
            if (xCenter[objIND1] < xCenter[objIND2] && fabs(yCenter[objIND1] - yCenter[objIND2]) + (movSpeed[objIND1] + movSpeed[objIND2]) < ((xSize[objIND1] / 2) + (xSize[objIND2] / 2)))
            {
                std::cout << "-> From RIGHT_1 to LEFT_2.\n";
                xCenter[objIND1] = (xCenter[objIND2] - (xSize[objIND2] / 2)) - (xSize[objIND1] / 2);
                return GL_TRUE;
            }
            // if collision is from left1 to right2
            if (xCenter[objIND1] > xCenter[objIND2] && fabs(yCenter[objIND1] - yCenter[objIND2]) + (movSpeed[objIND1] + movSpeed[objIND2]) < ((xSize[objIND1] / 2) + (xSize[objIND2] / 2)))
            {
                std::cout << "-> From LEFT_1 to RIGHT_2.\n";
                xCenter[objIND1] = (xCenter[objIND2] + (xSize[objIND2] / 2)) + (xSize[objIND1] / 2);
                return GL_TRUE;
            }
            // if collision is from up1 to down2
            if (yCenter[objIND1] < yCenter[objIND2])
            {
                std::cout << "-> From UP_1 to DOWN_2.\n";
                yCenter[objIND1] = (yCenter[objIND2] - (ySize[objIND2] / 2)) - (ySize[objIND1] / 2);
                return GL_TRUE;
            }
            // if collision is from down1 to up2
            if (yCenter[objIND1] > yCenter[objIND2])
            {
                std::cout << "-> From DOWN_1 to UP_2.\n";
                yCenter[objIND1] = (yCenter[objIND2] + (ySize[objIND2] / 2)) + (ySize[objIND1] / 2);
                return GL_TRUE;
            }
        } 
        return GL_TRUE;
    }
    return GL_FALSE;
}

void speedGrow(GLint objIND, GLfloat incrementation)
{
    movSpeed[objIND] += incrementation;
}

void BallMov() {
   
    int getCol = checkCollisionToScreenBounds(SquareBallID);

    GLfloat difficultIncremention = 0.0025f;

    if (checkCollisionBetweenObj(SquareBallID, SquarePlayerID, GL_FALSE) && ballMv == 1)
    {
        speedGrow(SquareBallID, difficultIncremention); // Difficult and balancing changing
        speedGrow(SquarePlayerID, difficultIncremention * 2); // Difficult and balancing changing
        ballMv = 3;
    }

    if (checkCollisionBetweenObj(SquareBallID, SquarePlayerID, GL_FALSE) && ballMv == 2)
    {
        speedGrow(SquareBallID, difficultIncremention); // Difficult and balancing changing
        speedGrow(SquarePlayerID, difficultIncremention * 2); // Difficult and balancing changing
        ballMv = 4;
    }

    if (getCol == 4)
    {
        speedGrow(SquareBallID, difficultIncremention); // Difficult and balancing changing
        if (ballMv == 3) ballMv = 4;
        if (ballMv == 1) ballMv = 2;
    }

    if (getCol == 3)
    {
        speedGrow(SquareBallID, difficultIncremention); // Difficult and balancing changing
        if (ballMv == 2) ballMv = 1;
        if (ballMv == 4) ballMv = 3;
    }

    if (getCol == 2)
    {
        std::cout << "GameOver.\n";
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }

    if (getCol == 1)
    {
        speedGrow(SquareBallID, difficultIncremention); // Difficult and balancing changing
        if (ballMv == 3) ballMv = 1;
        if (ballMv == 4) ballMv = 2;
    }

    

    // ballMV = 1 (esquerda, baixo)
    // ballMV = 2 (esquerda, cima)
    // ballMV = 3 (direita, baixo)
    // ballMV = 4 (direita, cima)
    if (ballMv == 1)
    {
        xCenter[SquareBallID] -= movSpeed[SquareBallID];
        yCenter[SquareBallID] -= movSpeed[SquareBallID];
    }
    if (ballMv == 2)
    {
        xCenter[SquareBallID] -= movSpeed[SquareBallID];
        yCenter[SquareBallID] += movSpeed[SquareBallID];
    }
    if (ballMv == 3)
    {
        xCenter[SquareBallID] += movSpeed[SquareBallID];
        yCenter[SquareBallID] -= movSpeed[SquareBallID];
    }
    if (ballMv == 4)
    {
        xCenter[SquareBallID] += movSpeed[SquareBallID];
        yCenter[SquareBallID] += movSpeed[SquareBallID];
    }


}

// Objects configuration

void Square(GLint objIND, GLfloat xStartPos, GLfloat yStartPos, GLfloat xCollisionBoxSize, GLfloat yCollisionBoxSize, GLfloat xSca, GLfloat ySca, GLfloat moviSpeed, GLfloat rot, GLfloat r, GLfloat g, GLfloat b)
{
    xCenter[objIND] = xStartPos;
    yCenter[objIND] = yStartPos;
    xSize[objIND] = xCollisionBoxSize;
    ySize[objIND] = yCollisionBoxSize;
    movSpeed[objIND] = moviSpeed;
    rotation[objIND] = rot;
    xScale[objIND] = xSca;
    yScale[objIND] = ySca;

    GLfloat vertices[]{
        // Square
        -10.0f,  -10.0f,  0.0f, r,  g,  b,
        10.0f, -10.0f,  0.0f, r,  g,  b,
        10.0f, 10.0f, 0.0f, r,  g,  b,

        -10.0f,  -10.0f,  0.0f, r,  g,  b,
        10.0f, 10.0f, 0.0f, r,  g,  b,
        -10.0f,  10.0f, 0.0f, r,  g,  b,
    };

    //Modern Opengl requires that we use a vertex array object
    glGenVertexArrays(1, &vao[objIND]);
    glBindVertexArray(vao[objIND]);

    //buffer vert_pos
    glGenBuffers(1, &vbo[objIND]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[objIND]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (GLvoid*)(sizeof(GLfloat) * 3));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //Shaders
    const GLchar* vertexShaderSrc =
        "#version 330 core\n"
        "layout (location = 0) in vec3 pos;"
        "layout (location = 1) in vec3 color;"
        "\n"
        "uniform mat4 projection;"
        "uniform mat4 translation;"
        "uniform mat4 scaling;"
        "uniform mat4 rotating;"
        "out vec3 vert_color;"
        "void main()"
        "{"
        "   vert_color = color;"
        "   gl_Position = projection * translation * scaling * rotating * vec4(pos.x, pos.y, pos.z, 1.0);"
        "}";

    const GLchar* fragmentShaderSrc =
        "#version 330 core\n"
        "in vec3 vert_color;"
        "out vec4 frag_color;"
        "void main()"
        "{"
        "    frag_color = vec4(vert_color, 1.0f);"
        "}";

    attachShaders(objIND, vertexShaderSrc, fragmentShaderSrc);
}

// Função Principal

int main(void)
{
    srand((unsigned int)time(NULL));
    seed = (unsigned int)time(NULL);
    ballMv = (rand() % 4) + 1;

    if (!initOpenGL(0.5f, 0.5f, 0.5f, 0.0f)) // Verificar se a função initopenGL conseguiu inicializar e configurar o OpenGL com sucesso
    {
        //ocorrencia de erro
        std::cerr << "GLFW initialization failed" << std::endl;
        return -1;
    }

    // Create View Objects
    // 
    // Object_Creation_function(objID, xStartPos, yStartPos, xSizeCollisionBox, ySizeCollisionBox, movSpeed)
    Square(SquarePlayerID, -35.0f, 0.0f, 5.0f, 20.0f, 0.25f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f); // Player
    Square(SquareBallID, 0.0f, 0.0f, 4.0f, 4.0f, 0.2f, 0.2f, 0.01f, 0.0f, 0.0f, 1.0f, 0.0f); // Ball

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(pWindow)) // Enquanto a janela não for fechada
    {

        /* Poll for and process events */
        glfwPollEvents(); // Cria os eventos e processos do glfw

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT); // Renderiza a imagem

        // Render (objType, objIND, numVertices)
        // obType = 1 => GL_POINTS
        // obType = 2 => GL_LINE
        // obType = 3 => GL_LINE_STRIP
        // obType = 4 => GL_TRIANGLES
        // obType = 5 => GL_TRIANGLE_FAN

        // Player Configuration
        ConfigObj(SquarePlayerID,
            xScreenI, xScreenF, yScreenI, yScreenF, -1.0f, 1.0f,
            xCenter[SquarePlayerID], yCenter[SquarePlayerID], 0.0f,
            xScale[SquarePlayerID], yScale[SquarePlayerID], 0.0f,
            rotation[SquarePlayerID], 0.0f, 0.0f, 1.0f);    
        RenderObj(SquarePlayerID, 4, 6);

        // Ball Configuration
        ConfigObj(SquareBallID,
            xScreenI, xScreenF, yScreenI, yScreenF, -1.0f, 1.0f,
            xCenter[SquareBallID], yCenter[SquareBallID], 0.0f,
            xScale[SquareBallID], yScale[SquareBallID], 0.0f,
            rotation[SquareBallID], 0.0f, 0.0f, 1.0f);
        RenderObj(SquareBallID, 4, 6);

        /* Swap front and back buffers */
        glfwSwapBuffers(pWindow); // Troca os buffers

        // Objects Collision
        checkCollisionToScreenBounds(SquarePlayerID);
        BallMov();

        
        
    }

    //Clean UP
    shutdown();
    glfwTerminate(); // Finaliza os processos criados pelo glfw na criação e renderização da janela
    return 0;
}