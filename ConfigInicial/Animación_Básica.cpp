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



////
// ANIMACIÓN NUEVOS MODELOS
bool animateRise = false;
float riseDistance = 4.0f;  // Distancia inicial bajo la lava
float riseSpeed = 1.5f;     // Velocidad de ascenso
float rotationRiseAngle = 0.0f;
float rotationRiseSpeed = 180.0f; // Velocidad de rotación durante el ascenso

////

bool show = false; // Variable global para mostrar salones
bool shownew = false; // Variable global para mostrar salones

bool animateFall = false;
float fallDistance = 0.0f;
float rotationAngle = 0.0f;
float fallSpeed = 3.0f;       // Velocidad de caída
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
	glm::vec3(15.0f, 9.0f, -22.0f),   // Esquina inferior-derecha (B)
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
glm::vec3 segmentStart;  // <--- ¡Nueva!
glm::vec3 segmentEnd;    // <--- ¡Nueva!
glm::vec3 segmentDir;    // <--- ¡Nueva!
float segmentLength = 0.0f;  // <--- ¡Nueva!
float segmentProgress = 0.0f;  // <--- ¡Nueva!

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Animacion basica", nullptr, nullptr);

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





	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

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



		////Carga de modelo 
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Piso.Draw(lightingShader);

		/////Salon con mesas, sillas y computadoras
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(-9.0f, -1.0f, 25.0f)); // Ajusta X,Y,Z
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//full.Draw(lightingShader);




		/////////////Modelo de Lava/////////////////////////
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.06f, 0.09f, 0.09f));
		model = glm::translate(model, glm::vec3(40.0f, -4.8f, 35.0f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		lava.Draw(lightingShader);

		if (!show) {
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

		if (!shownew) {


			//////////////////////////////////	/*Modelos de escritorios nuevos*///////////////////////////////////////////////////////
			/*Derecha*/
			//view = camera.GetViewMatrix();
			//model = glm::mat4(1);
			//model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			//model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, -13.7f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//escritorionuevo.Draw(lightingShader);


			//// Ejemplo para uno de los modelos (aplica a todos los que necesiten rotación)
			//view = camera.GetViewMatrix();
			//model = glm::mat4(1);
			//model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));
			//model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, -13.7f));
			//model = glm::rotate(model, glm::radians(rotationRiseAngle), glm::vec3(1.0f, 1.0f, 1.0f)); // Rotación Z
			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//escritorionuevo.Draw(lightingShader);

			//// Modelo de escritorio nuevo (dentro de if(!shownew))
			//view = camera.GetViewMatrix();
			//model = glm::mat4(1);
			//model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f)); // Escala
			//model = glm::rotate(model, glm::radians(rotationRiseAngle), glm::vec3(1.0f, 1.0f, 1.0f)); // Rotación en Y (¡antes de trasladar!)
			//model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, -13.7f)); // Posición con riseDistance
			//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación existente (orientación)
			//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			//escritorionuevo.Draw(lightingShader);

			model = glm::mat4(1.0f);
			// Primero traslación (incluye la animación de subida)
			model = glm::translate(model, glm::vec3(9.0f, 0.9f - riseDistance, -13.7f));
			// Luego rotación animada (gira sobre su propio eje en su nueva posición)
			model = glm::rotate(model, glm::radians(rotationRiseAngle), glm::vec3(1.0f, 1.0f, 1.0f));
			// Luego rotación fija (orientación final del modelo)
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			// Finalmente escala
			model = glm::scale(model, glm::vec3(3.0f, 1.5f, 1.5f));


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
			model = glm::translate(model, glm::vec3(-0.48f, 0.22f - riseDistance, 0.88f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			logo.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(27.0f, 27.0f, 27.0f));
			model = glm::translate(model, glm::vec3(0.56f, 0.22f - riseDistance, 0.87f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			logo.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(27.0f, 27.0f, 27.9f));
			model = glm::translate(model, glm::vec3(-0.45f, 0.22f - riseDistance, -0.95f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			logo.Draw(lightingShader);

			view = camera.GetViewMatrix();
			model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(27.0f, 27.0f, -27.0f));
			model = glm::translate(model, glm::vec3(0.56f, 0.22f - riseDistance, 0.968f)); // Ajusta X,Y,Z   ////// Posicion correctya en x
			//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Giro de 90° a la derecha (eje Y)
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



		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


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
		animateFall = true;
		animationPaused = !animationPaused; // Iniciar/reanudar animación
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
			Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
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
			dronPos = segmentStart; // Opcional: ajustar al waypoint inicial
		}
		else {
			currentWaypoint = 0;
			dronPos = waypoints[0]; // Reiniciar al inicio
		}
	}

}

void Animation() {

	/*if (animateFall) {
		fallDistance += fallSpeed * deltaTime;
		rotationAngle += rotationSpeed * deltaTime;
	}*/
	//////if (animateRise) {
	//////	// Actualizar distancia y rotación
	//////	riseDistance -= riseSpeed * deltaTime;
	//////	rotationRiseAngle += rotationRiseSpeed * deltaTime;

	//////	// Limitar valores mínimos
	//////	if (riseDistance <= 0.0f) {
	//////		riseDistance = 0.0f;
	//////		animateRise = false; // Detener animación
	//////	}
	//////}

	////// Animación de subida con rotación
	////if (animateRise) {
	////	if (riseDistance > 0.0f) {
	////		float delta = riseSpeed * deltaTime;
	////		riseDistance -= delta;
	////		rotationRiseAngle += rotationRiseSpeed * deltaTime; // Actualizar ángulo
	////	}
	////	else {
	////		riseDistance = 0.0f;
	////		animateRise = false;
	////	}
	////}

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