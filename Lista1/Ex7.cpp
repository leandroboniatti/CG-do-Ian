/*** Hello Triangle ***/
/*** Disciplina de Computação Gráfica - Jogos Digitais - Unisinos ***/
/*** Aluno: Ian Rossetti Boniatti ***/
/*** Código adaptado da adaptaçao de Rossana Baptista Queiroz de https://learnopengl.com/#!Getting-started/Hello-Triangle ***/


/*** INCLUDES ***/

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <assert.h> // COM .h é biblioteca do C - SEM o .h é biblioteca do C++

#include <glad/glad.h> 	// biblioteca de funções baseada nas definições/especificações OPENGL
						// Certifique-se de incluir a GLAD antes de outros arquivos de cabeçalho que requerem OpenGL (como GLFW)

#include <GLFW/glfw3.h> // biblioteca de funções para criação da janela no Windows

using namespace std;	// Para não precisar digitar std:: na frente de comandos como cout e cin					


/*** Constantes	***/

const float Pi = 3.14159;

const GLuint WIDTH = 400, HEIGHT = 400;	// Dimensões da janela (pode ser alterado em tempo de execução)

const GLchar* vertexShaderSource = "#version 400\n"		// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
"layout (location = 0) in vec3 position;\n"	// "position" recebe as informações que estão no local 0 -> definidas no setupGeometry() -> glVertexAttribPointer(0, xxxxxxxx);
"void main()\n"
"{\n"
//...pode ter mais linhas de código para outros atributos, como cor, textura e normalização 
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"	// poderia ser: "gl_Position = vec4(position, 1.0);\n"	
"}\0";																// "gl_Position" é uma variável específica do GLSL que recebe a posição final do vertice processado
																	// é vec4 por causa das multiplicações de matrizes, usadas para translação, rotação e escala.

const GLchar* fragmentShaderSource = "#version 400\n"	//Código fonte do Fragment Shader (em GLSL - Graphics Library Shading Language)
"uniform vec4 inputColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = inputColor;\n"
"}\0";


/*** Protótipos das funções ***/

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode); // Protótipo da função de callback de teclado

int setupShader();		// Protótipo da função responsável pela compilação e montagem do programa de shader

int setupGeometry();	// Protótipo da função responsável pela criação do VBO e do VAO

int createPoligno(int verticesExternos, float anguloInicial, float anguloFinal, float raio = 0.5);

int createEstrela(int numeroPontos, float raioMinimo, float raioMaximo);

int createEspiral(int numeroPontos, float anguloInicial, float anguloFinal, float raioMinimo, float raioMaximo);


/*** Função MAIN ***/
int main() {

	// 	Parâmetros para exerc. 6 a,b,c,d	//	Círculo	 |	Octagno	 |	Pentagno  |  PacMan  |  FatiaPizz  | Estrela
	int verticesExternos =    100;	 		//     60   		8			 5		    60		    60		    10
	float anguloInicial  =     0; 		    // 	    0		    0		     0		    30		   330
	float anguloFinal	 =  1440; 	  		// 	  360		   360		    360		   330		    30
	int deslocaContorno  =	   0;			//		1			1			 1			 0			 0 					// Deslocamento a partir do byte zero para o traço

	bool desenhaInterior = 0;	// 0 = false	1 = true
	bool desenhaContorno = 1;
	bool desenhaPontos   = 0;

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


	/*** Compilando e montando o programa de shader (retorna o identificador OpenGL para o programa de shader) ***/
	GLuint shaderID = setupShader(); 	
	
	
	// Aqui estavam os Parâmetros para exerc. 6 a,b,c,d


	/*** Gerando o buffer VAO (retorna o identificador OpenGL para o VAO) ***/ 

	GLuint VAO;
	
	//VAO = setupGeometry();	// HelloTriangle

	//VAO = createPoligno(verticesExternos, anguloInicial, anguloFinal);	// exerc. 6 a,b,c,d

	//VAO = createEstrela(verticesExternos, 0.10, 0.90);	// exerc. 6 e

	VAO = createEspiral(verticesExternos, anguloInicial, anguloFinal, 0.10, 0.90);	// exerc. 7
	
	// Neste código, para enviar a cor desejada para o fragment shader, utilizamos variável do tipo uniform (um vec4) já que a informação não estará nos buffers
	glUseProgram(shaderID);
	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");	// busca a localização da varíavel "inputColor" dentro do programa de shader
																	// armazena esta localização em "colorLoc"
	
	/*** Loop da aplicação - "game loop" ***/
	while (!glfwWindowShouldClose(window))	{
		
		glfwPollEvents();	// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // define a cor de fundo - % normalizado, definido de 0.0 a 1.0 -> glClearColor(%RED, %GREEN, %BLUE, %ALPHA);
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);	// o espessura padrão da linha é 1 pixel - alterado para....
		glPointSize(10);	// o tamanho padrão do ponto é 1 pixel - alterado para....

		glBindVertexArray(VAO); // Conectando ao buffer de geometria

		// Desenho preenchido (interior)
		if (desenhaInterior) {
			glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f); //enviando cor do objeto para variável uniform chamada "inputColor" -> glUniform4f(%RED, %GREEN, %BLUE, %ALPHA);
			glDrawArrays(GL_TRIANGLE_FAN, 0, verticesExternos + 2); 	// Chamada de desenho - drawcall
		}

		//Desenho com contorno (linhas)
		if (desenhaContorno) {
			glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f); //enviando NOVA cor para variável uniform inputColor
			glDrawArrays(GL_LINE_STRIP, deslocaContorno, verticesExternos + 1);
		}

		//Desenho só dos pontos (vértices + centro)
		if (desenhaPontos) {
			glUniform4f(colorLoc, 1.0f, 1.0f, 0.0f, 1.0f); //enviando NOVA cor para variável uniform inputColor
			glDrawArrays(GL_POINTS, 0, verticesExternos + 1);
		}
										
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
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


// Função responsável pela compilação e montagem do programa de shader
// Por enquanto, neste código, um único e simples programa de shader
// Os códigos fonte do vertex shader e do fragment shader estão nos arrays vertexShaderSource e fragmentShaderSource no iniçio deste arquivo
// A função retorna o identificador do programa de shader (em "main" teremos shaderID = setupShader(), que equivale a shaderID = shaderProgram)
int setupShader() {	/*** Função para gerar o ***/

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


// Função responsável pela criação do VBO e do VAO - por enquanto, somente um de cada
// O objetivo é criar os buffers que armazenam a geometria de um triângulo: VBO e VAO
// Por enquanto, enviando apenas atributo de coordenadas dos vértices
// Por enquanto, o atributo de cor é enviado externamente por uma variável tipo "uniform" chamada "inputColor"
// Por enquanto, 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO (em "main" teremos VAOm = setupShader(), que equivale a VAOm = VAO)
int setupGeometry() {

	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc) pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
	//      x     y    z
	//T0
		-0.5, -0.5, 0.0, // v0 (Vértice 0 do Triângulo 0)
		 0.5, -0.5, 0.0, // v1 (Vértice 1 do Triângulo 0)
 		 0.0,  0.5, 0.0, // v2 (Vértice 2 do Triângulo 0)

	//T1
		-0.5,  0.5, 0.0, // v0 (Vértice 0 do Triângulo 1)
		-0.2,  0.5, 0.0, // v1 (Vértice 1 do Triângulo 1)
 		-0.5,  0.0, 0.0, // v2 (Vértice 2 do Triângulo 1)

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
	// 1) Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertexShaderSource)
	// 2) Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// 3) Tipo do dado
	// 4) Se está normalizado (entre zero e um)
	// 5) Tamanho em bytes 
	// 6) Deslocamento a partir do byte zero 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)

	return VAO;	// VAO (Vertex Array Object)	// i (interno à função - só para diferenciar do VAOe que está no main)
}


int createPoligno(int verticesExternos, float anguloInicial, float anguloFinal, float raio) {
	
	vector <GLfloat> vertices;

	float intervalo;
	float anguloAtual = anguloInicial;
	float anguloAbertura = anguloFinal - anguloInicial;	

	if (anguloAbertura == 360.0) {
		intervalo = 360 / (float)(verticesExternos); 
		anguloAtual = 0;
		verticesExternos++;	// ??? precisamos +1 vertice externo = primeiro para fechar o poligno
	}
	else {
		if (anguloAbertura < 0.0) {
			intervalo = (anguloAbertura + 360) / (float)(verticesExternos-1);
		}
		else {		
			intervalo = anguloAbertura / (float)(verticesExternos-1);
		}
	}

	intervalo = intervalo * 2 * Pi / 360;
	anguloAtual = anguloAtual * 2 * Pi / 360;

	// adicionando o centro do circulo no vetor
	vertices.push_back(0.0);	// Xc
	vertices.push_back(0.0);	// Yc
	vertices.push_back(0.0);	// Zc

	for (int i = 1; i <= verticesExternos; i++)	{		
		
		vertices.push_back(raio * cos(anguloAtual));	// Xi
		vertices.push_back(raio * sin(anguloAtual)); 	// Yi
		vertices.push_back(0.0);						// Zi

		anguloAtual = anguloAtual + intervalo;
	}

	/*** Configuração dos buffers VBO e VAO ***/
	GLuint VBO, VAO;
	
	glGenBuffers(1, &VBO);	// Geração do identificador do VBO (Vertex Buffer Objects)
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// Faz a conexão/vinculação do buffer como um buffer de array

	
	
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);	//Envia os dados do array de floats para o buffer da OpenGl

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


int createEstrela(int numeroPontos, float raioMinimo, float raioMaximo) {

	vector <GLfloat> vertices;

	float angulo = 0.0;

	float intervalo = 2 * Pi / (float)numeroPontos;

	float raio;

	//Adicionando o ponto da origem (0.0,0.0,0.0) como sendo o centro do círculo
	vertices.push_back(0.0); // Xc
	vertices.push_back(0.0); // Yc
	vertices.push_back(0.0); // Zc

	for (int i = 1; i < numeroPontos+1; i++)
	{
		if (i % 2 == 0) raio = raioMaximo;
		else raio = raioMinimo;

		float x = raio * cos(angulo);
		float y = raio * sin(angulo);
		float z = 0.0;

		vertices.push_back(x); // x
		vertices.push_back(y); // y
		vertices.push_back(z); // z

		angulo = angulo + intervalo;
	}

	//Configuração dos buffer VBO e VAO
	GLuint VBO, VAO;
	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
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

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0); 

	return VAO;
}


int createEspiral(int numeroPontos, float anguloInicial, float anguloFinal, float raioMinimo, float raioMaximo) {

	vector <GLfloat> vertices;

	float angulo = 0.0;

	float intervaloAngular = ((anguloFinal - anguloInicial) / (float)(numeroPontos-1)) * 2 * Pi / 360;

	float intervaloRadial = (raioMaximo - raioMinimo) / (float)numeroPontos;

	float raio = raioMinimo;

	//Adicionando o ponto da origem (0.0,0.0,0.0) como sendo o centro do círculo
	vertices.push_back(0.0); // Xc
	vertices.push_back(0.0); // Yc
	vertices.push_back(0.0); // Zc

	for (int i = 0; i < numeroPontos; i++)
	{
		float x = raio * cos(angulo);
		float y = raio * sin(angulo);
		float z = 0.0;

		vertices.push_back(x); // x
		vertices.push_back(y); // y
		vertices.push_back(z); // z

		angulo = angulo + intervaloAngular;
		raio = raio + intervaloRadial;
	}

	//Configuração dos buffer VBO e VAO
	GLuint VBO, VAO;
	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
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

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0); 

	return VAO;
}
