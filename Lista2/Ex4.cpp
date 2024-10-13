/*** Hello Triangle ***/
/*** Disciplina de Computação Gráfica - Jogos Digitais - Unisinos ***/
/*** Aluno: Ian Rossetti Boniatti ***/
/*** Código adaptado da adaptaçao de Rossana Baptista Queiroz de https://learnopengl.com/#!Getting-started/Hello-Triangle ***/


/*** INCLUDES ***/

#include <iostream>
#include <string>
#include <cmath>
#include <assert.h>

#include <glad/glad.h> 	// biblioteca de funções baseada nas definições/especificações OPENGL
						// Certifique-se de incluir a GLAD antes de outros arquivos de cabeçalho que requerem OpenGL (como GLFW)

#include <GLFW/glfw3.h> // biblioteca de funções para criação da janela no Windows

#include <glm/glm.hpp>	// biblioteca de operações matriciais
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca
using namespace glm;	// Para não precisar digitar glm:: na frente de comandos da biblioteca


enum directions {NONE, UP, DOWN, LEFT, RIGHT};


/*** Protótipos das funções ***/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode); // Protótipo da função de callback de teclado

int setupShader();		// Protótipo da função responsápasso pela compilação e montagem do programa de shader

int setupGeometry();	// Protótipo da função responsápasso pela criação do VBO e do VAO

void aplicaTransformacoes(GLuint shaderID, GLuint VAO, vec3 posicaoNaTela, float anguloDeRotacao, vec3 escala, vec3 color, vec3 eixoDeRotacao = (vec3(0.0, 0.0, 1.0)));


/*** Constantes	***/

const GLuint WIDTH = 800, HEIGHT = 600;	// Dimensões da janela (pode ser alterado em tempo de execução)

const GLchar* vertexShaderSource = "#version 400\n"		// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
"layout (location = 0) in vec3 position;\n"	// "position" recebe as informações que estão no local 0 -> definidas no setupGeometry() -> glVertexAttribPointer(0, xxxxxxxx);
"uniform mat4 projection;\n"				// "projection" receberá as informações da forma de projeção escolhida
"uniform mat4 model;\n"						// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)
"void main()\n"
"{\n"
//...pode ter mais linhas de código para outros atributos, como cor, textura e normalização 
"gl_Position = projection * model * vec4(position, 1.0);\n"	// era: vec4(position.x, position.y, position.z, 1.0);\n	
"}\0";														// "gl_Position" é uma variápasso específica do GLSL que recebe a posição final do vertice processado
		// sempre nessa ordem: projection * model * 		// é vec4 por causa das multiplicações de matrizes, usadas para translação, rotação e escala.

const GLchar* fragmentShaderSource = "#version 400\n"	//Código fonte do Fragment Shader (em GLSL - Graphics Library Shading Language)
"uniform vec4 inputColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = inputColor;\n"
"}\0";


/*** Variáveis Globais	***/

int dir = NONE;


/*** Função MAIN ***/
int main() {

	glfwInit();	// Inicialização da GLFW

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);		// Informa a versão do OpenGL a partir da qual o código funcionará			
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);		// Exemplo para versão 4.6 - Você deve adaptar para a versão do OpenGL suportada por sua placa
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	//Sugestão: comente as 3 linhas de código anteriores para descobrir a versão suportada por sua placa e depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Essencial para computadores da Apple
		
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Olá Triângulo! - Ian R. Boniatti", nullptr, nullptr);	// Criação da janela GLFW
	
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);	// Fazendo o registro da função de callback para a janela GLFW
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {	// GLAD: carrega todos os ponteiros de funções da OpenGL
		cout << "Failed to initialize GLAD" << std::endl;
	}


	// Obtendo as informações de versão da placa de vídeo
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); 	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;


	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint shaderID = setupShader(); 	// Compilando e montando o programa de shader (retorna o identificador OpenGL para o programa de shader)
	
	GLuint VAO = setupGeometry();		// Função para Gerar um buffer VAO simples com a geometria de um triângulo (retorna o identificador OpenGL para o VAO
		
	// Neste código, para enviar a cor desejada para o fragment shader, utilizamos variápasso do tipo uniform (um vec4) já que a informação não estará nos buffers
	glUseProgram(shaderID);
	//GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");	// busca a localização da varíapasso "inputColor" dentro do programa de shader
																	// armazena esta localização em "colorLoc"

	//Matriz de projeção paralela ortográfica
	mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);  	// ortho(Left, Right, Bottom, Top, Near, Far)
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	//Matriz de modelo inicial
	//mat4 model = mat4(1); //salva em model a matriz identidade 4x4
	//glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));


	vec3 posicaoNaTela = vec3(400,300,0);

	float passo = 0.1;

	
	/*** Loop da aplicação - "game loop" ***/
	while (!glfwWindowShouldClose(window))	{
		
		glfwPollEvents();	// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // define a cor de fundo - % normalizado, definido de 0.0 a 1.0 -> glClearColor(%RED, %GREEN, %BLUE, %ALPHA);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO); // Conectando ao buffer de geometria

		switch (dir) {
			case UP:	posicaoNaTela.y += passo;	break;
			case DOWN: 	posicaoNaTela.y -= passo;	break;
			case LEFT: 	posicaoNaTela.x -= passo;	break;
			case RIGHT: posicaoNaTela.x += passo;	break;
			default:								break;
		} 

		aplicaTransformacoes(shaderID, VAO, posicaoNaTela, 0.0, vec3(100,100,1), vec3(1.0,0.0,0.0));
		glDrawArrays(GL_TRIANGLES, 0, 3);


		glBindVertexArray(0);	//Desconectando o buffer de geometria
		
		glfwSwapBuffers(window);	// Troca os buffers da tela
	}
	
	glDeleteVertexArrays(1, &VAO);	// Pede pra OpenGL desalocar os buffers
	
	glfwTerminate();	// Finaliza a execução da GLFW, limpando os recursos alocados por ela

	return 0;
}


// Função de callback de teclado - só pode ter uma instância (deve ser estática se estiver dentro de uma classe)
// É chamada sempre que uma tecla for pressionada ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {glfwSetWindowShouldClose(window, GL_TRUE);}
	
	if 		(key == GLFW_KEY_W || key == GLFW_KEY_UP   ) {dir = UP;   }		// || = OR
	else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN ) {dir = DOWN; }
	else if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT ) {dir = LEFT; }
	else if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) {dir = RIGHT;}
	
	if (action == GLFW_RELEASE)	{ dir = NONE; }	
}


// Função responsápasso pela compilação e montagem do programa de shader
// Por enquanto, neste código, um único e simples programa de shader
// Os códigos fonte do vertex shader e do fragment shader estão nos arrays vertexShaderSource e fragmentShaderSource no iniçio deste arquivo
// A função retorna o identificador do programa de shader (em "main" teremos shaderID = setupShader(), que equivale a shaderID = shaderProgram)
int setupShader() {	/*** Função para gerar o programa de shader ***/

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


// Função responsápasso pela criação do VBO e do VAO - por enquanto, somente um de cada
// O objetivo é criar os buffers que armazenam a geometria de um triângulo: VBO e VAO
// Por enquanto, enviando apenas atributo de coordenadas dos vértices
// Por enquanto, o atributo de cor é enviado externamente por uma variápasso tipo "uniform" chamada "inputColor"
// Por enquanto, 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO (em "main" teremos VAOm = setupShader(), que equivale a VAOm = VAO)
int setupGeometry() {

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
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// Faz a conexão/vinculação do buffer como um buffer de array

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//Envia os dados do array de floats para o buffer da OpenGl


	glGenVertexArrays(1, &VAO);	// Geração do identificador do VAO (Vertex Array Object)

	// Vincula (bind) o VAO primeiro, e em seguida conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertexShaderSource)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)

	return VAO;	// VAO (Vertex Array Object)	// i (interno à função - só para diferenciar do VAOe que está no main)
}


// Função responsápasso pela aplicação das transformações de Translação, Rotação e Escala
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