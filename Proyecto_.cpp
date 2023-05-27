/*
* Universidad Nacional Autónoma de México.
* Facultad de Ingeniería.
* Computación gráfica e interacción humano-computadora.
* Grupo: 6.
* Proyecto.
* Alumnos: Juan Andrés Cruz Romero.
*		   Alexis Rafael del Valle Aragón.
*		   Barulio Mauricio Blancas Galicia.//
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
using namespace std;

const float toRadians = 3.14159265f / 180.0f;

//Declaramos nuestras variables con las cuales realizaremos el ciclo de día y de noche en el proyecto
float movSun;
float movySun;
float movSunOffset;
float luz;
bool dia;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture pisoTexture;

Model Camino_M;
Model Blackhawk_M;
//-------Modelo PORSCHE.-------
Model PORSCHE_M;
Model PORSCHELLANTA_M;
//-------Modelo esfera 1 estrella.-------
Model ESFERA1ESTRELLA_M;
//-------Modelo faro público.-------
Model FAROPUBLICO_M;
//-------Modelo peralte.-------
Model PERALTE_M;
//-------Modelo Casa de Kaiosama.-------
Model Casa_Kaio;
//-------Maquina expendedora.-------
Model MaquinaExpendedora;
//-------Ring ML.-------
Model Ring_ML;
//-------Lata.-------
Model Lata;
//-------Nube Voladora.-------
Model Nube_Goku;
//-------Modelo autopista.-------
Model AUTOPISTA_M;

Skybox skybox;
Skybox skybox1;
Skybox skybox2;

//materiales
Material Material_brillante;
Material Material_opaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";
// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//Input con keyframes.
void inputKeyframes(bool* keys);


//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

	};

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//-------Keyframes.-------
//Variables animación.
float Rotate_ = 0.0f;
float RotateOffset_ = 4.0f;
bool FlagForward_;
float reproduciranimacion, habilitaranimacion,
guardoFrame, reinicioFrame, contador = 0;
bool animacion = false;
float PosicionX_ = 0.0f, PosicionY_ = 0.0f, PosicionZ_ = 0.0f;//Posición inicial.
float MovimientoX_ = 0.0f, MovimientoY_ = 0.0f, MovimientoZ_ = 0.0f;//Movimiento en ejes.
float Giro_ = 0;
float Ciclo_, Ciclo_1;

#define MAX_FRAMES 9000
int i_max_steps = 70;//Más pasos más lentitud.
int i_curr_steps = 33;//Número máximo de frames de la animación actual.
typedef struct _frame
{
	//Variables para guardar movimiento en keyframes.
	float MovimientoX_;
	float MovimientoY_;
	float MovimientoZ_;
	float MovimientoXInc_;		
	float MovimientoYInc_;	
	float MovimientoZInc_;
	float Giro_;
	float GiroInc_;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 33;//Número máximo de frames de la animación actual.
bool play = false;
int playIndex = 0;

void resetElements(void)
{
	MovimientoX_ = KeyFrame[0].MovimientoX_;
	MovimientoY_ = KeyFrame[0].MovimientoY_;
	MovimientoZ_ = KeyFrame[0].MovimientoZ_;
	Giro_ = KeyFrame[0].Giro_;
}

void interpolation(void)
{
	KeyFrame[playIndex].MovimientoXInc_ = (KeyFrame[playIndex + 1].MovimientoX_ - KeyFrame[playIndex].MovimientoX_) / i_max_steps;
	KeyFrame[playIndex].MovimientoYInc_ = (KeyFrame[playIndex + 1].MovimientoY_ - KeyFrame[playIndex].MovimientoY_) / i_max_steps;
	KeyFrame[playIndex].MovimientoZInc_ = (KeyFrame[playIndex + 1].MovimientoZ_ - KeyFrame[playIndex].MovimientoZ_) / i_max_steps;
	KeyFrame[playIndex].GiroInc_ = (KeyFrame[playIndex + 1].Giro_ - KeyFrame[playIndex].Giro_) / i_max_steps;
}

void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps)
		{
			playIndex++;
			printf("\n--------------------------------------------------");
			printf("\nFrame en reproduccion(playindex): %d.", playIndex);

			printf("\nKeyFrame[%d].MovimientoX_= %ff;", playIndex, MovimientoX_);
			printf("\nKeyFrame[%d].MovimientoY_= %ff;", playIndex, MovimientoY_);
			printf("\nKeyFrame[%d].MovimientoZ_= %ff;", playIndex, MovimientoZ_);
			printf("\nKeyFrame[%d].Giro_= %ff;\n", playIndex, Giro_);
			printf("\n--------------------------------------------------");

			if (playIndex > FrameIndex - 2)
			{
				printf("\n--------------------------------------------------");
				printf("\nNumero de frames finales(Frame index)= %d.", FrameIndex);
				printf("\nFin de animacion.\n");
				printf("\n0 para resetear animacion y espacio para reproducirla.");
				printf("\n--------------------------------------------------");
				playIndex = 0;
				play = false;
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
			MovimientoX_ += KeyFrame[playIndex].MovimientoXInc_;
			MovimientoY_ += KeyFrame[playIndex].MovimientoYInc_;
			MovimientoZ_ += KeyFrame[playIndex].MovimientoZInc_;
			Giro_ += KeyFrame[playIndex].GiroInc_;
			i_curr_steps++;
		}

	}
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	
	//Definimos nuestras variables con las que se hará el ciclo de día y noche
	movySun = -1.0f; //Aquí la luz siempre va a iniciar apunto a -1 en Y 
	movSun = -1.0f;
	movSunOffset = 0.0002f; //Declaramos el tiempo en el que queremos que vaya avanzando la luz
	luz = 1.0f;
	dia = true;
	
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	pisoTexture = Texture("Textures/cesped.png");
	pisoTexture.LoadTextureA();
	//-------Modelo PORSCHE.-------
	PORSCHE_M = Model();
	PORSCHE_M.LoadModel("Models/PORSCHESINLLANTAS_.obj");
	PORSCHELLANTA_M = Model();
	PORSCHELLANTA_M.LoadModel("Models/PORSCHELLANTA_1.obj");
	//-------Modelo esfera 1 estrella.-------
	Model ESFERA1ESTRELLA_M = Model();
	ESFERA1ESTRELLA_M.LoadModel("Models/1 estrella_.obj");
	//-------Modelo faro público.-------
	Model FAROPUBLICO_M = Model();
	FAROPUBLICO_M.LoadModel("Models/Faro público_2.obj");
	//-------Modelo peralte.-------
	Model PERALTE_M = Model();
	PERALTE_M.LoadModel("Models/Peralte_.obj");
	//Modelo Casa de Kaiosama
	Model Casa_Kaio = Model();
	Casa_Kaio.LoadModel("Models/Casa_Kaio.obj");
	//Maquina expendedora
	Model MaquinaExpendedora = Model();
	MaquinaExpendedora.LoadModel("Models/MaquinaExpendedora.obj");
	//Ring ML
	Model Ring_ML = Model();
	Ring_ML.LoadModel("Models/Ring_ML.obj");
	//Lata
	Model Lata = Model();
	Lata.LoadModel("Models/Lata.obj");
	//Nube voladora
	Model Nube_Goku = Model();
	Nube_Goku.LoadModel("Models/Nube_Goku.obj");
	//-------Modelo Autopista.-------
	Model AUTOPISTA_M = Model();
	AUTOPISTA_M.LoadModel("Models/Autopista_.obj");
	
	//Skybox
	std::vector<std::string> skyboxFaces;
	std::vector<std::string> skyboxFaces2;

	skyboxFaces.push_back("Textures/Skybox/Dia_derecha.jpg");
	skyboxFaces.push_back("Textures/Skybox/Dia_izquierda.jpg");
	skyboxFaces.push_back("Textures/Skybox/Dia_abajo.jpg");
	skyboxFaces.push_back("Textures/Skybox/Dia_arriba.jpg");
	skyboxFaces.push_back("Textures/Skybox/Dia_atras.jpg");
	skyboxFaces.push_back("Textures/Skybox/Dia_frente.jpg");
	
	//Skybox de noche
	skyboxFaces2.push_back("Textures/Skybox/Noche_izquierda.jpg");
	skyboxFaces2.push_back("Textures/Skybox/Noche_derecha.jpg");
	skyboxFaces2.push_back("Textures/Skybox/Noche_abajo.jpg");
	skyboxFaces2.push_back("Textures/Skybox/Noche_arriba.jpg");
	skyboxFaces2.push_back("Textures/Skybox/Noche_atras.jpg");
	skyboxFaces2.push_back("Textures/Skybox/Noche_frente.jpg");
	
	skybox = Skybox(skyboxFaces);// Ejecutar skybox de dia
	skybox2 = Skybox(skyboxFaces2); // Ejecutar skybox de noche


	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,
		1.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;
	/*
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;
	*/
	//-------Luz de esfera del dragón.-------
	spotLights[0] = SpotLight(1.0f, 0.4f, 0.0f,//Color: Naranja.
		0.5f, 1.0f,//Concentración.
		-2.0f, 5.0f, -50.2f,//Posición normal.
		1.0f, -5.0f, 0.0f,//Profundidad normal.
		1.0f, 0.0f, 0.0f,//Cercanía.
		50.0f);//Ancho.
	spotLightCount++;
	//-------Luz faro público.-------
	spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,//Color: Magenta.
		0.5f, 1.0f,//Concentración.
		0.0f, 5.0f, 50.2f,//Posición normal.
		0.0f, -5.0f, 0.0f,//Profundidad normal.
		1.0f, 0.0f, 0.0f,//Cercanía.
		89.0f);//Ancho.
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	//-------Animación keyframes.-------.
	KeyFrame[0].MovimientoX_ = 0.000000f;
	KeyFrame[0].MovimientoY_ = 0.000000f;
	KeyFrame[0].MovimientoZ_ = 0.000000f;
	KeyFrame[0].Giro_ = 0.000000f;

	KeyFrame[1].MovimientoX_ = 0.000000f;
	KeyFrame[1].MovimientoY_ = 4.000000f;
	KeyFrame[1].MovimientoZ_ = 0.000000f;
	KeyFrame[1].Giro_ = 0.000000f;

	KeyFrame[2].MovimientoX_ = 8.000000f;
	KeyFrame[2].MovimientoY_ = 0.000000f;
	KeyFrame[2].MovimientoZ_ = 0.000000f;
	KeyFrame[2].Giro_ = 0.000000f;

	KeyFrame[3].MovimientoX_ = 16.000000f;
	KeyFrame[3].MovimientoY_ = -4.000000f;
	KeyFrame[3].MovimientoZ_ = 0.000000f;
	KeyFrame[3].Giro_ = 0.000000f;

	KeyFrame[4].MovimientoX_ = 24.000000f;
	KeyFrame[4].MovimientoY_ = 0.000000f;
	KeyFrame[4].MovimientoZ_ = 0.000000f;
	KeyFrame[4].Giro_ = 0.000000f;

	KeyFrame[5].MovimientoX_ = 32.000000f;
	KeyFrame[5].MovimientoY_ = 4.000000f;
	KeyFrame[5].MovimientoZ_ = 0.000000f;
	KeyFrame[5].Giro_ = 0.000000f;

	KeyFrame[6].MovimientoX_ = 40.000000f;
	KeyFrame[6].MovimientoY_ = 0.000000f;
	KeyFrame[6].MovimientoZ_ = 0.000000f;
	KeyFrame[6].Giro_ = 0.000000f;

	KeyFrame[7].MovimientoX_ = 48.000000f;
	KeyFrame[7].MovimientoY_ = -4.000000f;
	KeyFrame[7].MovimientoZ_ = 0.000000f;
	KeyFrame[7].Giro_ = 0.000000f;

	KeyFrame[8].MovimientoX_ = 56.000000f;
	KeyFrame[8].MovimientoY_ = 0.000000f;
	KeyFrame[8].MovimientoZ_ = 0.000000f;
	KeyFrame[8].Giro_ = 0.000000f;

	KeyFrame[9].MovimientoX_ = 64.000000f;
	KeyFrame[9].MovimientoY_ = 4.000000f;
	KeyFrame[9].MovimientoZ_ = 0.000000f;
	KeyFrame[9].Giro_ = 0.000000f;

	KeyFrame[10].MovimientoX_ = 72.000000f;
	KeyFrame[10].MovimientoY_ = 0.000000f;
	KeyFrame[10].MovimientoZ_ = 0.000000f;
	KeyFrame[10].Giro_ = 0.000000f;

	KeyFrame[11].MovimientoX_ = 80.000000f;
	KeyFrame[11].MovimientoY_ = -4.000000f;
	KeyFrame[11].MovimientoZ_ = 0.000000f;
	KeyFrame[11].Giro_ = 0.000000f;

	KeyFrame[12].MovimientoX_ = 88.000000f;
	KeyFrame[12].MovimientoY_ = 0.000000f;
	KeyFrame[12].MovimientoZ_ = 0.000000f;
	KeyFrame[12].Giro_ = 0.000000f;

	KeyFrame[13].MovimientoX_ = 96.000000f;
	KeyFrame[13].MovimientoY_ = 4.000000f;
	KeyFrame[13].MovimientoZ_ = 0.000000f;
	KeyFrame[13].Giro_ = 0.000000f;

	KeyFrame[14].MovimientoX_ = 104.000000f;
	KeyFrame[14].MovimientoY_ = 0.000000f;
	KeyFrame[14].MovimientoZ_ = 0.000000f;
	KeyFrame[14].Giro_ = 0.000000f;

	KeyFrame[15].MovimientoX_ = 112.000000f;
	KeyFrame[15].MovimientoY_ = 0.000000f;
	KeyFrame[15].MovimientoZ_ = 0.000000f;
	KeyFrame[15].Giro_ = 0.000000f;

	GLfloat now = glfwGetTime();
	deltaTime = now - lastTime;
	lastTime = now;

	//Loop mientras no se cierra la ventana.
	while (!mainWindow.getShouldClose())
	{
		now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime = (now - lastTime) * 270;
		lastTime = now;
		
		//Realizamos nuestro ciclo de día y noche dentro de whilec con duración de 1 minuto con 10 segundos.
		if (dia == true)
		{
			if (movSun < 1.0f)
			{
				movSun += movSunOffset;

			}
			else
			{
				dia = false;
				movySun = 1.0f;
				luz = 0.2f;
				skybox = Skybox(skyboxFaces2);
				pointLightCount = 12;
				spotLightCount = 7;
			}
		}
		else
		{
			if (movSun > -1.0f)
			{
				movSun -= movSunOffset;
			
			}
			else
			{
				dia = true;
				movySun = -1.0f;
				luz = 1.0f;
				skybox = Skybox(skyboxFaces);
				pointLightCount = 0;
				spotLightCount = 0;
			}
		}
		
		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Para keyframes.
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//--------------Alternar Skybox-----------
		/*while (true)
		{

		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		std::this_thread::sleep_for(std::chrono::seconds(20)); // Esperar 20 segundos

		skybox2.DrawSkybox(camera.calculateViewMatrix(), projection); // Ejecutar skybox de noche

		std::this_thread::sleep_for(std::chrono::seconds(20)); // Esperar 20 segundos
		}
		*/
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		/*
		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		*/
		
		mainLight = DirectionalLight(luz, luz, luz,
		0.5f, 0.3f,
		0.0f, movySun, movSun);
		
		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		
		//Modelo ----Casa de Kaiosama--------
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-120.0f, -2.0f, -85.0f)); //ubicación del modelo
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Casa_Kaio.RenderModel();

		//Maquina expendedora
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-120.0f, -2.0f, -25.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MaquinaExpendedora.RenderModel();

		//Lata
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, 7.0f, 4.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lata.RenderModel();

		//Ring ML
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, -2.0f, 120.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ring_ML.RenderModel();

		//Nube voladora
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 110.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Nube_Goku.RenderModel();


		//-------Modelo PORSCHE.-------
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -1.0f, 100.0f));
		model = glm::translate(model, glm::vec3(0.0f + MovimientoX_, -0.9f + MovimientoY_, 327.0f + MovimientoZ_));
		model = glm::scale(model, glm::vec3(1.4f, 1.4f, 1.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, Giro_ * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PORSCHE_M.RenderModel();
		Rotate_ += RotateOffset_ * deltaTime;
		//-------Llanta superior derecha.-------
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, 2.7f, -11.0f));
		model = glm::rotate(model, -Rotate_ * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PORSCHELLANTA_M.RenderModel();
		//-------Llanta inferior derecha.-------
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, 2.7f, 10.7f));
		model = glm::rotate(model, -Rotate_ * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PORSCHELLANTA_M.RenderModel();
		//-------Llanta superior izquierda.-------
		model = modelaux;
		model = glm::translate(model, glm::vec3(-7.0f, 2.7f, -10.7f));
		model = glm::rotate(model, -Rotate_ * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PORSCHELLANTA_M.RenderModel();
		//-------Llanta inferior izquierda.-------
		model = modelaux;
		model = glm::translate(model, glm::vec3(-7.0f, 2.7f, 11.0f));
		model = glm::rotate(model, -Rotate_ * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PORSCHELLANTA_M.RenderModel();

		//-------Modelo esfera 1 estrella.-------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -50.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ESFERA1ESTRELLA_M.RenderModel();

		//-------Modelo faro público.-------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 50.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		FAROPUBLICO_M.RenderModel();
		
		//-------Circuito cerrado.-------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f, -1.5f, 327.0f));//X: 54.
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(57.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(111.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(165.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(219.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(273.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		//Esquina superior derecha.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(291.0f, -2.0f, 361.5f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PERALTE_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, 273.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, 219.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, 165.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, 111.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, 57.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, 3.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, -50.5f));//Z: 53
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, -103.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, -156.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, -209.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, -262.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(327.0f, -1.5f, -273.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		//Esquina superior izquierda.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(361.3f, -2.0f, -291.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PERALTE_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(273.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(219.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(165.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(111.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(57.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-51.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-159.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-213.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-267.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-273.0f, -1.5f, -327.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		//Esquina inferior izquierda.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-291.0f, -2.0f, -361.3f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PERALTE_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, -273.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, -220.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, -167.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, -114.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, -61.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, -8.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, 45.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, 98.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, 151.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, 204.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, 257.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-326.7f, -1.5f, 273.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		//Esquina inferior derecha.
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-361.0f, -2.0f, 291.2f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PERALTE_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-51.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-105.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-159.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-213.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-267.0f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-272.7f, -1.5f, 327.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AUTOPISTA_M.RenderModel();
		
		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}

void inputKeyframes(bool* keys)
{
	//Espacio reproduce de nuevo animación.
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				habilitaranimacion = 0;
			}
			else
			{
				play = false;
			}
		}
	}
	//0 resetea animación.
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}
}
