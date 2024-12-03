/*** Hello Triangle ***/
/*** Disciplina de Computação Gráfica - Jogos Digitais - Unisinos ***/
/*** Aluno: Ian Rossetti Boniatti ***/
/*** Código adaptado da adaptaçao de Rossana Baptista Queiroz de https://learnopengl.com/#!Getting-started/Hello-Triangle ***/


/*** INCLUDES ***/

#include <iostream>
#include <string>
#include <cmath>
#include <assert.h>
#include <stb_image.h>

#include <glad/glad.h> 	// biblioteca de funções baseada nas definições/especificações OPENGL
						// Certifique-se de incluir a GLAD antes de outros arquivos de cabeçalho que requerem OpenGL (como GLFW)

#include <GLFW/glfw3.h> // biblioteca de funções para criação da janela no Windows

#include <glm/glm.hpp>	// biblioteca de operações matriciais
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca
using namespace glm;	// Para não precisar digitar glm:: na frente de comandos da biblioteca


// Estrutura para armazenar informações sobre um determinado elemento
struct Sprite {
	GLfloat VAO; 	//id do buffer de geometria
	GLfloat texID; 	//id da textura
	vec3 pos, dimensions;
	float angle;
	//Para a animação da spritesheet
	int nAnimations, nFrames;
	int iAnimation, iFrame;
	float ds, dt;
};


/*** Protótipos das funções ***/
void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode); // Função de callback de teclado
int  setupShader ();		// Função responsável pela compilação e montagem do programa de shader
int  setupSprite();
void drawSprite(GLuint shaderID, Sprite &sprite);
void updateSprite(GLuint shaderID, Sprite &sprite);
Sprite initializeSprite(GLuint texID, vec3 dimensions, vec3 position, int nAnimations=1, int nFrames=1, float angle=0.0);
GLuint loadTexture(string filePath, int &width, int &height);	// Protótipo da função responsável pelo carregamento da textura

// Função responsável pela aplicação das transformações de Translação, Rotação e Escala
void aplicaTransformacoes (	GLuint shaderID,			// 1º parâmetro: identificador do programa de shader
							GLuint VAO,					// 2º parâmetro: identificador do VAO do elemento que será processado
							vec3 posicaoNaTela,			// 3º parâmetro: posição para onde será transladado o elemento
							float anguloDeRotacao,		// 4º parâmetro: rotação a ser aplicada ao elemento de acordo com o eixo de rotação determinado no 6º parâmetro
							vec3 escala,				// 5º parâmetro: dimensão final do elemento
							vec3 color,					// 6º parâmetro: cor do elemento
							vec3 eixoDeRotacao = (vec3(0.0, 0.0, 1.0)) ); // 7º parâmetro: eixo no qual se processará a rotação estipulada no 4º parâmetro. Defauld -> eixo "z"

// Sobrecarga da função responsável pela aplicação das transformações de Translação, Rotação e Escala
void aplicaTransformacoes (	GLuint shaderID, Sprite elemento, vec3 eixoDeRotacao = vec3(0.0, 0.0, 1.0) );
														// 1º parâmetro: identificador do programa de shader
														// 2º parâmetro: elemento (Sprite) que será processado
														// 3º parâmetro: eixo no qual se processará a rotação do elemento. Defauld -> eixo "z"



/*** Constantes	***/
const GLuint WIDTH = 800, HEIGHT = 600;	// Dimensões da janela


/*** Variáveis globais ***/
float FPS = 12.0f;
float lastTime = 0;


/*** Função MAIN ***/
int main() {

	// GLFW: Inicialização e configurações de versão do OpenGL
	glfwInit();	// Inicialização da GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);		// Informa a versão do OpenGL a partir da qual o código funcionará			
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);		// Exemplo para versão 4.6 - Você deve adaptar para a versão do OpenGL suportada por sua placa
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Essencial para computadores da Apple
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ian R. Boniatti e Eduardo Tropea", nullptr, nullptr);	// Criação da janela GLFW
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);	// Registro da função de callback para a janela GLFW
	
	// GLAD: carrega todos os ponteiros de funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { cout << "Failed to initialize GLAD" << std::endl; }

	// Obtendo as informações de versão da placa de vídeo
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); 	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e montando o programa de shader
	GLuint shaderID = setupShader(); // Retorna o identificador OpenGL do programa de shader

	// Informa qual programa de shader usará, no caso -> shaderID
	glUseProgram(shaderID);

	//Criação dos sprites - objetos da cena
	Sprite background, character;
	int imgWidth, imgHeight, texID;

	// Carregando uma textura do personagem e armazenando seu id
	texID = loadTexture("../Textures/Characters/Female 23-1.png",imgWidth,imgHeight);
	character = initializeSprite(texID, vec3(imgWidth*3,imgHeight*3,1.0),vec3(400,100,0),4,3);

	texID = loadTexture("../Textures/Backgrounds/orig.png",imgWidth,imgHeight);
	background = initializeSprite(texID, vec3(imgWidth*1.4,imgHeight*1.6,1.0),vec3(400,300,0));

	// Ativando o primeiro buffer (0) de textura da OpenGL
	glActiveTexture(GL_TEXTURE0);
		
	// Enviar a informação de qual variável armazenará o buffer da textura
	glUniform1i(glGetUniformLocation(shaderID, "texBuff"), 0);

	// Aplica a Matriz de projeção paralela ortográfica (usada para desenhar em 2D)
	mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);  	// ortho(Left, Right, Bottom, Top, Near, Far)
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	//Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	//Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	character.iAnimation = IDLE; // = 1
	

	/*** Loop da aplicação - "game loop" ***/
	while (!glfwWindowShouldClose(window))	{
		
		glfwPollEvents();	// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes

		// Limpa o buffer de cor
		glClearColor(193/255.0f, 229/255.0f, 245/255.0f, 1.0f); // define a cor de fundo - % normalizado, definido de 0.0 a 1.0 -> glClearColor(%RED, %GREEN, %BLUE, %ALPHA);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0, 0.0); // enviando para variável uniform offsetTex

		// Primeiro Sprite = background
		drawSprite(shaderID, background);
		
		//Depois, o personagem e outros itens
		updateSprite(shaderID, character);
		drawSprite(shaderID, character);

		//glBindVertexArray(0);	//Desconectando o buffer de geometria
		
		glfwSwapBuffers(window);	// Troca os buffers da tela
	}
	
	//glDeleteVertexArrays(1, &VAO);	// Pede pra OpenGL desalocar os buffers
	
	glfwTerminate();	// Finaliza a execução da GLFW, limpando os recursos alocados por ela

	return 0;
}


// Função de callback de teclado - só pode ter uma instância (deve ser estática se estiver dentro de uma classe)
// É chamada sempre que uma tecla for pressionada ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE); }

}


// Função responsável pela compilação e montagem do programa de shader
// Por enquanto, neste código, um único e simples programa de shader
// Os códigos fonte do vertex shader e do fragment shader estão nos arrays vertexShaderSource e fragmentShaderSource no iniçio desta função
// A função retorna o identificador do programa de shader (em "main" teremos shaderID = setupShader(), que equivale a shaderID = shaderProgram)
int setupShader() {	/*** Função para gerar o programa de shader ***/

	// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
	const GLchar* vertexShaderSource = R"(
		#version 400							
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec2 texc;		// incluído
		uniform mat4 projection;				// "projection" receberá as informações da forma de projeção escolhida
		uniform mat4 model;						// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)
		out vec2 texCoord;						// incluído
		void main() {
			gl_Position = projection * model * vec4(position, 1.0);
			texCoord = vec2(texc.s,1.0-texc.t);	// incluído
		}
	)";
			// "position" recebe as informações que estão no local 0 -> definidas em glVertexAttribPointer(0, xxxxxxxx);
			// "projection" receberá as informações da forma de projeção escolhida
			// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)			
			// "gl_Position" é uma variável específica do GLSL que recebe a posição final do vertice processado
			// sempre nessa ordem: projection * model * 
			// é vec4 por causa das multiplicações de matrizes, usadas para translação, rotação e escala.

	//Código fonte do Fragment Shader (em GLSL - Graphics Library Shading Language)
	const GLchar* fragmentShaderSource = R"(
		#version 400
		in vec2 texCoord;				// incluído
		uniform sampler2D texBuff;		// incluído
		uniform vec2 offsetTex;			// incluído
		out vec4 color;
		void main() { color = texture(texBuff,texCoord + offsetTex); }	// modificado
	)";
	
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

	return shaderProgram;	// retorna o identificador para o programa de shader
}


//
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


//
Sprite initializeSprite(GLuint texID, vec3 dimensions, vec3 position, int nAnimations, int nFrames, float angle)
{
	Sprite sprite;

	sprite.texID = texID;
	sprite.dimensions.x = dimensions.x / nFrames;
	sprite.dimensions.y = dimensions.y / nAnimations;
	sprite.pos = position;
	sprite.nAnimations = nAnimations;
	sprite.nFrames = nFrames;
	sprite.angle = angle;
	sprite.iFrame = 0;
	sprite.iAnimation = 0;

	sprite.ds = 1.0 / (float) nFrames;
	sprite.dt = 1.0 / (float) nAnimations;

	GLfloat vertices[] = {
		// x    y    z   s    t 
		// T0
		-0.5,  0.5, 0.0, 0.0, sprite.dt,        // v0
		-0.5, -0.5, 0.0, 0.0, 0.0,              // v1
		 0.5,  0.5, 0.0, sprite.ds, sprite.dt, 	// v2
		// T1
		-0.5, -0.5, 0.0, 0.0, 0.0,              // v1
		 0.5,  0.5, 0.0, sprite.ds, sprite.dt, 	// v2
		 0.5, -0.5, 0.0, sprite.ds, 0.0  	    // v3
	};

	GLuint VBO, VAO;
	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);
	// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo)
	
	//Atributo posição - coord x, y, z - 3 valores
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	//Atributo coordenada de textura - coord s, t - 2 valores
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3* sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	sprite.VAO = VAO;

    return sprite;
}


//
void drawSprite(GLuint shaderID, Sprite &sprite)
{
	glBindVertexArray(sprite.VAO); // Conectando ao buffer de geometria
	glBindTexture(GL_TEXTURE_2D, sprite.texID); //conectando com o buffer de textura que será usado no draw

	// Matriz de modelo: transformações na geometria (objeto)
	mat4 model = mat4(1); // matriz identidade
	// Translação
	model = translate(model, sprite.pos);
	// Rotação
	model = rotate(model, radians(sprite.angle), vec3(0.0,0.0,1.0));
	// Escala
	model = scale(model, sprite.dimensions);
	
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0); // Desconectando ao buffer de geometria
	glBindTexture(GL_TEXTURE_2D, 0); // Desconectando com o buffer de textura 

}


//
void updateSprite(GLuint shaderID, Sprite &sprite)
{

	// Incrementando o índice do frame apenas quando fechar a taxa de FPS desejada
	float now = glfwGetTime();
	float dt = now - lastTime;
	if (dt >= 1 / FPS)
	{
		sprite.iFrame = (sprite.iFrame + 1) % sprite.nFrames; //incrementando ciclicamente o indice do Frame
		lastTime = now;
	}
	
	vec2 offsetTex;
	offsetTex.s = sprite.iFrame * sprite.ds;
	offsetTex.t = sprite.iAnimation * sprite.dt;
	glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t); // enviando cor para variável uniform offsetTex
}


// Função responsável pelo carregamento da textura
GLuint loadTexture(string filePath, int &width, int &height) {
	GLuint texID; // id da textura a ser carregada

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Ajuste dos parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carregamento da imagem usando a função stbi_load da biblioteca stb_image
	int nrChannels;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data) {
		if (nrChannels == 3) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); }	// jpg, bmp -> 3 canais
		else { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); }				// png -> 4 canais
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else { std::cout << "Failed to load texture " << filePath << std::endl; }

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}