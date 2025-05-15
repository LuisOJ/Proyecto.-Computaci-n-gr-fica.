#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

//// SKYBOX
#include "Texture.h"


//--------------------------------------------------------------------------
//-----------------------------Movimientos de sillas------------------------
// Número de filas de sillas y cuántas sillas hay por fila
// ======================= Configuración de sillas ============================
const int filasDeSillas = 14;
const int sillasPorFila = 4;
const int numSillas = filasDeSillas * sillasPorFila;

// Variables para animación
glm::vec3 sillaPosArr[numSillas];            // Posiciones actuales
float sillaRotArr[numSillas] = { 0.0f };      // Rotaciones actuales
float rotSillaArr[numSillas] = { 0.0f };      // Auxiliar de rotación
int sillaAnimArr[numSillas] = { 0 };          // Estado de animación
bool sillaActiva = false;

// Parámetros para personalizar trayectorias
glm::vec3 posicionesIniciales[numSillas];     // Desde la puerta
glm::vec3 destinoIntermedio[numSillas];       // Entrada al pasillo
float rotacionObjetivo[numSillas];            // Rotación hacia pasillo
glm::vec3 posicionesFinales[numSillas];           // Frente al escritorio
float rotacionFinalAjuste[numSillas];         // Rotación final
glm::vec3 destino1[numSillas];  // Primero se mueven al frente
glm::vec3 destino2[numSillas];  // Luego giran y se mueven lateral
glm::vec3 destinoFinalAvance[numSillas];  // Agrega esta nueva variable global
float rotacionPostFinal[numSillas]; // (fuera del loop, como variable global)






////
// ANIMACIÓN NUEVOS MODELOS
bool animateRise = false;
float riseDistance = 5.0f;  // Distancia inicial bajo la lava
float riseSpeed = 1.5f;     // Velocidad de ascenso
float rotationRiseAngle = 0.0f;
float rotationRiseSpeed = 180.0f; // Velocidad de rotación durante el ascenso

////

bool show = false; // Variable global para mostrar salones
bool shownew = false; // Variable global para mostrar salones
bool showlava = false; // La lava está visible inicialmente
bool showsilla = false;
bool animacionActiva = false;
int faseAnimacion = 0; // 0:inactiva, 1:bajar escritorios, 2:subir nuevos modelos
float animationSpeed = 2.0f;


bool animateFall = false;
float fallDistance = 0.0f;
float rotationAngle = 0.0f;
float fallSpeed = 2.0f;       // Velocidad de caída
float rotationSpeed = 90.0f;  // Velocidad de rotación (grados/segundo)
bool animationPaused = true;  // Controla si la animación está en pausa
// (mantén las variables anteriores de animación)

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Animación del dron
glm::vec3 dronPos(-15.0f, 9.0f, -24.0f); // Posición inicial del dron (igual al primer waypoint)

glm::vec3 waypoints[4] = {
	glm::vec3(-15.0f, 9.0f, -24.0f),  // Esquina inferior-izquierda (A)
	glm::vec3(15.0f, 9.0f, -20.0f),   // Esquina inferior-derecha (B)
	glm::vec3(15.0f, 9.0f, 20.0f),    // Esquina superior-derecha (C) 
	glm::vec3(-15.0f, 9.0f, 20.0f)    // Esquina superior-izquierda (D) 
};



int currentWaypoint = 0;
float speed = 5.0f; // Velocidad de movimiento
bool moveDron = false; // Control de movimiento
// 
bool sinusoidalMode = false;        // Modo sinusoidal activo/desactivado
float sineAmplitude = 3.0f;        // Altura de la onda
float sineFrequency = 2.0f;        // Velocidad de oscilación
float sineTime = 0.0f;             // Tiempo acumulado para el cálculo de la onda
glm::vec3 initialSinePos;          // Posición inicial al activar el modo sinusoidal
glm::vec3 segmentStart;  //  
glm::vec3 segmentEnd;    //  
glm::vec3 segmentDir;    //  
float segmentLength = 0.0f;  //  
float segmentProgress = 0.0f;  //

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,2.0f, 0.0f),
	glm::vec3(0.0f,0.0f, 0.0f),
	glm::vec3(0.0f,0.0f,  0.0f),
	glm::vec3(0.0f,0.0f, 0.0f)
};

float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};



glm::vec3 Light1 = glm::vec3(0);
//Anim
float rotBall = 0;
bool AnimBall = false;

//ANIMACION DEL ESTUDIANTE
glm::vec3 studentPos(-22.0f, -1.0f, 20.0f);
float rot = 0.0f;
int   Anima = 0;
float braD = 0.0f;
float braD2 = 0.0f;
float braI = 0.0f;
float pierD = 0.0f;
float pierI = 0.0f;
float body = 90.0f;
bool step = false;

//Animacion de logos
float roticon;
float posicon;
#define MAX_FRAMES 90
int i_max_steps = 90;
int i_curr_steps = 0;
typedef struct _frame {
float roticon;
float posicon;

float roticonINC;
float posiconINC;


}FRAME;


FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//En qu  lugar de la l nea de tiempo me encuentro al guardar los datos
bool play = false;			//Si se est  reproduciendo la animaci n
int playIndex = 0;			//En qu  lugar de la l nea de tiempo me encuentro al estar reproduciendo la animaci n

void SaveAnimation(const char* filename = "Animacion.txt") {
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error al abrir el archivo para guardar." << std::endl;
		return;
	}

	for (int i = 0; i < FrameIndex; i++) {
		file << KeyFrame[i].roticon << " "
			<< KeyFrame[i].posicon << "\n";
	}
	file.close();
	std::cout << "Animacion guardada correctamente." << std::endl;
}

void LoadAnimation(const char* filename = "Animacion.txt") {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error al abrir el archivo para cargar." << std::endl;
		return;
	}

	FrameIndex = 0;
	while (FrameIndex < MAX_FRAMES &&
		file >> KeyFrame[FrameIndex].roticon
		>> KeyFrame[FrameIndex].posicon)
	{
		FrameIndex++;
	}
}

void PrintAnimation(const char* filename = "Animacion.txt") {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error al abrir el archivo para imprimir." << std::endl;
		return;
	}

	std::string line;
	std::cout << "Contenido del archivo " << filename << ":\n";
	while (std::getline(file, line)) {
		std::cout << line << std::endl;
	}
	file.close();
}

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].posicon = posicon;
	KeyFrame[FrameIndex].roticon = roticon;


	FrameIndex++;
}

void resetElements(void)
{

	posicon = KeyFrame[0].posicon;
	roticon = KeyFrame[0].roticon;



}
void interpolation(void)
{


	KeyFrame[playIndex].posiconINC = (KeyFrame[playIndex + 1].posicon - KeyFrame[playIndex].posicon) / i_max_steps;
	KeyFrame[playIndex].roticonINC = (KeyFrame[playIndex + 1].roticon - KeyFrame[playIndex].roticon) / i_max_steps;
}

//// ANIMACION PARED
glm::vec3 canonPos(-26.0f, -1.0f, 20.0f);
glm::vec3 movBala(0.0f, 0.0f, -2.57423f);
glm::vec3 fragPared1(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared2(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared3(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared4(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared5(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared6(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared7(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared8(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared9(0.0f, 0.0f, 0.0f);
glm::vec3 fragPared10(0.0f, 0.0f, 0.0f);
float rotcanon = 90.0f;
float canon = 0.0f;
int   CanonWall = 0;
float Sboom = 0.0f;




// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto de Teoria. Sala B. Equipo 4.", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);



	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	
//// SKYBOX
	Shader skyboxShader("Shader/SkyBox.vs", "Shader/SkyBox.frag");


	//models
	Model Piso((char*)"Models/piso.obj");
	Model dron((char*)"Models/dron/model.obj");
	Model full((char*)"Models/salon/salonnn.obj");
	Model escribe((char*)"Models/escribe/escribe.obj");
	Model nuevosalon((char*)"Models/nuevosalon/salon_nuevo.obj");
	Model escritorionuevo((char*)"Models/mesanueva/model.obj");
	Model sillanueva((char*)"Models/sillanueva/silla.obj");
	Model imac((char*)"Models/imac/model.obj");
	Model logo((char*)"Models/logo/model.obj");
	Model escritorioviejo((char*)"Models/texturas_Salon_viejo/Tex_escritorio.obj");
	Model pcvieja((char*)"Models/texturas_Salon_viejo/Tex_electricos.obj");
	Model sillavieja((char*)"Models/texturas_Salon_viejo/Tex_silla.obj");
	Model lava((char*)"Models/lava/lava.obj");


	//ANIMACION DEL ESTUDIANTE
	Model persona((char*)"Models/persona/Main.obj");
	Model brazoI((char*)"Models/persona/L_arm.obj");
	Model brazoD((char*)"Models/persona/R_arm.obj");
	Model piernaI((char*)"Models/persona/L_Leg.obj");
	Model piernaD((char*)"Models/persona/R_Leg.obj");
	Model pieI((char*)"Models/persona/L_Foot.obj");
	Model pieD((char*)"Models/persona/R_Foot.obj");

	//// ANIMACION PARED
	//Animaci n paredes
	Model Prueba((char*)"Models/anim_explosion/todo.obj");
	Model Bala((char*)"Models/anim_explosion/Bala.obj");
	Model Boom((char*)"Models/anim_explosion/Boom.obj");
	Model P1((char*)"Models/nuevosalon/P1.obj");
	Model P2((char*)"Models/nuevosalon/P2.obj");
	Model P3((char*)"Models/nuevosalon/P3.obj");
	Model P4((char*)"Models/nuevosalon/P4.obj");
	Model P5((char*)"Models/nuevosalon/P5.obj");
	Model P6((char*)"Models/nuevosalon/P6.obj");
	Model P7((char*)"Models/nuevosalon/P7.obj");
	Model P8((char*)"Models/nuevosalon/P8.obj");
	Model P9((char*)"Models/nuevosalon/P9.obj");
	Model P10((char*)"Models/nuevosalon/P10.obj");

	//// SKYBOX
//
//
//
//

	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	//
//
//
//
//// SKYBOX
//
//
//
//

	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	//
//
//
//
//// SKYBOX
//
//
//
//

	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);


	//
//
//
//
//// SKYBOX
//
//
//
//

	//SkyBox

	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//
//
//
//
//// SKYBOX
//
//
//
//

	//Load textures
	vector < const GLchar*> faces;
	faces.push_back("SkyBox2/right.jpg");
	faces.push_back("SkyBox2/left.jpg");
	faces.push_back("SkyBox2/top.jpg");
	faces.push_back("SkyBox2/bottom.jpg");
	faces.push_back("SkyBox2/back.jpg");
	faces.push_back("SkyBox2/front.jpg");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);



	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	LoadAnimation(); //Carga la animaci n por medio del archivo previamente guardado
	PrintAnimation(); //Imprime en terminar los valores del archivo
	resetElements(); // Resetear los elementos a los primeros keyframes cargados

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		Animation();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);






		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		glm::mat4 modelTemp = glm::mat4(1.0f); //Temp
		glm::mat4 modelTemp2 = glm::mat4(1.0f); //Temp


		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);


		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);


		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);


		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));


		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);

		if (showsilla) {
			for (int i = 0; i < numSillas; i++) {
				view = camera.GetViewMatrix();
				model = glm::mat4(1.0f);
				model = glm::translate(model, posicionesIniciales[i] + sillaPosArr[i]);
				model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
				model = glm::rotate(model, glm::radians(sillaRotArr[i] + 180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				sillanueva.Draw(lightingShader);
			}
		}

		if (showlava) {
			/////////////Modelo de Lava/////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(0.06f, 0.09f, 0.09f));
			model = glm::translate(model, glm::vec3(40.0f, -4.8f, 35.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			lava.Draw(lightingShader);

		}

		if (faseAnimacion < 2) {
			/////////////////////////////////////////////////////////////Modelos de iMac viejas////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////Primer mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(15.6f, -0.9f - fallDistance, -22.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(12.6f, -0.9f - fallDistance, -22.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(9.7f, -0.9f - fallDistance, -22.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(6.7f, -0.9f - fallDistance, -22.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Segunda mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(15.6f, -0.9f - fallDistance, -16.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(12.6f, -0.9f - fallDistance, -16.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(9.7f, -0.9f - fallDistance, -16.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(6.7f, -0.9f - fallDistance, -16.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Tercer mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(15.6f, -0.9f - fallDistance, -10.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(12.6f, -0.9f - fallDistance, -10.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(9.7f, -0.9f - fallDistance, -10.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(6.7f, -0.9f - fallDistance, -10.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);



			////////////////////////////////////////////////////////////Cuarta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(15.6f, -0.9f - fallDistance, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(12.6f, -0.9f - fallDistance, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(9.7f, -0.9f - fallDistance, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(6.7f, -0.9f - fallDistance, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Quinta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(15.9f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(13.5f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(11.2f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(8.8f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(6.5f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Sexta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(15.9f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(13.5f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(11.2f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(8.8f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(6.5f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Septima mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(15.9f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(13.5f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(11.2f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(8.8f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(6.5f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			//////////////////////////////////////////////////////////////Mesas de la IZQUIERDA////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////Primer mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-9.8f, -0.9f - fallDistance, -22.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-6.8f, -0.9f - fallDistance, -22.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-3.9f, -0.9f - fallDistance, -22.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.9f, -0.9f - fallDistance, -22.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Segunda mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-9.8f, -0.9f - fallDistance, -16.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-6.8f, -0.9f - fallDistance, -16.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-3.9f, -0.9f - fallDistance, -16.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.9f, -0.9f - fallDistance, -16.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);


			////////////////////////////////////////////////////////////Tercer mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-9.8f, -0.9f - fallDistance, -10.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-6.8f, -0.9f - fallDistance, -10.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-3.9f, -0.9f - fallDistance, -10.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.9f, -0.9f - fallDistance, -10.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Cuarta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-9.8f, -0.9f - fallDistance, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-6.8f, -0.9f - fallDistance, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-3.9f, -0.9f - fallDistance, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.9f, -0.9f - fallDistance, -4.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Quinta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-10.0f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-7.6f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-5.3f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-2.9f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.6f, -0.9f - fallDistance, 2.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Sexta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-10.0f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-7.6f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-5.3f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-2.9f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.6f, -0.9f - fallDistance, 8.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Septima mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-10.0f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-7.6f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-5.3f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-2.9f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.6f, -0.9f - fallDistance, 14.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			pcvieja.Draw(lightingShader);


			///////////////////////////////////////////////////////////////////////////Modelos de mesas viejas///////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////Primer mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.2f, -1.0f - fallDistance, -20.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Segunda mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.2f, -1.0f - fallDistance, -14.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Tercer mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.2f, -1.0f - fallDistance, -7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);


			////////////////////////////////////////////////////////////Cuarta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.2f, -1.0f - fallDistance, -1.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Quinta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.2f, -1.0f - fallDistance, 4.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Sexta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.2f, -1.0f - fallDistance, 10.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Septima mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.2f, -1.0f - fallDistance, 17.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Octava mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(11.3f, -1.0f - fallDistance, 20.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////Primer mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-8.2f, -1.0f - fallDistance, -20.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Segunda mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-8.2f, -1.0f - fallDistance, -14.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Tercer mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-8.2f, -1.0f - fallDistance, -7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);


			////////////////////////////////////////////////////////////Cuarta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-8.2f, -1.0f - fallDistance, -1.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Quinta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-8.2f, -1.0f - fallDistance, 4.8f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Sexta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-8.2f, -1.0f - fallDistance, 10.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);

			////////////////////////////////////////////////////////////Septima mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-8.2f, -1.0f - fallDistance, 17.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorioviejo.Draw(lightingShader);




			///////////////////////////////////////////////////////////////////////////Modelos de sillas viejas///////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////Primer mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(13.0f, -1.2f - fallDistance, -24.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(10.25f, -1.2f - fallDistance, -24.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(7.32f, -1.2f - fallDistance, -24.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(4.25f, -1.2f - fallDistance, -24.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Segunda mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(13.0f, -1.2f - fallDistance, -17.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(10.25f, -1.2f - fallDistance, -17.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(7.32f, -1.2f - fallDistance, -17.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(4.25f, -1.2f - fallDistance, -17.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Tercera mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(13.0f, -1.2f - fallDistance, -11.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(10.25f, -1.2f - fallDistance, -11.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(7.32f, -1.2f - fallDistance, -11.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(4.25f, -1.2f - fallDistance, -11.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);


			////////////////////////////////////////////////////////////Cuarta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(13.0f, -1.2f - fallDistance, -5.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(10.25f, -1.2f - fallDistance, -5.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(7.32f, -1.2f - fallDistance, -5.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(4.25f, -1.2f - fallDistance, -5.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Quinta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(13.3f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(11.1f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.8f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(6.3f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(4.0f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Sexta mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(13.3f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(11.1f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.8f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(6.3f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(4.0f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Septima mesa de la derecha///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(13.3f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(11.1f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(8.8f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(6.3f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(4.0f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////Primer mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-12.0f, -1.2f - fallDistance, -24.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-9.25f, -1.2f - fallDistance, -24.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-6.32f, -1.2f - fallDistance, -24.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-3.25f, -1.2f - fallDistance, -24.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Segunda mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-12.0f, -1.2f - fallDistance, -17.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-9.25f, -1.2f - fallDistance, -17.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-6.32f, -1.2f - fallDistance, -17.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-3.25f, -1.2f - fallDistance, -17.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);


			////////////////////////////////////////////////////////////Tercera mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-12.0f, -1.2f - fallDistance, -11.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-9.25f, -1.2f - fallDistance, -11.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-6.32f, -1.2f - fallDistance, -11.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-3.25f, -1.2f - fallDistance, -11.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);


			////////////////////////////////////////////////////////////Cuarta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-12.0f, -1.2f - fallDistance, -5.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-9.25f, -1.2f - fallDistance, -5.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-6.32f, -1.2f - fallDistance, -5.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-3.25f, -1.2f - fallDistance, -5.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Quinta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-12.3f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-10.1f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-7.8f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-5.3f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-3.0f, -1.2f - fallDistance, 1.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Sexta mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-12.3f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-10.1f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-7.8f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-5.3f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-3.0f, -1.2f - fallDistance, 7.6f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			////////////////////////////////////////////////////////////Septima mesa de la IZQUIERDA///////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-12.3f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-10.1f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-7.8f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-5.3f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(-3.0f, -1.2f - fallDistance, 13.9f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::scale(model, glm::vec3(1.0f, 1.1f, 1.0f));
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			sillavieja.Draw(lightingShader);

		}

		if (faseAnimacion >= 1) {



			//////////////////////////////////	/*Modelos de escritorios nuevos*///////////////////////////////////////////////////////
			/*Derecha*/
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, -13.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			//Izquierda
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(3.6f, 0.9f - riseDistance, -13.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			//Derecha
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, -9.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			//Izquierda
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(3.6f, 0.9f - riseDistance, -9.4f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			/////Derecha
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, -5.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			//Izquierda
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(3.6f, 0.9f - riseDistance, -5.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);


			//Derecha
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, -1.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			//Izquierda
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(3.6f, 0.9f - riseDistance, -1.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);


			//Derecha
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, 3.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			//Izquierda
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(3.6f, 0.9f - riseDistance, 3.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);


			//Derecha
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, 7.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			//Izquierda
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(3.6f, 0.9f - riseDistance, 7.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);


			//Derecha
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, 11.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);

			//Izquierda
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(3.6f, 0.9f - riseDistance, 11.2f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);



			//Mesa profesor //Derecha
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.3f, 1.5f, 1.5f));
			model = glm::translate(model, glm::vec3(15.1f, 0.9f - riseDistance, 13.5f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			escritorionuevo.Draw(lightingShader);


			/////////////////////////////////////////////////////////Modelos de iMac////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////Primer mesa de la derecha//////////////////////////////////////////////////////////////////////////

			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(11.0f, 2.2f - riseDistance, -17.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(8.4f, 2.2f - riseDistance, -17.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(5.9f, 2.2f - riseDistance, -17.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(3.3f, 2.2f - riseDistance, -17.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Seguunda mesa de la derecha//////////////////////////////////////////////////////////////////////////
			// 
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(11.0f, 2.2f - riseDistance, -12.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(8.4f, 2.2f - riseDistance, -12.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(5.9f, 2.2f - riseDistance, -12.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(3.3f, 2.2f - riseDistance, -12.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Tercera mesa de la derecha//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(11.0f, 2.2f - riseDistance, -6.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(8.4f, 2.2f - riseDistance, -6.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(5.9f, 2.2f - riseDistance, -6.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(3.3f, 2.2f - riseDistance, -6.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Cuarta mesa de la derecha//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(11.0f, 2.2f - riseDistance, -1.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(8.4f, 2.2f - riseDistance, -1.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(5.9f, 2.2f - riseDistance, -1.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(3.3f, 2.2f - riseDistance, -1.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Cuarta mesa de la derecha//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(11.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(9.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(7.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(5.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(3.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Quinta mesa de la derecha//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(11.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(9.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(7.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(5.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(3.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Sexta mesa de la derecha//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(11.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(9.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(7.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(5.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(3.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////Primer mesa de la izquierda//////////////////////////////////////////////////////////////////////////
			// 
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-11.0f, 2.2f - riseDistance, -17.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-8.4f, 2.2f - riseDistance, -17.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-5.9f, 2.2f - riseDistance, -17.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-3.3f, 2.2f - riseDistance, -17.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Seguunda mesa de la izquierda//////////////////////////////////////////////////////////////////////////
			// 
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-11.0f, 2.2f - riseDistance, -12.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-8.4f, 2.2f - riseDistance, -12.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-5.9f, 2.2f - riseDistance, -12.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-3.3f, 2.2f - riseDistance, -12.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Tercera mesa de la izquierda//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-11.0f, 2.2f - riseDistance, -6.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-8.4f, 2.2f - riseDistance, -6.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-5.9f, 2.2f - riseDistance, -6.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-3.3f, 2.2f - riseDistance, -6.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Cuarta mesa de la izquierda//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-11.0f, 2.2f - riseDistance, -1.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-8.4f, 2.2f - riseDistance, -1.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-5.9f, 2.2f - riseDistance, -1.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-3.3f, 2.2f - riseDistance, -1.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Cuarta mesa de la izquierda//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-11.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-9.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-7.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-5.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-3.2f, 2.2f - riseDistance, 4.3f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Quinta mesa de la izquierda//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-11.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-9.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-7.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-5.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-3.2f, 2.2f - riseDistance, 9.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			////////////////////////////////////////////////////////////Sexta mesa de la izquierda//////////////////////////////////////////////////////////////////////////
			//Modelo de iMac
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-11.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-9.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-7.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-5.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.15f, 1.15f, 1.15f));
			model = glm::translate(model, glm::vec3(-3.2f, 2.2f - riseDistance, 15.1f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			imac.Draw(lightingShader);

			//////////////////////////////////////////////////////////////////Modelo de logos////////////////////////////////////////////////////////////////////////////////////////////////////
			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(27.0f, 27.0f, 27.0f));
			model = glm::translate(model, glm::vec3(-0.48f, 0.22f, 0.88f - posicon)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f + roticon), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90? a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			logo.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(27.0f, 27.0f, 27.0f));
			model = glm::translate(model, glm::vec3(0.56f, 0.22f, 0.87f - posicon)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f + roticon), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90? a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			logo.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(27.0f, 27.0f, 27.9f));
			model = glm::translate(model, glm::vec3(-0.45f, 0.22f, -0.95f + posicon)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(roticon), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90? a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			logo.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(27.0f, 27.0f, -27.0f));
			model = glm::translate(model, glm::vec3(0.49f, 0.22f, 0.968f - posicon)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(roticon), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90? a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			logo.Draw(lightingShader);

		}

		///////////////////////////////////////////////////////////////////////////Salón Nueevoooo////////////////////////////////////////////////////////////////////////////////////////////////////
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::translate(model, glm::vec3(15.1f, -0.1f, 14.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		nuevosalon.Draw(lightingShader);



		////////////////////////////////////////////////////////////////////////////////////Modelo del Dron////////////////////////////////////////////////////////////////////////////////////////////
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		model = glm::mat4(1);
		model = glm::translate(model, dronPos); // Aplicar posición actual
		model = glm::scale(model, glm::vec3(0.8f));
		model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación en Y
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		dron.Draw(lightingShader);
		glBindVertexArray(0);

		//// ANIMACION PARED
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::translate(model, glm::vec3(15.1f, -0.1f, 14.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		nuevosalon.Draw(lightingShader);
		// P1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared1));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P1.Draw(lightingShader);
		// P2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared2));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P2.Draw(lightingShader);
		// P3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared3));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P3.Draw(lightingShader);
		// P4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared4));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P4.Draw(lightingShader);
		// P5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared5));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P5.Draw(lightingShader);
		// P6
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared6));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P6.Draw(lightingShader);
		// P7
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared7));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P7.Draw(lightingShader);
		// P8
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared8));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P8.Draw(lightingShader);
		// P9
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared9));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P9.Draw(lightingShader);
		// P10
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(fragPared10));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		P10.Draw(lightingShader);

		//ANIMACION DEL ESTUDIANTE
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//Cuerpo
		modelTemp = model = glm::translate(model, studentPos);
		model = glm::rotate(model, glm::radians(body), glm::vec3(0.0f, 1.0f, 0.0f));
		modelTemp = model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		persona.Draw(lightingShader);
		//Brazo Derecho
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.90f, 7.4889f, 0.0f));
		model = glm::rotate(model, glm::radians(braD), glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(braD2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		brazoD.Draw(lightingShader);
		//Brazo Izquierdo
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.9f, 7.3314f, 0.0f));
		model = glm::rotate(model, glm::radians(braI), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		brazoI.Draw(lightingShader);
		//Pierna Derecha
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.4126f, 4.27f, 0.0f));
		model = glm::rotate(model, glm::radians(pierD), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		piernaD.Draw(lightingShader);
		//Pierna Izquierda
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.397f, 4.1259f, 0.0f));
		model = glm::rotate(model, glm::radians(pierI), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));;
		piernaI.Draw(lightingShader);

		
		//// ANIMACION PARED
		//Ca on
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);


		modelTemp2 = model = glm::translate(model, canonPos);
		model = glm::rotate(model, glm::radians(canon), glm::vec3(0.0f, 1.0f, 0.0f));
		modelTemp2 = model = glm::rotate(model, glm::radians(rotcanon), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Prueba.Draw(lightingShader);
		//Bala
		model = modelTemp2;
		model = glm::translate(model, movBala);
		//model = glm::rotate(model, glm::radians(pierI), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));;
		Bala.Draw(lightingShader);

		//Boom
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(Sboom, Sboom, 1.0));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, -17.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Boom.Draw(lightingShader);





		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		//// SKYBOX
		//Draw Sky
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);


		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	//// SKYBOX
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();



	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);


	}

	if (keys[GLFW_KEY_T])
	{
		pointLightPositions[0].x += 0.01f;
	}
	if (keys[GLFW_KEY_G])
	{
		pointLightPositions[0].x -= 0.01f;
	}

	if (keys[GLFW_KEY_Y])
	{
		pointLightPositions[0].y += 0.01f;
	}

	if (keys[GLFW_KEY_H])
	{
		pointLightPositions[0].y -= 0.01f;
	}
	if (keys[GLFW_KEY_U])
	{
		pointLightPositions[0].z -= 0.1f;
	}
	if (keys[GLFW_KEY_J])
	{
		pointLightPositions[0].z += 0.01f;
	}

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_0])
	{
		if (faseAnimacion) {

			if (play == false && (FrameIndex > 1))
			{

				resetElements();
				//First Interpolation				
				interpolation();

				play = true;
				playIndex = 0;
				i_curr_steps = 0;
			}
			else
			{

				play = false;
			}
		}


	}


	/*if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		animateFall = !animateFall;
	}*/

	if (key == GLFW_KEY_T && action == GLFW_PRESS) { // Tecla R para activar
		animateRise = !animateRise;
		if (animateRise) {
			// Resetear valores iniciales
			riseDistance = 2.0f;
			rotationRiseAngle = 0.0f;
		}
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		//animateFall = true;
		//animationPaused = !animationPaused; // Iniciar/reanudar animación
		if (!animacionActiva) {
			animacionActiva = true;
			faseAnimacion = 1;
			showlava = false; // Oculta la lava para revelar la animación
		}
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		// Reiniciar valores
		animateFall = false;
		animationPaused = true;
		fallDistance = 0.0f;
		rotationAngle = 0.0f;
	}


	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		show = !show; // Toggle para mostrar solo sillas
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		shownew = !shownew; // Toggle para mostrar solo sillas
	}

	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		showlava = !showlava; // Toggle para mostrar solo la lava
	}

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(1.0f, 1.0f, 0.0f);

		}
		else
		{
			Light1 = glm::vec3(0);
		}
	}

	if (keys[GLFW_KEY_N])
	{
		AnimBall = !AnimBall;

	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		sinusoidalMode = !sinusoidalMode;
		if (sinusoidalMode) {
			// Configurar el primer segmento
			currentWaypoint = 0;
			segmentStart = waypoints[currentWaypoint];
			int nextWP = (currentWaypoint + 1) % 4;
			segmentEnd = waypoints[nextWP];
			segmentDir = glm::normalize(segmentEnd - segmentStart);
			segmentLength = glm::distance(segmentStart, segmentEnd);
			segmentProgress = 0.0f;
			sineTime = 0.0f;
			dronPos = segmentStart;
		}
		else {
			currentWaypoint = 0;
			dronPos = waypoints[0]; // Reiniciar al inicio
		}
	}

	// INICIALIZACIÓN DEL MOVIMIENTO DE SILLAS (al presionar 'B')
// ===========================================================
// Este bloque configura todas las posiciones, rotaciones y trayectorias iniciales para las sillas. Agrupa las sillas
// en diferentes zonas del pasillo y escritorios según su índice.

	if (keys[GLFW_KEY_B] && !sillaActiva) {
		showsilla = true;
		sillaActiva = true;  // Se activa el sistema de animación

		for (int i = 0; i < numSillas; i++) {
			// --------- Cálculo de fila y columna ---------
			int fila = i / sillasPorFila;
			int columna = i % sillasPorFila;

			// --------- Configuración de posición y rotación inicial ---------

			// Posiciones iniciales: todas las sillas entran desde la izquierda,
			// separadas ligeramente por columnas
			posicionesIniciales[i] = glm::vec3(-23.7f + columna * 0.25f, -1.2f, 20.2f);

			// Desplazamiento relativo inicial (sin movimiento aún)
			sillaPosArr[i] = glm::vec3(0.0f);

			// Rotaciones iniciales (sin giro aún)
			sillaRotArr[i] = 0.0f;
			rotSillaArr[i] = 0.0f;

			// Inicializamos el primer movimiento de la primera silla
			sillaAnimArr[0] = 1;

			// --------- Destinos y objetivos de rotación ---------

			// Destino intermedio: llegar al pasillo central (eje X = 0.0f)
			destinoIntermedio[i] = glm::vec3(0.0f, -1.2f, posicionesIniciales[i].z);

			// Giro hacia el pasillo (90° a la izquierda)
			rotacionObjetivo[i] = 90.0f;

			// Avance final en Z hasta el fondo del pasillo (-23.0f)
			destinoFinalAvance[i] = glm::vec3(destinoIntermedio[i].x, -1.2f, -23.0f);

			// Rotación final después del avance por el pasillo
			rotacionFinalAjuste[i] = 180.0f;

			// Rotación lateral final al posicionarse
			rotacionPostFinal[i] = 270.0f;

			// ===========================================================
			// AGRUPAMIENTO DE SILLAS EN DIFERENTES ZONAS DEL SALÓN
			// ===========================================================
			// Cada grupo corresponde a un escritorio o posición distinta
			// y se acomoda a la izquierda o derecha del pasillo.
			// ===========================================================

			if (i < 4) {
				// ?? Grupo 1 - izquierda (fondo del salón)
				posicionesFinales[i] = glm::vec3(-12.9f + i * 3.0f, -1.2f, -23.0f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 4 && i < 8) {
				// Grupo 1 - derecha (fondo del salón)
				posicionesFinales[i] = glm::vec3(12.9f - (i - 4) * 3.0f, -1.2f, -23.0f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 8 && i < 12) {
				// Grupo 2 - izquierda (Z = -16.5)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 8) * 3.0f, -1.2f, -16.5f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 12 && i < 16) {
				// Grupo 2 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 12) * 3.0f, -1.2f, -16.5f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 16 && i < 20) {
				// Grupo 3 - izquierda (Z = -9.8)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 16) * 3.0f, -1.2f, -9.8f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 20 && i < 24) {
				// Grupo 3 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 20) * 3.0f, -1.2f, -9.8f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 24 && i < 28) {
				// Grupo 4 - izquierda (Z = -4.1)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 24) * 3.0f, -1.2f, -4.1f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 28 && i < 32) {
				// Grupo 4 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 28) * 3.0f, -1.2f, -4.1f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 32 && i < 36) {
				// Grupo 5 - izquierda (Z = 2.6)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 32) * 3.0f, -1.2f, 2.6f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 36 && i < 40) {
				// Grupo 5 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 36) * 3.0f, -1.2f, 2.6f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 40 && i < 44) {
				// Grupo 6 - izquierda (Z = 8.3)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 40) * 3.0f, -1.2f, 8.3f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 44 && i < 48) {
				// Grupo 6 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 44) * 3.0f, -1.2f, 8.3f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
			else if (i >= 48 && i < 52) {
				// Grupo 7 - izquierda (Z = 14.8)
				posicionesFinales[i] = glm::vec3(-12.9f + (i - 48) * 3.0f, -1.2f, 14.8f);
				rotacionFinalAjuste[i] = 180.0f;
				rotacionPostFinal[i] = 270.0f;
			}
			else if (i >= 52 && i < 56) {
				// Grupo 7 - derecha
				posicionesFinales[i] = glm::vec3(12.9f - (i - 52) * 3.0f, -1.2f, 14.8f);
				rotacionFinalAjuste[i] = 0.0f;
				rotacionPostFinal[i] = -90.0f;
			}
		}

		// Iniciar el movimiento de la primera silla
		sillaAnimArr[0] = 1;
	}

	//ANIMACION DEL ESTUDIANTE
	if (keys[GLFW_KEY_9])
	{
		Anima = 1;

	}

	//// ANIMACION PARED
	if (keys[GLFW_KEY_8])
	{
		CanonWall = 1;

	}


}

void Animation() {

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{

				playIndex = 0;
				play = false;
				printf("termina anim\n");


			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posicon += KeyFrame[playIndex].posiconINC;
			roticon += KeyFrame[playIndex].roticonINC;


			i_curr_steps++;

		}

	}


	// --------- ANIMACIÓN DE LA SILLA EN ORDEN ----------
	if (sillaActiva) {
		for (int i = 0; i < numSillas; i++) {
			switch (sillaAnimArr[i]) {

			case 0: // Espera secuencial
				if (i == 0 || sillaAnimArr[i - 1] >= 2)
					sillaAnimArr[i] = 1;
				break;

			case 1: // Movimiento en X hacia el pasillo
				if (sillaPosArr[i].x < destinoIntermedio[i].x - posicionesIniciales[i].x)
					sillaPosArr[i].x += 0.18f;
				else
					sillaAnimArr[i] = 2;
				break;

			case 2: // Primer giro (todos hacia 90°)
				if (sillaRotArr[i] < rotacionObjetivo[i])
					sillaRotArr[i] += 0.55f;
				else {
					sillaRotArr[i] = rotacionObjetivo[i];
					sillaAnimArr[i] = 3;
				}
				break;

			case 3:
				if (sillaPosArr[i].z > posicionesFinales[i].z - posicionesIniciales[i].z)
					sillaPosArr[i].z -= 0.18f;
				else {
					sillaPosArr[i].z = posicionesFinales[i].z - posicionesIniciales[i].z;
					sillaRotArr[i] = 90.0f;
					sillaAnimArr[i] = 4;
				}
				break;

			case 4:
			{
				// Grupos de la izquierda: índices múltiplos de 8 (0, 8, 16, 24, ...)
				if (i % 8 < 4) { // Solo las 4 sillas de la izquierda por grupo
					int grupoBase = (i / 8) * 8; // Inicio del grupo actual
					if (i == grupoBase || sillaAnimArr[i - 1] >= 6) {
						sillaAnimArr[i] = 40; // Primer giro lateral
					}
				}
				else {
					// Grupo derecho
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.4f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 40;
					}
				}
				break;
			}

			case 40:
			{
				if (i % 8 < 4) {
					// Grupo izquierdo: 90° ? 180°
					if (sillaRotArr[i] < rotacionFinalAjuste[i])
						sillaRotArr[i] += 0.4f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 41;
					}
				}
				else {
					// Grupo derecho: 90° ? 0°
					if (sillaRotArr[i] > rotacionFinalAjuste[i] + 0.1f)
						sillaRotArr[i] -= 0.4f;
					else {
						sillaRotArr[i] = rotacionFinalAjuste[i];
						sillaAnimArr[i] = 41;
					}
				}
				break;
			}

			case 41:
			{
				float destinoX = posicionesFinales[i].x - posicionesIniciales[i].x;

				if ((i % 8 < 4 && sillaPosArr[i].x > destinoX) || (i % 8 >= 4 && sillaPosArr[i].x < destinoX)) {
					sillaPosArr[i].x += (i % 8 < 4 ? -0.06f : 0.06f);
				}
				else {
					sillaPosArr[i].x = destinoX;
					sillaAnimArr[i] = 42;
				}
				break;
			}

			case 42:
			{
				if (sillaRotArr[i] < rotacionPostFinal[i] - 0.1f)
					sillaRotArr[i] += 0.4f;
				else if (sillaRotArr[i] > rotacionPostFinal[i] + 0.1f)
					sillaRotArr[i] -= 0.4f;
				else {
					sillaRotArr[i] = rotacionPostFinal[i];
					sillaAnimArr[i] = 43;
				}
				break;
			}

			case 5:
				if (i < 4) {
					float destinoX = posicionesFinales[i].x - posicionesIniciales[i].x;

					if (sillaPosArr[i].x > destinoX + 0.01f)
						sillaPosArr[i].x -= 0.03f;  // Reduce lento
					else {
						sillaPosArr[i].x = destinoX;
						sillaAnimArr[i] = 6;
					}
				}
				break;

			case 6:
			{
				float diff = rotacionPostFinal[i] - sillaRotArr[i];

				if (fabs(diff) > 0.5f) {
					if (diff > 0)
						sillaRotArr[i] += 0.55f;
					else
						sillaRotArr[i] -= 0.55f;
				}
				else {
					sillaRotArr[i] = rotacionPostFinal[i];
					sillaAnimArr[i] = 7;
				}
				break;
			}


			case 7:
				break;

			case 10:
				if ((i >= 4 && i < 8) || (i >= 12 && i < 16) || (i >= 20 && i < 24) || (i >= 28 && i < 32) || (i >= 36 && i < 40) || (i >= 44 && i < 48) || (i >= 52 && i < 56)) {
					float destinoX = posicionesFinales[i].x - posicionesIniciales[i].x;

					if (sillaPosArr[i].x < destinoX)
						sillaPosArr[i].x += 0.18f;
					else {
						sillaPosArr[i].x = destinoX;
						sillaAnimArr[i] = 11;
					}
				}
				break;

			case 11: // Giro lateral final (grupo derecho)
			{
				float diff = rotacionPostFinal[i] - sillaRotArr[i]; //

				if (fabs(diff) > 0.5f) {
					if (diff > 0)
						sillaRotArr[i] += 0.55f;
					else
						sillaRotArr[i] -= 0.55f;
				}
				else {
					sillaRotArr[i] = rotacionPostFinal[i];
					sillaAnimArr[i] = 12; // Finalizado
				}
				break;
			}

			case 12:
				// Animación final completa
				break;
			}

		}
	}




	if (faseAnimacion == 1) { // Fase de caída
		if (fallDistance < 5.0f) {
			fallDistance += fallSpeed * deltaTime;
			rotationAngle += rotationSpeed * deltaTime;
			showlava = true; // Lava visible
		}
		else {
			faseAnimacion = 2; // Iniciar fase 2
			showlava = false;  // Ocultar lava
		}
	}
	else if (faseAnimacion == 2) { // Fase de ascenso
		if (riseDistance > 0.0f) {
			riseDistance -= riseSpeed * deltaTime;
			rotationRiseAngle += rotationRiseSpeed * deltaTime;
		}
		else {
			animateRise = false;
			// ¡Mantener faseAnimacion en 2 para ocultar modelos viejos!
		}
	}
	///////////////////////////////////////
	if (animateRise) {
		if (riseDistance > 0.0f) {
			float delta = riseSpeed * deltaTime;
			riseDistance -= delta;
			rotationRiseAngle += rotationRiseSpeed * deltaTime; // Ángulo actualizado
		}
		else {
			riseDistance = 0.0f;
			animateRise = false;
		}
	}

	if (!animationPaused && animateFall) { // Solo animar si no está en pausa
		fallDistance += fallSpeed * deltaTime;
		rotationAngle += rotationSpeed * deltaTime;
	}

	if (AnimBall) {
		if (sinusoidalMode) {
			// Movimiento sinusoidal siguiendo waypoints
			segmentProgress += speed * deltaTime;

			// Cambiar de segmento si es necesario
			while (segmentProgress > segmentLength) {
				segmentProgress -= segmentLength;
				currentWaypoint = (currentWaypoint + 1) % 4;
				segmentStart = waypoints[currentWaypoint];
				int nextWP = (currentWaypoint + 1) % 4;
				segmentEnd = waypoints[nextWP];
				segmentDir = glm::normalize(segmentEnd - segmentStart);
				segmentLength = glm::distance(segmentStart, segmentEnd);
			}

			// Calcular posición base en el segmento
			glm::vec3 basePos = segmentStart + segmentDir * segmentProgress;

			// Calcular dirección perpendicular al segmento
			glm::vec3 perp = glm::cross(segmentDir, glm::vec3(0.0f, 1.0f, 0.0f));
			perp = glm::normalize(perp);

			// Aplicar oscilación sinusoidal
			float sineValue = sin(sineTime * sineFrequency) * sineAmplitude;
			dronPos = basePos + perp * sineValue;

			// Actualizar rotación basada en la dirección del segmento
			float angle = atan2(segmentDir.x, segmentDir.z);
			rotBall = glm::degrees(angle);

			// Actualizar tiempo para la onda
			sineTime += deltaTime;

		}
		else {
			// Movimiento rectangular original
			glm::vec3 target = waypoints[currentWaypoint];
			glm::vec3 direction = target - dronPos;
			float distanceToTarget = glm::length(direction);

			if (distanceToTarget > 0.1f) {
				direction = glm::normalize(direction);
				dronPos += direction * speed * deltaTime;
				float angle = atan2(direction.x, direction.z);
				rotBall = glm::degrees(angle);
			}
			else {
				currentWaypoint = (currentWaypoint + 1) % 4;
			}
		}
	}

	if (Anima == 1) {


		if (!step) {	//STATE 1
			braD += 0.3f;
			braI += 0.3f;
			pierD += 0.3f;
			pierI += 0.3f;
			if (braI > 30.0f) {
				step = true;
			}
		}
		else {
			braD -= 0.3f;
			braI -= 0.3f;
			pierD -= 0.3f;
			pierI -= 0.3f;
			if (braI < -30.0f) {
				step = false;
			}
		}


		studentPos.x += 0.01;

		if (studentPos.x >= -0.1f) {

			Anima = 4;

		}

	}

	else if (Anima == 4) {

		body += 0.1;

		if (body >= 180.0f) {

			Anima = 5;

		}

	}

	else if (Anima == 5) {


		if (!step) {
			braD += 0.3f;
			braI += 0.3f;
			pierD += 0.3f;
			pierI += 0.3f;
			if (braI > 30.0f) {
				step = true;
			}
		}
		else {
			braD -= 0.3f;
			braI -= 0.3f;
			pierD -= 0.3f;
			pierI -= 0.3f;
			if (braI < -30.0f) {
				step = false;
			}
		}


		studentPos.z -= 0.01;

		if (studentPos.z <= -23.0f) {

			Anima = 8;

		}

	}

	else if (Anima == 8) {

		body += 0.1;

		if (body >= 270.0f) {

			Anima = 9;

		}

	}

	else if (Anima == 9) {


		if (!step) {	//STATE 1
			braD += 0.3f;
			braI += 0.3f;
			pierD += 0.3f;
			pierI += 0.3f;
			if (braI > 30.0f) {
				step = true;
			}
		}
		else {
			braD -= 0.3f;
			braI -= 0.3f;
			pierD -= 0.3f;
			pierI -= 0.3f;
			if (braI < -30.0f) {
				step = false;
			}
		}


		studentPos.x -= 0.01;

		if (studentPos.x <= -16.5f) {

			Anima = 10;

		}

	}

	else if (Anima == 10) {

		body += 0.1;

		if (body >= 360.0f) {

			Anima = 11;

		}

	}

	else if (Anima == 11) {


		if (!step) {
			braD += 0.3f;
			braI += 0.3f;
			pierD += 0.3f;
			pierI += 0.3f;
			if (braI > 30.0f) {
				step = true;
			}
		}
		else {
			braD -= 0.3f;
			braI -= 0.3f;
			pierD -= 0.3f;
			pierI -= 0.3f;
			if (braI < -30.0f) {
				step = false;
			}
		}


		studentPos.z += 0.01;

		if (studentPos.z >= 20.5f) {

			Anima = 12;

		}

	}

	else if (Anima == 12) {

		body += 0.1;

		if (body >= 450.0f) {

			Anima = 13;

		}

	}

	else if (Anima == 13) {


		if (!step) {	//STATE 1
			braD += 0.3f;
			braI += 0.3f;
			pierD += 0.3f;
			pierI += 0.3f;
			if (braI > 30.0f) {
				step = true;
			}
		}
		else {
			braD -= 0.3f;
			braI -= 0.3f;
			pierD -= 0.3f;
			pierI -= 0.3f;
			if (braI < -30.0f) {
				step = false;
			}
		}


		studentPos.x += 0.01;

		if (studentPos.x >= -2.0f) {

			Anima = 16;

		}

	}

	else if (Anima == 16) {

		body += 0.1;

		if (body >= 540.0f) {

			Anima = 17;

		}

	}

	else if (Anima == 17) {

		braD += 0.3;

		if (braD >= 180.0f) {

			Anima = 18;

		}

	}

	else if (Anima == 18) {

		braD -= 0.3;

		if (braD <= 0.0f) {

			Anima = 19;

		}

	}

	else if (Anima == 19) {

		body += 0.1;

		if (body >= 630.0f) {

			Anima = 20;

		}

	}

	else if (Anima == 20) {

		if (!step) {
			braD += 0.3f;
			braI += 0.3f;
			pierD += 0.3f;
			pierI += 0.3f;
			if (braI > 30.0f) {
				step = true;
			}
		}
		else {
			braD -= 0.3f;
			braI -= 0.3f;
			pierD -= 0.3f;
			pierI -= 0.3f;
			if (braI < -30.0f) {
				step = false;
			}
		}


		studentPos.x -= 0.01;

		if (studentPos.x <= -22.0f) {

			Anima = 23;

		}

	}

	else if (Anima == 23) {

		studentPos.x = -22.0f;
		studentPos.y = -1.0f;
		studentPos.z = 20.0f;
		Anima = 0;
		braD = 0.0f;
		braD2 = 0.0f;
		braI = 0.0f;
		pierD = 0.0f;
		pierI = 0.0f;
		body = 90.0f;
		step = false;
		rot = 0.0f;

	}

	//// ANIMACION PARED
	if (CanonWall == 1) {

		canonPos.x += 0.01;

		if (canonPos.x >= -0.1f) {

			CanonWall = 2;

		}

	}

	else if (CanonWall == 2) {

		canon += 0.1;

		if (canon >= 90.0f) {

			CanonWall = 3;

		}

	}

	else if (CanonWall == 3) {

		canonPos.z -= 0.01;

		if (canonPos.z <= 8.0f) {

			CanonWall = 4;

		}

	}

	else if (CanonWall == 4) {

		movBala.z += 0.05;
		canonPos.z = 9;

		if (movBala.z >= 35.0f) {

			CanonWall = 5;

		}

	}

	else if (CanonWall == 5) {

		Sboom += 0.005;
		canonPos.y -= 0.2;

		if (Sboom >= 3.0f) {

			CanonWall = 7;

		}

	}

	else if (CanonWall == 6) {
		Sboom -= 0.005;


		if (Sboom <= 0.0f) {
			CanonWall = 7;
		}
	}

	else if (CanonWall == 7) {


		Sboom -= 0.002;

		fragPared1.z -= 0.003;
		fragPared2.z -= 0.020;
		fragPared3.z -= 0.030;
		fragPared4.z -= 0.025;
		fragPared5.z -= 0.008;
		fragPared6.z -= 0.012;
		fragPared7.z -= 0.05;
		fragPared8.z -= 0.015;
		fragPared9.z -= 0.001;
		fragPared10.z -= 0.008;

		fragPared1.y += 0.005;
		fragPared2.y -= 0.005;
		fragPared3.y += 0.005;
		fragPared4.y -= 0.005;
		fragPared5.y -= 0.005;
		fragPared6.y -= 0.005;
		fragPared8.y -= 0.005;
		fragPared9.y -= 0.005;
		fragPared10.y += 0.005;


		if (Sboom <= 0.0f) {
			CanonWall = 8;
		}

	}

	else if (CanonWall == 8) {

		canonPos.x = -26.0;
		canonPos.y = -1.0f;
		canonPos.z = 20.0f;
		movBala.z = -2.57423f;
		fragPared1.y = 0.0f;
		fragPared2.y = 0.0f;
		fragPared3.y = 0.0f;
		fragPared4.y = 0.0f;
		fragPared5.y = 0.0f;
		fragPared6.y = 0.0f;
		fragPared7.y = 0.0f;
		fragPared8.y = 0.0f;
		fragPared9.y = 0.0f;
		fragPared10.y = 0.0f;

		fragPared1.z = 0.0f;
		fragPared2.z = 0.0f;
		fragPared3.z = 0.0f;
		fragPared4.z = 0.0f;
		fragPared5.z = 0.0f;
		fragPared6.z = 0.0f;
		fragPared7.z = 0.0f;
		fragPared8.z = 0.0f;
		fragPared9.z = 0.0f;
		fragPared10.z = 0.0f;

		rotcanon = 90.0f;
		canon = 0.0f;
		CanonWall = 0;


	}




}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}