/* Segue o Mouse - código adaptado de https://learnopengl.com/Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Fundamentos de Computação Gráfica - Unisinos
 * Versão inicial: 05/10/2024 (ver gravação da aula)
 * Última atualização em 17/10/2024
 *
 * Este programa desenha um triângulo que segue o cursor do mouse
 * usando OpenGL e GLFW.
 * A posição e a rotação do triângulo são calculadas com base no movimento do mouse.
 */

#include <iostream>
#include <string>
#include <assert.h>

// Bibliotecas GLAD para carregar funções OpenGL
#include <glad/glad.h>

// Biblioteca GLFW para criar janelas e gerenciar entrada de teclado/mouse
#include <GLFW/glfw3.h>

// GLM para operações matemáticas (vetores, matrizes)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <vector>

using namespace std;
using namespace glm;

// Constantes
const float Pi = 3.14159265;
const GLuint WIDTH = 800, HEIGHT = 600; // Dimensões da janela

// Estrutura para armazenar informações sobre as geometrias da cena
struct Geometry {
    GLuint VAO;        // Vertex Array Geometry
    vec3 position;     // Posição do objeto
    float angle;       // Ângulo de rotação
    vec3 dimensions;   // Escala do objeto (largura, altura)
    vec3 color;        // Cor do objeto
    int nVertices;     // Número de vértices a desenhar
};

// Variáveis globais
bool keys[1024];   // Estados das teclas (pressionadas/soltas)
vec2 mousePos;     // Posição do cursor do mouse
vec3 dir = vec3(0.0, -1.0, 0.0); // Vetor direção (do objeto para o mouse)

// Protótipos das funções
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
int setupShader();           // Função para configurar os shaders
int setupGeometry();         // Função para configurar a geometria (triângulo)
void drawGeometry(GLuint shaderID, GLuint VAO, int nVertices, vec3 position, vec3 dimensions, float angle, vec3 color, GLuint drawingMode = GL_TRIANGLES, vec3 axis = vec3(0.0, 0.0, 1.0));

int main() {
    // Inicializa GLFW e configurações de versão do OpenGL
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Seguindo o Mouse", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // Inicializa GLAD para carregar todas as funções OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Informações sobre o Renderer e a versão OpenGL
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "Versão OpenGL suportada: " << version << endl;

    // Configurações de viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compila o shader e cria a geometria (triângulo)
    GLuint shaderID = setupShader();
    Geometry triangle;
    triangle.VAO = setupGeometry();
    triangle.position = vec3(0.0, 0.0, 0.0);
    triangle.color = vec3(1.0, 1.0, 0.0);  // Amarelo
    triangle.dimensions = vec3(50.0, 50.0, 1.0);
    triangle.nVertices = 3;  // Triângulo

    // Ativa o teste de profundidade
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS); // Sempre passa no teste de profundidade (desnecessário se não houver profundidade)

    glUseProgram(shaderID);

    // Matriz de projeção ortográfica (usada para desenhar em 2D)
    mat4 projection = ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Loop da aplicação
    while (!glfwWindowShouldClose(window)) {
        // Processa entradas (teclado e mouse)
        glfwPollEvents();

        // Limpa a tela
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pega a posição do mouse e calcula a direção
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        mousePos = vec2(xPos, height - yPos);  // Inverte o eixo Y para se alinhar à tela
        vec3 dir = normalize(vec3(mousePos, 0.0) - triangle.position);
        float angle = atan2(dir.y, dir.x);

        // Move o triângulo suavemente na direção do mouse
        if (distance(triangle.position, vec3(mousePos, 0.0)) > 0.01f) {
            triangle.position += 0.5f * dir;  // Aumente ou diminua 0.5f para controlar a velocidade
        }

        // Atualiza o ângulo de rotação do triângulo
        triangle.angle = angle + radians(-90.0f); // Rotaciona para que a ponta aponte para o mouse

        // Desenha o triângulo e o cursor
        drawGeometry(shaderID, triangle.VAO, triangle.nVertices, triangle.position, triangle.dimensions, triangle.angle, triangle.color);
        drawGeometry(shaderID, triangle.VAO, 3, vec3(mousePos, 0.0), vec3(10.0, 10.0, 1.0), 0.0f, vec3(1.0, 0.0, 1.0));

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }

    // Limpa a memória alocada pelos buffers
    glDeleteVertexArrays(1, &triangle.VAO);
    glfwTerminate();
    return 0;
}

// Callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
        keys[key] = true;
    if (action == GLFW_RELEASE)
        keys[key] = false;
}


// Configura e compila os shaders
int setupShader() {
    // Código do vertex shader
    const GLchar *vertexShaderSource = R"(
    #version 400
    layout (location = 0) in vec3 position;
    uniform mat4 projection;
    uniform mat4 model;
    void main() {
        gl_Position = projection * model * vec4(position, 1.0);
    })";

    // Código do fragment shader
    const GLchar *fragmentShaderSource = R"(
    #version 400
    uniform vec4 inputColor;
    out vec4 color;
    void main() {
        color = inputColor;
    })";

    // Compilação do vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Verificando erros de compilação do vertex shader
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compilação do fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Verificando erros de compilação do fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Linkando os shaders no programa
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    // Verificando erros de linkagem do programa de shaders
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Limpando os shaders compilados após o link
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderID;
}

// Configura a geometria do triângulo
int setupGeometry() {
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, // Vértice 1
         0.5f, -0.5f, 0.0f, // Vértice 2
         0.0f,  0.5f, 0.0f  // Vértice 3
    };

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Desvincula o VAO e o VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

// Função para desenhar o objeto
void drawGeometry(GLuint shaderID, GLuint VAO, int nVertices, vec3 position, vec3 dimensions, float angle, vec3 color, GLuint drawingMode, vec3 axis) {
    glBindVertexArray(VAO); // Vincula o VAO

    // Aplica as transformações de translação, rotação e escala
    mat4 model = mat4(1.0f);
    model = translate(model, position);
    model = rotate(model, angle, axis);
    model = scale(model, dimensions);

    // Envia a matriz de modelo ao shader
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
    
    // Envia a cor do objeto ao shader
    glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b, 1.0f);

    // Desenha o objeto
    glDrawArrays(drawingMode, 0, nVertices);

    // Desvincula o VAO
    glBindVertexArray(0);
}
