#include "BOX.h"
#include "auxiliar.h"
#include "PLANE.h"

#include <gl/glew.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdlib>

#define RAND_SEED 31415926
#define M_2PI 6.28318530718
#define M_PI 3.14159265359
#define SCREEN_SIZE 800,800

//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////

//VAO
unsigned int vao;

//VBO
unsigned int fbo;

//VBOs que forman parte del objeto
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO;
unsigned int triangleIndexVBO;

unsigned int planeVAO;
unsigned int planeVertexVBO;

///////////
//Forward-rendering
///////////
unsigned int forwardVShader;
unsigned int forwardFShader;
unsigned int forwardProgram;

//Atributos
int inPos;
int inColor;
int inNormal;
int inTexCoord;

//Matrices Uniform
int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;

//Identificadores de texturas Forward-rendering
unsigned int colorTexId;
unsigned int emiTexId;

//Texturas Uniform
int uColorTex;
int uEmiTex;

///////////
//Post-proceso
///////////
unsigned int postProccesVShader;
unsigned int postProccesFShader;
unsigned int postProccesProgram;

//Atributos
int inPosPP;

//Uniforms
int uLightPosPP;

//Identificadores de texturas Post-proceso
unsigned int colorBuffTexId;
unsigned int emiBuffTexId;
unsigned int depthBuffTexId;
unsigned int vertexBuffTexId;
unsigned int normalBuffTexId;

//Texturas Post-proceso
unsigned int uColorTexPP;
unsigned int uVertexTexPP;
unsigned int uNormalTexPP;
unsigned int uDepthTexPP;
unsigned int uEmiTexPP;

////////////////////
//Geometry
///////////////////

unsigned int geometryVShader;
unsigned int geometryGShader;
unsigned int geometryFShader;
unsigned int geometryProgram;

//Atributos
int inPosGeo;
int inNormalGeo;

//Matrices Uniform
int uModelViewMatGeo;
int uModelViewProjMatGeo;
int uNormalMatVGeo;

//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices
glm::mat4 proj = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 modelObject = glm::mat4(1.0f);
glm::mat4 modelLight = glm::mat4(1.0f);

//Variable del modelo
unsigned int nVertexIndex;

//Control de camara
float theta = 0.0f;
float phi = 0.0f;
bool moveCam;

//Variables near y far
float projNear, projFar;

//Posicion de la luz
glm::vec4 lightPos;

//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////

//Declaración de CB
void renderFunc();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

void renderObject();

//Funciones de inicialización y destrucción
void initContext(int argc, char** argv);
void initOGL();
void initShaderFw(const char* vname, const char* fname);
void initShaderPP(const char* vname, const char* fname);
void initShaderGeo(const char* vname, const char* gname, const char* fname);
void initObj();
void initPlane();
void initFBO();
void destroy();
void resizeFBO(unsigned int w, unsigned int h);

//Carga el shader indicado, devuele el ID del shader
GLuint loadShader(const char* fileName, GLenum type);

//Crea una textura, la configura, la sube a OpenGL,
//y devuelve el identificador de la textura
unsigned int loadTex(const char* fileName);

int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();
	//initShaderFw("../shaders/fwRendering.vert", "../shaders/fwRendering.frag");
	//initShaderPP("../shaders/postProcessing.vert", "../shaders/postProcessing.frag");
	initShaderGeo("../shaders/renderNormals.vert", "../shaders/renderNormals.geo", "../shaders/renderNormals.frag");

	initObj();
	initPlane();

	initFBO();

	glutMainLoop();

	destroy();

	return 0;
}

//////////////////////////////////////////
// Funciones auxiliares
void initContext(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_SIZE);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Prácticas PGATR");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	const GLubyte* oglVersion = glGetString(GL_VERSION);
	std::cout << "This system supports OpenGL Version: " << oglVersion << std::endl;

	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
}

void initOGL()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);

	//Inicializamos matriz de proyeccion
	projNear = 1.0f;
	projFar = 400.0f;
	proj = glm::perspective(glm::radians(60.0f), 1.0f, projNear, projFar);

	//Establecemos la posición de la luz
	lightPos = glm::vec4(0, 30, 0, 1);
	modelLight = glm::mat4(1);

	//Inicializamos la cámara
	view = glm::lookAt(glm::vec3(0, 0, 40), glm::vec3(0), glm::vec3(0, 1, 0));
	moveCam = false;
}

void destroy()
{
	glDetachShader(forwardProgram, forwardVShader);
	glDetachShader(forwardProgram, forwardFShader);
	glDeleteShader(forwardVShader);
	glDeleteShader(forwardFShader);
	glDeleteProgram(forwardProgram);

	glDetachShader(postProccesProgram, postProccesVShader);
	glDetachShader(postProccesProgram, postProccesFShader);
	glDeleteShader(postProccesVShader);
	glDeleteShader(postProccesFShader);
	glDeleteProgram(postProccesProgram);

	glDetachShader(geometryProgram, geometryVShader);
	glDetachShader(geometryProgram, geometryFShader);
	glDetachShader(geometryProgram, geometryGShader);
	glDeleteShader(geometryVShader);
	glDeleteShader(geometryFShader);
	glDeleteShader(geometryGShader);
	glDeleteProgram(geometryProgram);

	if (inPos != -1) glDeleteBuffers(1, &posVBO);
	if (inColor != -1) glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1) glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1) glDeleteBuffers(1, &texCoordVBO);
	glDeleteBuffers(1, &triangleIndexVBO);

	glDeleteVertexArrays(1, &vao);

	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);

	glDeleteBuffers(1, &planeVertexVBO);
	glDeleteVertexArrays(1, &planeVAO);

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &colorBuffTexId);
	glDeleteTextures(1, &depthBuffTexId);
	glDeleteTextures(1, &vertexBuffTexId);
	glDeleteTextures(1, &emiBuffTexId);
	glDeleteTextures(1, &normalBuffTexId);
}

void initShaderFw(const char* vname, const char* fname)
{
	forwardVShader = loadShader(vname, GL_VERTEX_SHADER);
	forwardFShader = loadShader(fname, GL_FRAGMENT_SHADER);

	forwardProgram = glCreateProgram();
	glAttachShader(forwardProgram, forwardVShader);
	glAttachShader(forwardProgram, forwardFShader);

	glBindAttribLocation(forwardProgram, 0, "inPos");
	glBindAttribLocation(forwardProgram, 1, "inColor");
	glBindAttribLocation(forwardProgram, 2, "inNormal");
	glBindAttribLocation(forwardProgram, 3, "inTexCoord");

	glLinkProgram(forwardProgram);

	int linked;
	glGetProgramiv(forwardProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(forwardProgram, GL_INFO_LOG_LENGTH, &logLen);

		char* logString = new char[logLen];
		glGetProgramInfoLog(forwardProgram, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;

		glDeleteProgram(forwardProgram);
		forwardProgram = 0;
		exit(-1);
	}

	uNormalMat = glGetUniformLocation(forwardProgram, "normal");
	uModelViewMat = glGetUniformLocation(forwardProgram, "modelView");
	uModelViewProjMat = glGetUniformLocation(forwardProgram, "modelViewProj");

	uColorTex = glGetUniformLocation(forwardProgram, "colorTex");
	uEmiTex = glGetUniformLocation(forwardProgram, "emiTex");

	inPos = glGetAttribLocation(forwardProgram, "inPos");
	inColor = glGetAttribLocation(forwardProgram, "inColor");
	inNormal = glGetAttribLocation(forwardProgram, "inNormal");
	inTexCoord = glGetAttribLocation(forwardProgram, "inTexCoord");
}

void initShaderGeo(const char* vname, const char* gname, const char* fname)
{
	geometryVShader = loadShader(vname, GL_VERTEX_SHADER);
	geometryGShader = loadShader(gname, GL_GEOMETRY_SHADER);
	geometryFShader = loadShader(fname, GL_FRAGMENT_SHADER);

	geometryProgram = glCreateProgram();
	glAttachShader(geometryProgram, geometryVShader);
	glAttachShader(geometryProgram, geometryGShader);
	glAttachShader(geometryProgram, geometryFShader);

	glBindAttribLocation(geometryProgram, 0, "inPos");
	glBindAttribLocation(geometryProgram, 1, "inNormal");

	glLinkProgram(geometryProgram);

	int linked;
	glGetProgramiv(geometryProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(geometryProgram, GL_INFO_LOG_LENGTH, &logLen);

		char* logString = new char[logLen];
		glGetProgramInfoLog(geometryProgram, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;

		glDeleteProgram(geometryProgram);
		geometryProgram = 0;
		exit(-1);
	}

	uNormalMatVGeo = glGetUniformLocation(geometryProgram, "normal");
	uModelViewMatGeo = glGetUniformLocation(geometryProgram, "modelView");
	uModelViewProjMatGeo = glGetUniformLocation(geometryProgram, "modelViewProj");

	inPosGeo = glGetAttribLocation(geometryProgram, "inPos");
	inNormalGeo = glGetAttribLocation(geometryProgram, "inNormal");
}

void initShaderPP(const char* vname, const char* fname)
{
	postProccesVShader = loadShader(vname, GL_VERTEX_SHADER);
	postProccesFShader = loadShader(fname, GL_FRAGMENT_SHADER);

	postProccesProgram = glCreateProgram();
	glAttachShader(postProccesProgram, postProccesVShader);
	glAttachShader(postProccesProgram, postProccesFShader);

	glBindAttribLocation(postProccesProgram, 0, "inPos");

	glLinkProgram(postProccesProgram);
	int linked;
	glGetProgramiv(postProccesProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetProgramiv(postProccesProgram, GL_INFO_LOG_LENGTH, &logLen);
		char* logString = new char[logLen];
		glGetProgramInfoLog(postProccesProgram, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete logString;
		glDeleteProgram(postProccesProgram);
		postProccesProgram = 0;
		exit(-1);
	}

	inPosPP = glGetAttribLocation(postProccesProgram, "inPos");

	glUseProgram(postProccesProgram);

	//Establecimiento de los localizadores de las texturas/buffers de post-proceso
	uColorTexPP = glGetUniformLocation(postProccesProgram, "colorTex");
	if (uColorTexPP != -1)
		glUniform1i(uColorTexPP, 0);

	uVertexTexPP = glGetUniformLocation(postProccesProgram, "vertexTex");
	if (uVertexTexPP != -1)
		glUniform1i(uVertexTexPP, 1);

	uNormalTexPP = glGetUniformLocation(postProccesProgram, "normalTex");
	if (uNormalTexPP != -1)
		glUniform1i(uNormalTexPP, 2);

	uEmiTexPP = glGetUniformLocation(postProccesProgram, "emiTex");
	if (uEmiTexPP != -1)
		glUniform1i(uEmiTexPP, 3);

	uDepthTexPP = glGetUniformLocation(postProccesProgram, "depthTex");
	if (uDepthTexPP != -1)
		glUniform1i(uDepthTexPP, 4);

	uLightPosPP = glGetUniformLocation(postProccesProgram, "lpos");
}

void initObj()
{
	// Read our .obj file
	std::vector< glm::vec3 > vertexes;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	std::vector <unsigned int> indexes;

	bool res = loadOBJ("../models/teapot.obj", vertexes, uvs, normals, indexes);
	unsigned int nVertex = vertexes.size();
	nVertexIndex = indexes.size();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (inPos != -1)
	{
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, nVertex * sizeof(glm::vec3),
			&vertexes[0], GL_STATIC_DRAW);
		glVertexAttribPointer(inPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inPos);
	}

	/*
	if (inColor != -1)
	{
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(GL_ARRAY_BUFFER, nVertex * sizeof(float) * 3,
			cubeVertexColor, GL_STATIC_DRAW);
		glVertexAttribPointer(inColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inColor);
	}
	*/

	if (inNormal != -1)
	{
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, nVertex * sizeof(glm::vec3),
			&normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(inNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inNormal);
	}

	if (inTexCoord != -1)
	{
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, nVertex * sizeof(glm::vec2),
			&uvs[0], GL_STATIC_DRAW);
		glVertexAttribPointer(inTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(inTexCoord);
	}

	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		nVertexIndex * sizeof(unsigned int), &indexes[0],
		GL_STATIC_DRAW);

	modelObject = glm::mat4(1.0f);

	colorTexId = loadTex("../img/color2.png");
	emiTexId = loadTex("../img/emissive.png");
}

void initPlane()
{
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	glGenBuffers(1, &planeVertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertexVBO);

	glBufferData(GL_ARRAY_BUFFER, planeNVertex * sizeof(float) * 3,
		planeVertexPos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

GLuint loadShader(const char* fileName, GLenum type)
{
	unsigned int fileLen;
	char* source = loadStringFromFile(fileName, fileLen);

	//////////////////////////////////////////////
	//Creación y compilación del Shader
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1,
		(const GLchar**)&source, (const GLint*)&fileLen);
	glCompileShader(shader);
	delete[] source;

	//Comprobamos que se compilo bien
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		char* logString = new char[logLen];
		glGetShaderInfoLog(shader, logLen, NULL, logString);
		std::cout << "Error: " << logString << std::endl;
		delete[] logString;

		glDeleteShader(shader);
		exit(-1);
	}

	return shader;
}

unsigned int loadTex(const char* fileName)
{
	unsigned char* map;
	unsigned int w, h;
	map = loadTexture(fileName, w, h);

	if (!map)
	{
		std::cout << "Error cargando el fichero: "
			<< fileName << std::endl;
		exit(-1);
	}

	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, (GLvoid*)map);
	delete[] map;
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);

	return texId;
}

void renderFunc()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///////////
	//Forward-rendering
	///////////
	glUseProgram(forwardProgram);

	//Texturas del forward-rendering
	if (uColorTex != -1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexId);
		glUniform1i(uColorTex, 0);
	}

	if (uEmiTex != -1)
	{
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, emiTexId);
		glUniform1i(uEmiTex, 1);
	}

	//Dibujado de objeto
	renderObject();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//////////
	//Post-procesing
	///////////
	glUseProgram(postProccesProgram);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	//Establecimiento de los distintos bufferes para el post-procesado
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, colorBuffTexId);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, vertexBuffTexId);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, normalBuffTexId);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, emiBuffTexId);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, depthBuffTexId);

	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (uLightPosPP != -1)
	{
		glm::vec3 lpos = (view * modelLight) * lightPos;
		glUniform3fv(uLightPosPP, 1, &lpos[0]);
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(geometryProgram);

	renderObject();

	glutSwapBuffers();
}

void renderObject()
{
	glm::mat4 modelView = view * modelObject;
	glm::mat4 modelViewProj = proj * view * modelObject;
	glm::mat4 normal = glm::transpose(glm::inverse(modelView));

	if (uModelViewMat != -1)
		glUniformMatrix4fv(uModelViewMat, 1, GL_FALSE, &(modelView[0][0]));

	if (uModelViewProjMat != -1)
		glUniformMatrix4fv(uModelViewProjMat, 1, GL_FALSE, &(modelViewProj[0][0]));

	if (uNormalMat != -1)
		glUniformMatrix4fv(uNormalMat, 1, GL_FALSE, &(normal[0][0]));

	if (uModelViewMatGeo != -1)
		glUniformMatrix4fv(uModelViewMatGeo, 1, GL_FALSE, &(modelView[0][0]));

	if (uModelViewProjMatGeo != -1)
		glUniformMatrix4fv(uModelViewProjMatGeo, 1, GL_FALSE, &(modelViewProj[0][0]));

	if (uNormalMatVGeo != -1)
		glUniformMatrix4fv(uNormalMatVGeo, 1, GL_FALSE, &(normal[0][0]));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, nVertexIndex, GL_UNSIGNED_INT, (void*)0);
}

void resizeFunc(int width, int height)
{
	//Mantenimiento del aspect ratio
	glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(60.0f), float(width) / float(height), projNear, projFar);

	resizeFBO(width, height);

	glutPostRedisplay();
}

void idleFunc()
{
	if (moveCam)
	{
		if (phi < M_2PI)
		{
			phi += 0.02f;
			view = glm::rotate(view, 0.02f, glm::vec3(0, 1, 0));
		}
		else if (theta < M_2PI)
		{
			theta += 0.02f;
			view = glm::rotate(view, 0.02f, glm::vec3(1, 0, 0));
		}
		else
		{
			theta = 0.0f;
			phi = 0.0f;
		}
	}

	glutPostRedisplay();
}

void keyboardFunc(unsigned char key, int x, int y)
{
	glm::vec3 dir;
	switch (key)
	{
	case(' '):
		moveCam = !moveCam;
		break;
	case('a'):
	case('A'):
		dir = -view[3];
		view[3].x += dir.x * 0.1;
		view[3].y += dir.y * 0.1;
		view[3].z += dir.z * 0.1;
		break;
	case('s'):
	case('S'):
		dir = view[3];
		view[3].x += dir.x * 0.1;
		view[3].y += dir.y * 0.1;
		view[3].z += dir.z * 0.1;
		break;
	}
}

void mouseFunc(int button, int state, int x, int y)
{
}

void initFBO()
{
	glGenFramebuffers(1, &fbo);
	glGenTextures(1, &colorBuffTexId);
	glGenTextures(1, &depthBuffTexId);
	glGenTextures(1, &vertexBuffTexId);
	glGenTextures(1, &normalBuffTexId);
	glGenTextures(1, &emiBuffTexId);

	resizeFBO(SCREEN_SIZE);
}

void resizeFBO(unsigned int w, unsigned int h)
{
	//Inicialización de todos los buffers del post-procesado
	glBindTexture(GL_TEXTURE_2D, colorBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, depthBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, vertexBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, normalBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, emiBuffTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, vertexBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, normalBuffTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, emiBuffTexId, 0);

	const GLenum buffs[4] = { GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, buffs);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		std::cerr << "Error configurando el FBO" << std::endl;
		exit(-1);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}