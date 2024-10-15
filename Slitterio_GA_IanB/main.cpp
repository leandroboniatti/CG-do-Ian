
#include <iostream>
#include <string>
#include <cmath>
#include <assert.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <snake.h>
#include <item.h>
#include <collision.h>

// Funções para inicializar shaders
GLuint LoadShader(const char* vertexPath, const char* fragmentPath);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Snake& snake);

unsigned int VAO, VBO;

Item item(200.0f, 200.0f);

//if (glm::distance(snake.getSegments()[0], item.getPosition()) < 10.0f) { 
//    snake.grow();
//    // Gere um novo item em uma posição aleatória
//}


void setupSnakeSegment() {
    float vertices[] = {
        -10.0f, -10.0f, // Inferior esquerdo
        10.0f, -10.0f,  // Inferior direito
        10.0f, 10.0f,   // Superior direito
        -10.0f, 10.0f   // Superior esquerdo
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


int main() {
    // Inicializa o GLFW
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        return -1;
    }

    // Configurações da janela GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Snake Game", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar a janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Inicializa GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Compilando shaders
    GLuint shaderProgram = LoadShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    glUseProgram(shaderProgram);

    // Definindo a matriz de projeção ortográfica
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

    Snake snake;

    if (glm::distance(snake.getSegments()[0], item.getPosition()) < 10.0f) { 
        snake.grow();
        // Gere um novo item em uma posição aleatória
    }


    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Processa entradas do usuário
        processInput(window, snake);

        // Atualiza o estado da cobrinha
        snake.move(0.016f); // Assumindo um deltaTime fixo de 0.016 segundos (~60 FPS)

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);


        // Renderiza a cena
        glClear(GL_COLOR_BUFFER_BIT);

        // Renderiza a cobrinha
        for (const auto& segment : snake.getSegments()) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(segment, 0.0f));
            GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

            // Desenhe a geometria aqui (como um quadrado ou retângulo representando o segmento)
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpa os recursos
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, Snake& snake) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        snake.setDirection(glm::vec2(0.0f, 1.0f)); // Cima
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        snake.setDirection(glm::vec2(0.0f, -1.0f)); // Baixo
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        snake.setDirection(glm::vec2(-1.0f, 0.0f)); // Esquerda
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        snake.setDirection(glm::vec2(1.0f, 0.0f)); // Direita
    }
}

// Callback para redimensionamento de janela
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Função para carregar shaders (implementação simplificada)
GLuint LoadShader(const char* vertexPath, const char* fragmentPath) {
    
    
    
    // Código para carregar e compilar os shaders
}
