/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 13/08/2024
  */

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <assert.h>
#include <stb_image.h>

#include <glad/glad.h>	// GLAD

#include <GLFW/glfw3.h> // GLFW

#include <glm/glm.hpp> // GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;




enum sprites_states {IDLE = 1,MOVING_LEFT,MOVING_RIGHT};

// Estrutura de dados das Sprites
struct Sprite
{

	GLfloat VAO;   // id do buffer de geometria
	GLfloat texID; // id da textura
	vec3 pos;
	vec3 dimensions;
	float angle;
	
	// Para a animação da spritesheet
	int nAnimations, nFrames;
	int iAnimation, iFrame;
	float ds, dt;



	// Para a movimentação do sprite
	float vel;

	// Para o cálculo da colisão (AABB - Axis Aligned Bounding Box)
	vec2 PMax, PMin;
};


// Protótipos (ou Cabeçalhos) das funções
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode); // Protótipo da função de callback de teclado

int setupShader();
GLuint loadTexture(string filePath, int &width, int &height);

void drawSprite(GLuint shaderID, Sprite &sprite);
void updateSprite(GLuint shaderID, Sprite &sprite);
void moveSprite(GLuint shaderID, Sprite &sprite);

void updateItems(GLuint shader, Sprite &sprite);
void spawnItem(Sprite &sprite);

void calculateAABB(Sprite &sprite);
bool checkCollision(Sprite one, Sprite two);

Sprite initializeSprite(GLuint texID, vec3 dimensions, vec3 position, int nAnimations = 1, int nFrames = 1, float vel = 0.2, float angle = 0.0);








// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;


// Variáveis globais
float FPS = 12.0f;
float lastTime = 0;
bool keys[1024];
GLuint itemsTexIDs[3];
int lives = 5;
float velItems = 0.01f;
float lastSpawnX = 400.0;



// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texc;
uniform mat4 projection;
uniform mat4 model;
out vec2 texCoord;
void main()
{
   	gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);
	texCoord = vec2(texc.s,1.0-texc.t);
})";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"(
#version 400
in vec2 texCoord;
uniform sampler2D texBuff;
uniform vec2 offsetTex;
out vec4 color;
void main()
{
	color = texture(texBuff, texCoord + offsetTex);
})";

// Função MAIN
int main()
{

	srand(time(0)); // Pega o horário do sistema como semente para a geração de nros aleatórios

	// Inicialização da GLFW
	glfwInit();

	// Muita atenção aqui: alguns ambientes não aceitam essas configurações
	// Você deve adaptar para a versão do OpenGL suportada por sua placa
	// Sugestão: comente essas linhas de código para desobrir a versão e
	// depois atualize (por exemplo: 4.5 com 4 e 5)
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	// Essencial para computadores da Apple
	// #ifdef __APPLE__
	//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// #endif

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Sprites!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte *version = glGetString(GL_VERSION);	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	//--------------------------------
	// Inicializando o array de controle das teclas
	for (int i = 0; i < 1024; i++)
	{
		keys[i] = false;
	}

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Criação dos sprites - objetos da cena
	Sprite background, character;
	vector<Sprite> items;
	int score = 0;
	bool gameover = false;
	int imgWidth, imgHeight, texID;

	// Carregando uma textura do personagem e armazenando seu id
	texID = loadTexture("../Textures/Characters/Female 23-1.png", imgWidth, imgHeight);
	character = initializeSprite(texID, vec3(imgWidth * 3, imgHeight * 3, 1.0), vec3(400, 100, 0), 4, 3);

	texID = loadTexture("../Textures/Backgrounds/Preview 3.png", imgWidth, imgHeight);
	background = initializeSprite(texID, vec3(imgWidth * 0.4, imgHeight * 0.4, 1.0), vec3(400, 300, 0));

	itemsTexIDs[0] = loadTexture("../Textures/Items/Icon30.png", imgWidth, imgHeight);
	itemsTexIDs[1] = loadTexture("../Textures/Items/Icon26.png", imgWidth, imgHeight);
	itemsTexIDs[2] = loadTexture("../Textures/Items/Icon42.png", imgWidth, imgHeight);

	for (int i = 0; i < 3; i++)
	{
		items.push_back(initializeSprite(0, vec3(imgWidth * 1.5, imgHeight * 1.5, 1.0), vec3(0, 0, 0)));
		spawnItem(items[items.size()-1]);
	}

	glUseProgram(shaderID);

	// Ativando o primeiro buffer de textura da OpenGL
	glActiveTexture(GL_TEXTURE0);

	// Enviar a informação de qual variável armazenará o buffer da textura
	glUniform1i(glGetUniformLocation(shaderID, "texBuff"), 0);

	// Matriz de projeção paralela ortográfica
	// mat4 projection = ortho(-10.0, 10.0, -10.0, 10.0, -1.0, 1.0);
	mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	// Matriz de modelo: transformações na geometria (objeto)
	mat4 model = mat4(1); // matriz identidade
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	// Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	// Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	character.iAnimation = IDLE; // Explicar isso semana que vem

	cout << "Pontuacao = " << score << endl;
	cout << "Nro de vidas = " << lives << endl;

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window) && !gameover)
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(193 / 255.0f, 229 / 255.0f, 245 / 255.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0, 0.0); // enviando para variável uniform offsetTex
		
		//Checagem das colisões
		calculateAABB(character); // Calcula (atualiza) o PMin e o PMax usados para testar a colisão
		for (int i=0; i<items.size(); i++)
		{
			calculateAABB(items[i]);
			if (checkCollision(character,items[i]))
			{
				spawnItem(items[i]);
				score++;
				cout << "Pontuacao = " << score << endl;
				velItems += 0.1;
			}
		}
		
		
		// Primeiro Sprite
		drawSprite(shaderID, background);

		// Depois, o personagem e outros itens
		moveSprite(shaderID, character);
		updateSprite(shaderID, character);
		drawSprite(shaderID, character);

		// Itens
		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0, 0.0);
		for (int i = 0; i < items.size(); i++)
		{
			drawSprite(shaderID, items[i]);
			updateItems(shaderID, items[i]);
		}

		if (lives <= 0)
		{
			gameover = true;
			cout << "GAME OVER!" << endl;
		}

		glBindVertexArray(0); // Desconectando o buffer de geometria

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	// glDeleteVertexArrays(1, character.VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	if (!glfwWindowShouldClose(window)) glfwSetWindowShouldClose(window, GL_TRUE);
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}
}

// Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
//  shader simples e único neste exemplo de código
//  O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
//  fragmentShader source no iniçio deste arquivo
//  A função retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}



Sprite initializeSprite(GLuint texID, vec3 dimensions, vec3 position, int nAnimations, int nFrames, float vel, float angle)
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
	sprite.vel = vel;

	sprite.ds = 1.0 / (float)nFrames;
	sprite.dt = 1.0 / (float)nAnimations;

	GLfloat vertices[] = {
		// x    y    z   s    t
		// T0
		-0.5, 0.5, 0.0, 0.0, sprite.dt,		 // v0
		-0.5, -0.5, 0.0, 0.0, 0.0,			 // v1
		0.5, 0.5, 0.0, sprite.ds, sprite.dt, // v2
		// T1
		-0.5, -0.5, 0.0, 0.0, 0.0,			 // v1
		0.5, 0.5, 0.0, sprite.ds, sprite.dt, // v2
		0.5, -0.5, 0.0, sprite.ds, 0.0		 // v3
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

	// Atributo posição - coord x, y, z - 3 valores
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// Atributo coordenada de textura - coord s, t - 2 valores
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	sprite.VAO = VAO;

	return sprite;
}

void drawSprite(GLuint shaderID, Sprite &sprite)
{
	glBindVertexArray(sprite.VAO);				// Conectando ao buffer de geometria
	glBindTexture(GL_TEXTURE_2D, sprite.texID); // conectando com o buffer de textura que será usado no draw

	// Matriz de modelo: transformações na geometria (objeto)
	mat4 model = mat4(1); // matriz identidade
	// Translação
	model = translate(model, sprite.pos);
	// Rotação
	model = rotate(model, radians(sprite.angle), vec3(0.0, 0.0, 1.0));
	// Escala
	model = scale(model, sprite.dimensions);

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);			 // Desconectando ao buffer de geometria
	glBindTexture(GL_TEXTURE_2D, 0); // Desconectando com o buffer de textura
}

void updateSprite(GLuint shaderID, Sprite &sprite)
{

	// Incrementando o índice do frame apenas quando fechar a taxa de FPS desejada
	float now = glfwGetTime();
	float dt = now - lastTime;
	if (dt >= 1 / FPS)
	{
		sprite.iFrame = (sprite.iFrame + 1) % sprite.nFrames; // incrementando ciclicamente o indice do Frame
		lastTime = now;
	}

	vec2 offsetTex;
	offsetTex.s = sprite.iFrame * sprite.ds;
	offsetTex.t = sprite.iAnimation * sprite.dt;
	glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t); // enviando cor para variável uniform offsetTex
}

GLuint loadTexture(string filePath, int &width, int &height)
{
	GLuint texID; // id da textura a ser carregada

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Ajuste dos parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Carregamento da imagem usando a função stbi_load da biblioteca stb_image
	int nrChannels;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) // jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else // assume que é 4 canais png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture " << filePath << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}


void moveSprite(GLuint shaderID, Sprite &sprite)
{
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		sprite.pos.x -= sprite.vel;
		sprite.iAnimation = MOVING_LEFT;
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		sprite.pos.x += sprite.vel;
		sprite.iAnimation = MOVING_RIGHT;
	}

	if (!keys[GLFW_KEY_A] && !keys[GLFW_KEY_D] && !keys[GLFW_KEY_LEFT] && !keys[GLFW_KEY_RIGHT])
	{
		sprite.iAnimation = IDLE;
	}
}

void spawnItem(Sprite &sprite)
{
	int max = lastSpawnX + 250;
	if (max > 790) max = 790;
	int min = lastSpawnX - 250;
	if (min < 10) min = 10;
	
	sprite.pos.x = rand() % (max - min + 1) + min;	// valor entre 10 e 790
	lastSpawnX = sprite.pos.x;
	sprite.pos.y = rand() % (3000 - 650 + 1) + 650; // valor entre 650 e 850
	sprite.texID = itemsTexIDs[rand() % 3];
	sprite.vel = velItems;
	int n = rand() % 3;
	if (n == 1)
	{
		sprite.vel = sprite.vel + sprite.vel * 0.1;
	}
	else if (n == 2)
	{
		sprite.vel = sprite.vel - sprite.vel * 0.1;
	}
}

void updateItems(GLuint shader, Sprite &sprite)
{
	if (sprite.pos.y > 100)
	{
		sprite.pos.y -= sprite.vel;
	}
	else
	{
		lives--;
		cout << "Oh no! Vidas: " << lives << endl;
		spawnItem(sprite);
	}
}

void calculateAABB(Sprite &sprite)
{
	sprite.PMin.x = sprite.pos.x - sprite.dimensions.x/2.0; 
	sprite.PMin.y = sprite.pos.y - sprite.dimensions.y/2.0; 

	sprite.PMax.x = sprite.pos.x + sprite.dimensions.x/2.0; 
	sprite.PMax.y = sprite.pos.y + sprite.dimensions.y/2.0; 
}

bool checkCollision(Sprite one, Sprite two)
{
	// collision x-axis?
    bool collisionX = one.PMax.x >= two.PMin.x &&
        two.PMax.x >= one.PMin.x;
    // collision y-axis?
    bool collisionY = one.PMax.y>= two.PMin.y &&
        two.PMax.y >= one.PMin.y;
    // collision only if on both axes
    return collisionX && collisionY;  
}
