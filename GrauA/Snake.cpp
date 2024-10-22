/***             SIMULANDO A COBRINHA DO SLITHER.IO               ***/
/*** Disciplina de Computação Gráfica - Jogos Digitais - Unisinos ***/
/***        Alunos: Ian Rossetti Boniatti e Eduardo Tropea        ***/


/*** INCLUDES ***/
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <assert.h>

#include <glad/glad.h> 	// biblioteca de funções baseada nas definições/especificações OPENGL - Incluir antes de outros que requerem OpenGL (como GLFW)

#include <GLFW/glfw3.h> // biblioteca de funções para criação da janela no Windows e gerenciar entrada de teclado/mouse

#include <glm/glm.hpp>	// biblioteca de operações matriciais
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca
using namespace glm;	// Para não precisar digitar glm:: na frente de comandos da biblioteca


// ENUM para definir sentido dado pela entrada de teclado
enum Directions {NONE, UP, DOWN, LEFT, RIGHT};


// Estrutura para armazenar informações sobre um determinado elemento
struct Geometry { 
	GLuint VAO;		// Vertex Array Geometry do elemento
	vec3 posAtual;	// Posição atual do elemento
	vec3 dimensao;	// Escala aplicada ao elemento (largura, altura)
    float angulo;   // Ângulo de rotação aplicado ao elemento	// em radianos
	vec3 cor;       // Cor do elemento	
//    int nVertices;  // Número de vértices a desenhar para este elemento
};


/*** Protótipos das funções ***/
void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode); // Função de callback de teclado
int  setupShader ();		// Função responsável pela compilação e montagem do programa de shader
int  createTriangle ();	// Função responsável pela criação do VBO e do VAO de um TRIÂNGULO
int  createCircle (int verticesExternos, float raio = 0.5); // Função responsável pela criação do VBO e do VAO de um CÍRCULO
Geometry createSegment (int i);	// Cria um segmento e retorna um objeto Geometry com a posição e cor apropriadas

// Função responsápasso pela aplicação das transformações de Translação, Rotação e Escala
void aplicaTransformacoes(	GLuint shaderID,			// 1º parâmetro: identificador do programa de shader
							GLuint VAO,					// 2º parâmetro: identificador do VAO do elemento que será processado
							vec3 posicaoNaTela,			// 3º parâmetro: posição para onde será transladado o elemento
							float anguloDeRotacao,		// 4º parâmetro: rotação a ser aplicada ao elemento de acordo com o eixo de rotação determinado no 6º parâmetro
							vec3 escala,				// 5º parâmetro: dimensão final do elemento
							vec3 color,					// 6º parâmetro: cor do elemento
							vec3 eixoDeRotacao = (vec3(0.0, 0.0, 1.0))); // 7º parâmetro: eixo no qual se processará a rotação estipulada no 4º parâmetro. Defauld eixo "z"
						

/*** Constantes	***/
const float Pi = 3.14159;
const GLuint WIDTH = 800, HEIGHT = 600;	// Dimensões da janela
const vec2 segmentDim = vec2(40,40);
const float passoKbca = 0.01f;	// "velocidade" de incremento na posição da cabeça
const float passoSegm = 0.01f;	// "velocidade" de incremento na posição dos elementos seguintes
const bool controleTeclado = false;	// false -> controle pelo mouse
const GLuint drawingMode = GL_TRIANGLES;
const float minSegDistance = 10.0f, maxSegDistance = 20.0f;

/*** Variáveis Globais	***/
bool keys [1024];

vector <Geometry> snake;	// Vetor que armazena todos os segmentos da cobrinha, incluindo a cabeça
vector <Geometry> Eyes;	// Vetor que armazena os elementos dos olhos da cobrinha

//vec2 mousePos;     // Posição do cursor do mouse
//vec3 dir2Kbca  = vec3(0.0, -1.0, 0.0); // Vetor direção do segmento para a Kbca
//vec3 dir2Mouse = vec3(0.0, -1.0, 0.0); // Vetor direção da Kbca para o mouse


/*** Função MAIN ***/
int main() {

	// GLFW: Inicialização e configurações de versão do OpenGL
	glfwInit();	// Inicialização da GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);		// Informa a versão do OpenGL a partir da qual o código funcionará			
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);		// Exemplo para versão 4.6 - Você deve adaptar para a versão do OpenGL suportada por sua placa
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Essencial para computadores da Apple
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SLITHER.IO - Ian R. Boniatti e Eduardo Tropea", nullptr, nullptr);	// Criação da janela GLFW
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
	GLuint shaderID = setupShader(); // Retorna o identificador OpenGL para o programa de shader
	
	// Ativa o teste de profundidade
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS); // Sempre passará no teste de profundidade (desnecessário se não houver profundidade)

	glUseProgram(shaderID);

	// Aplica a Matriz de projeção paralela ortográfica (usada para desenhar em 2D)
	mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);  	// ortho(Left, Right, Bottom, Top, Near, Far)
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));


	// Criação dos elementos
	snake.push_back(createSegment(0));
	snake.push_back(createSegment(1));



/*
	Geometry kbca;	// Gera o elemento "cabeça"
	kbca.VAO = createTriangle(); //createCircle(32);
	kbca.posAtual = vec3(400,300,0);
	kbca.cor = vec3(1,0,0);
	kbca.dimensao = vec3(segmentDim,0);

	Geometry segmento;
	segmento.VAO = createTriangle(); //createCircle(32);
	segmento.posAtual = vec3(410,310,0);
	segmento.cor = vec3(1,1,0);
	segmento.dimensao = vec3(segmentDim,0);
*/

	
	/*** Loop da aplicação - "game loop" ***/
	while (!glfwWindowShouldClose(window))	{
		
		glfwPollEvents(); // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes

		// Limpa o buffer de cor	// Limpa a tela
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // define a cor de fundo (%RED, %GREEN, %BLUE, %ALPHA);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (controleTeclado) {	
			if (keys[GLFW_KEY_UP   ] || keys[GLFW_KEY_W]) { kbca.posAtual.y += passoKbca; kbca.angulo = radians(  0.0f); }
			if (keys[GLFW_KEY_DOWN ] || keys[GLFW_KEY_S]) { kbca.posAtual.y -= passoKbca; kbca.angulo = radians(180.0f); }
			if (keys[GLFW_KEY_LEFT ] || keys[GLFW_KEY_A]) { kbca.posAtual.x -= passoKbca; kbca.angulo = radians( 90.0f); }
			if (keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D]) { kbca.posAtual.x += passoKbca; kbca.angulo = radians(270.0f); }
		}
		else {
	    	// Obtem a posição do mouse e calcula o vetor direção e o angulo do vetor direção
			// Vetor direção é o vetor que aponta da posição atual do segmento para a posição do mouse)
    	    double xPos, yPos;
        	glfwGetCursorPos(window, &xPos, &yPos);		// Obtem a posição do mouse
        	vec2 mousePos = vec2(xPos, height - yPos);  // Inverte o eixo Y para se alinhar à tela
    
	    	vec3 dir2Mouse = normalize(vec3(mousePos, 0.0) - snake[0].posAtual);	// Calcula o vetor direção normalizado
        	float angle2Mouse = atan2(dir2Mouse.y, dir2Mouse.x);	// calcula o ângulo do vetor "dir2Mouse"

    		// Move o elemento suavemente na direção do mouse ou na direção dada pelas teclas
        	if (distance(snake[0].posAtual, vec3(mousePos, 0.0)) > 0.01f) { snake[0].posAtual += passoKbca * dir2Mouse; } // Aumentar ou diminuir "passoKbca" para controlar a velocidade

       		// Atualiza o ângulo de rotação do elemento
        	snake[0].angulo = angle2Mouse + radians(-90.0f); // Rotaciona para que aponte para o mouse
		}

		vec3 dir2Kbca = normalize(snake[0].posAtual - snake[1].posAtual);
        float angle2Kbca = atan2(dir2Kbca.y, dir2Kbca.x);

		if (distance(snake[1].posAtual, snake[0].posAtual) > minSegDistance) { snake[1].posAtual += passoSegm * dir2Kbca; }  // Aumentar ou diminuir "passoSegm" para controlar a velocidade
        
        // Atualiza o ângulo de rotação do segmento
        snake[1].angulo = angle2Kbca + radians(-90.0f); // Rotaciona para que a ponta aponte para o mouse

		aplicaTransformacoes(shaderID, snake[0].VAO, snake[0].posAtual, snake[0].angulo, snake[0].dimensao, snake[0].cor);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//glBindVertexArray(0);	//Desconectando o buffer de geometria

    	aplicaTransformacoes(shaderID, snake[1].VAO, snake[1].posAtual, snake[1].angulo, snake[1].dimensao, snake[1].cor);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);	//Desconectando o buffer de geometria
		
		glfwSwapBuffers(window);	// Troca os buffers da tela
	}
	
	glDeleteVertexArrays(1, &snake[1].VAO);	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &snake[0].VAO);
	
	glfwTerminate();	// Finaliza a execução da GLFW, limpando os recursos alocados por ela

	return 0;
}


// Função de callback de teclado - só pode ter uma instância (deve ser estática se estiver dentro de uma classe)
// É chamada sempre que uma tecla for pressionada ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {glfwSetWindowShouldClose(window, GL_TRUE);}
	
	if (action == GLFW_PRESS  )	{ keys[key] = true;  }	// seta com true a posição correspondente à tecla pressionada
	if (action == GLFW_RELEASE)	{ keys[key] = false; }	// seta com false a posição correspondente à tecla solta
	
}


// Função responsável pela compilação e montagem do programa de shader
// Por enquanto, neste código, um único e simples programa de shader
// Os códigos fonte do vertex shader e do fragment shader estão nos arrays vertexShaderSource e fragmentShaderSource no iniçio deste arquivo
// A função retorna o identificador do programa de shader (em "main" teremos shaderID = setupShader(), que equivale a shaderID = shaderProgram)
int setupShader() {	/*** Função para gerar o programa de shader ***/

	// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
	const GLchar* vertexShaderSource = R"(
		#version 400							
		layout (location = 0) in vec3 position;	
		uniform mat4 projection;				// "projection" receberá as informações da forma de projeção escolhida
		uniform mat4 model;						// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)
		void main() { gl_Position = projection * model * vec4(position, 1.0); }
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
		uniform vec4 inputColor;
		out vec4 color;
		void main() { color = inputColor; }
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


// Função responsável pela criação do VBO e do VAO dos Triângulos
// O objetivo é criar os buffers que armazenam a geometria de um triângulo: VBO e VAO
// Por enquanto, enviando apenas atributo de coordenadas dos vértices
// Por enquanto, o atributo de cor é enviado externamente por uma variável tipo "uniform" chamada "inputColor"
// Por enquanto, 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO (em "main" teremos VAOm = setupShader(), que equivale a VAOm = VAO)
int createTriangle() {

	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc) pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
	//      x     y    z
	//Triângulo
		-0.5, -0.5, 0.0, // v0 (Vértice 0 do Triângulo)	// deslocamento = 0
		 0.5, -0.5, 0.0, // v1 (Vértice 1 do Triângulo)
 		 0.0,  0.5, 0.0, // v2 (Vértice 2 do Triângulo)

	//Quadrado
		-0.5, -0.5, 0.0, // v0 (Vértice 0 do Quadrado)	// deslocamento = 3
		 0.5, -0.5, 0.0, // v1 (Vértice 1 do Quadrado)
 		 0.5,  0.5, 0.0, // v2 (Vértice 2 do Quadrado)
		-0.5,  0.5, 0.0, // v3 (Vértice 3 do Quadrado)

	// T2 ....			  
	};

	GLuint VBO, VAO;
	
	glGenBuffers(1, &VBO);	// Geração do identificador do VBO (Vertex Buffer Objects)
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// Faz a conexão/vinculação (bind) do VBO como um buffer de array

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//Envia os dados do array de floats "vertices" para o buffer da OpenGl

	glGenVertexArrays(1, &VAO);	// Geração do identificador do VAO (Vertex Array Object)

	glBindVertexArray(VAO); // Faz a conexão/vinculação (bind) do VAO e em seguida conecta e seta o(s) buffer(s) de vértices (VBOs)

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);	// seta/atribui o ponteiro para o atributo

		// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
			// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertexShaderSource)
			// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
			// Tipo do dado
			// Se está normalizado (entre zero e um)
			// Tamanho em bytes 
			// Deslocamento a partir do byte zero 
	
	glEnableVertexAttribArray(0);	// habilita para uso o atributo

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// desvincula o VBO

	glBindVertexArray(0); // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)

	return VAO;	// VAO (Vertex Array Object)
}


// Função responsável pela criação do VBO e do VAO dos Circulos 
int createCircle(int verticesExternos, float raio) {
	
	vector <GLfloat> vertices;

	float anguloAtual = 0.0;
	float intervalo = 2 * Pi / (float)(verticesExternos);
	
	// adicionando o centro do circulo no vetor "vertices - origem (0.0,0.0,0.0)"
	vertices.push_back(0.0);	// Xc
	vertices.push_back(0.0);	// Yc
	vertices.push_back(0.0);	// Zc

	for (int i = 1; i <= verticesExternos+1; i++)	{		
		
		vertices.push_back(raio * cos(anguloAtual));	// Xi
		vertices.push_back(raio * sin(anguloAtual)); 	// Yi
		vertices.push_back(0.0);						// Zi

		anguloAtual = anguloAtual + intervalo;
	}

	
	// Configuração dos buffers VBO e VAO
	GLuint VBO, VAO;
	
	glGenBuffers(1, &VBO);	// Geração do identificador do VBO (Vertex Buffer Objects)
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// Faz a conexão/vinculação (bind) do VBO como um buffer de array
	
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);	//Envia os dados do array de floats "vertices" para o buffer da OpenGl

	glGenVertexArrays(1, &VAO);	// Geração do identificador do VAO (Vertex Array Object)

	glBindVertexArray(VAO);	// Faz a conexão/vinculação (bind) do VAO e em seguida conecta e seta o(s) buffer(s) de vértices (VBOs)

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);	// seta/atribui o ponteiro para o atributo

	// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
			// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertexShaderSource)
			// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
			// Tipo do dado
			// Se está normalizado (entre zero e um)
			// Tamanho em bytes 
			// Deslocamento a partir do byte zero 
	
	glEnableVertexAttribArray(0);	// habilita o atributo para uso - no caso atributo com localização "0"

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// desvincula o VBO

	glBindVertexArray(0); // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs)

	return VAO;	// VAO (Vertex Array Object)
}


// Cria um segmento e retorna um objeto Geometry com a posição e cor apropriadas
// i: Índice do segmento (0 para a cabeça, >= 1 para os segmentos do corpo)
// dir: Vetor direção indicando a direção inicial do segmento
Geometry createSegment(int i) {

	cout << "Criando segmento " << i << endl;
	
	Geometry segment;	// Inicializa um objeto Geometry para armazenar as informações do segmento
	
	segment.VAO = createCircle(32); // Cria a geometria do segmento como um círculo
	segment.VAO = createTriangle();
	//segment.nVertices = 34; // Número de vértices do círculo

	// Define a posição inicial de cada segmento
	if (i == 0) { segment.posAtual = vec3(400.0, 300.0, 0.0); } // Cabeça -> Posição inicial no centro da tela

	else {	// Demais segmentos													
		vec3 dir;
		if (i >= 2)	{ dir = normalize(cobrinha[i - 1].posAtual - cobrinha[i - 2].posAtual); } // Ajusta a direção com base na posição dos segmentos anteriores para evitar sobreposição
		segment.posAtual = cobrinha[i - 1].posAtual + minSegDistance * dir; // Posiciona o novo segmento com uma distância mínima do segmento anterior
	}
	
	segment.dimensao = vec3(segmentDim, 1.0);	// Define as dimensões do segmento (tamanho do círculo)

	segment.angulo = 0.0; // Ângulo inicial (sem rotação)

	// Alterna a cor do segmento entre azul e amarelo, dependendo do índice
	if (i % 2 == 0) { segment.cor = vec3(0, 0, 1); } // Azul para segmentos de índice par
	else { segment.cor = vec3(1, 1, 0); } // Amarelo para segmentos de índice ímpar
	
	return segment;
}


// Função responsável pela aplicação das transformações de Translação, Rotação e Escala
void aplicaTransformacoes(GLuint shaderID, GLuint VAO, vec3 posicaoNaTela, float anguloDeRotacao, vec3 escala, vec3 color, vec3 eixoDeRotacao) {
	
	/*** Transformações na geometria (objeto) -> sempre na ordem Translação - Rotação - Escala ***/

	glBindVertexArray(VAO); // Vincula o VAO
	
	mat4 model = mat4(1); // salva em model a matriz identidade 4x4 (Matriz de modelo inicial)
	model = translate(model,posicaoNaTela);
	model = rotate(model,anguloDeRotacao,eixoDeRotacao);
	model = scale(model,escala);
	
	// Transformações sendo enviadas para "model"
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	// Cores sendo enviadas para "inputColor"
	glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b , 1.0f);
}