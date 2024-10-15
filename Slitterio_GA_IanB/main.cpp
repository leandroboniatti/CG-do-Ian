
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

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca
using namespace glm;	// Para não precisar digitar glm:: na frente de comandos da biblioteca

// Funções para inicializar shaders
//GLuint LoadShader(const char* vertexPath, const char* fragmentPath);
GLuint LoadShader();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Snake& snake);

void aplicaTransformacoes(GLuint shaderID, GLuint VAO, vec3 posicaoNaTela, float anguloDeRotacao, vec3 escala, vec3 color, vec3 eixoDeRotacao = (vec3(0.0, 0.0, 1.0)));

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

const GLchar* vertexShaderSource = "#version 400\n"		// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
"layout (location = 0) in vec2 aPos;\n"	// "position" recebe as informações que estão no local 0 -> definidas no setupGeometry() -> glVertexAttribPointer(0, xxxxxxxx);
"uniform mat4 projection;\n"				// "projection" receberá as informações da forma de projeção escolhida
"uniform mat4 model;\n"						// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)
"void main()\n"
"{\n"
//...pode ter mais linhas de código para outros atributos, como cor, textura e normalização 
"gl_Position = projection * model * vec4(aPos, 0.0, 1.0);\n"	// era: vec4(position.x, position.y, position.z, 1.0);\n	
"}\0";														// "gl_Position" é uma variável específica do GLSL que recebe a posição final do vertice processado
		// sempre nessa ordem: projection * model * 		// é vec4 por causa das multiplicações de matrizes, usadas para translação, rotação e escala.

const GLchar* fragmentShaderSource = "#version 400\n"	//Código fonte do Fragment Shader (em GLSL - Graphics Library Shading Language)
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n" // Cor verde para a cobrinha
"}\0";



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
    //GLuint shaderProgram = LoadShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    GLuint shaderProgram = LoadShader();

    setupSnakeSegment(); 

    glUseProgram(shaderProgram);

    // Definindo a matriz de projeção ortográfica
    //glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

    mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);  	// ortho(Left, Right, Bottom, Top, Near, Far)
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));


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

        aplicaTransformacoes(shaderProgram, VAO, vec3(snake.getSegments()[0],0.0), 0.0, vec3(1,1,1), vec3(1,0,0));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        //glBindVertexArray(0);


        // Renderiza a cena
        //glClear(GL_COLOR_BUFFER_BIT);

        // Renderiza a cobrinha
        //for (const auto& segment : snake.getSegments()) {
        //    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(segment, 0.0f));
        //    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        //    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

        //    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            // Desenhe a geometria aqui (como um quadrado ou retângulo representando o segmento)
        //}

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        //glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);	// Pede pra OpenGL desalocar os buffers

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
//GLuint LoadShader(const char* vertexPath, const char* fragmentPath) {

GLuint LoadShader() {

	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);

	glDeleteShader(fragmentShader);

	return shaderProgram;
}


void aplicaTransformacoes(GLuint shaderID, GLuint VAO, vec3 posicaoNaTela, float anguloDeRotacao, vec3 escala, vec3 color, vec3 eixoDeRotacao) {
	
	/*** Transformações na geometria (objeto) -> sempre na ordem Translação - Rotação - Escala ***/

	//Matriz de modelo inicial
	mat4 model = mat4(1); //salva em model a matriz identidade 4x4

	//Translação
	model = translate(model,posicaoNaTela);

	//Rotação 
	model = rotate(model,radians(anguloDeRotacao),eixoDeRotacao);

	//Escala
	model = scale(model,escala);
	
	// Transformações sendo enviadas para "model"
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	// Cores sendo enviadas para "inputColor"
	glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b , 1.0f);
	
}