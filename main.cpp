#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <stack>

#include "Sphere.cpp"
#include "Utils.h"

using namespace std;

#define numVAOs 1
#define numVBOs 5

float cameraX, cameraY, cameraZ, modLocX, modLocY, modLocZ;
GLuint renderingProgram, renderingProgramCubeMap, vao[numVAOs], vbo[numVBOs];

glm::vec3 lightLoc = glm::vec3(0.0f, 0.0f, 0.0f); /////
float amt = 0.0f, quad_data[1080];

GLuint earthTexture;
GLuint moonTexture; 
GLuint sunTexture;
GLuint mercuryTexture;
GLuint venusTexture;
GLuint marsTexture;
GLuint jupiterTexture;
GLuint saturnTexture;
GLuint neptuneTexture;
GLuint uranusTexture;
GLuint skyboxTexture;

// variable allocation for display
GLuint vLoc, mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc; /////
int width, height;
float aspect, timeFactor;

glm::mat4 pMat, vMat, vMat1, mMat, mvMat, invTrMat, rMat;
glm::vec3 currentLightPos, transformed;
float lightPos[3]; //

stack<glm::mat4> mvStack;
Sphere mySphere = Sphere(48);

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

// white light
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// silver material
float* matAmb = silverAmbient();
float* matDif = silverDiffuse();
float* matSpe = silverSpecular();
float matShi = silverShininess();

void installLights( glm::mat4 vMatrix ) {
	transformed = glm::vec3( vMatrix * glm::vec4(currentLightPos, 1.0) );
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}

void setupVertices(void) {
			
	// para el cubo que servirá como fondo
	float cubeVertexPositions[108] =
	{	-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};

	// Para crear los cuerpos del anillo
	int k = 0;
	for(float theta = 0.0; theta <= 360.0; theta += 1.0){
	      quad_data[k] = 4*cos(toRadians(theta));
	      k++;
	      quad_data[k] = 4*sin(toRadians(theta));
	      k++;
	      quad_data[k] = 0;
	      k++;
	}

	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;


	int numIndices = mySphere.getNumIndices();
	for (int i = 0; i < numIndices; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);	
					
}

void init(GLFWwindow *window) {
	renderingProgramCubeMap = createShaderProgram("src/vertCShader.glsl", "src/fragCShader.glsl");
	renderingProgram = createShaderProgram("src/vertShader.glsl", "src/fragShader.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float) width / (float) height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians == 60 degrees

	// position the camera further down the positive Z axis (to see all of the cubes)
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 100.0f;
	modLocX = 0.0f; modLocY = 0.0f; modLocZ = -0.5f;

	setupVertices();
	
	earthTexture = loadTexture("images/earth.png");
	marsTexture = loadTexture("images/mars.png");
	mercuryTexture = loadTexture("images/mercury.png");
	moonTexture = loadTexture("images/moon.png");
	sunTexture = loadTexture("images/sun.jpg");
	venusTexture = loadTexture("images/venus.png");
	jupiterTexture = loadTexture("images/jupiter.png");
	saturnTexture = loadTexture("images/saturn.png");
	uranusTexture = loadTexture("images/uranus.png");
	neptuneTexture = loadTexture("images/neptune.png");

	skyboxTexture = loadSkybox("images/skybox"); 

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

// repeatedly
void display(GLFWwindow *window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// skybox
	glUseProgram(renderingProgramCubeMap);

	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

	projLoc = glGetUniformLocation(renderingProgramCubeMap, "p_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	// PLANETAS
	glUseProgram(renderingProgram);

	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");   ///

	// push view matrix onto the stack
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(modLocX, modLocY, modLocZ));
	mMat *= glm::rotate(mMat, toRadians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	amt += 2.5f;
	rMat = glm::rotate(glm::mat4(1.0f), toRadians(amt), glm::vec3(0.0f, 0.0f, 1.0f));
	currentLightPos = glm::vec3(rMat * glm::vec4(currentLightPos, 1.0f));

	vMat1 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	installLights(vMat1);

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	
	//mvStack.push(vMat);
	mvStack.push(mvMat);

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	// Sol coordenada 
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f));  // posicion del sol
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 2*((float) currentTime),
			glm::vec3(0.0f, 1.0f, 0.0f));  // rotacion del sol
	///		
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(3.0f, 3.0f, 3.0f));  // tamaño del sol
       ///
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 1026);
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();
	
	// mercurio coordenada
	mvStack.push(mvStack.top());  // mvMat of sun
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(sin((float) currentTime) * 5.0, 0.0f,
					cos((float) currentTime) * 5.0));  // posicion de mercurio
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 1.0, 0.0));  // rotacion de mercurio
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(0.8f, 0.8f, 0.8f));  // tamaño de mercurio		

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mercuryTexture);
	
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 1026);   //36
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();
	
	// venus coordenada
	mvStack.push(mvStack.top());  // mvMat of sun
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(sin((float) currentTime + (3.1416/2)) * 10.0, 0.0f,
					cos((float) currentTime + (3.1416/2)) * 10.0));  // posicion de venus
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 1.0, 0.0));  // rotacion de venus
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(2.0f, 2.0f, 2.0f));  // tamaños de venus		

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, venusTexture);
	
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 1026);   //36
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();
	
	// marte coordenada
	mvStack.push(mvStack.top());  // mvMat of sun
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(sin((float) currentTime + (3.1416/4)) * 20.0, 0.0f,
					cos((float) currentTime + (3.1416/4)) * 20.0));  // posicion de marte
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 1.0, 0.0));  //rotacion de marte
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(1.5f, 1.5f, 1.5f));  // tamaño de marte		

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, marsTexture);
	
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 1026);   //36
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();
	
	// jupiter coordenada
	mvStack.push(mvStack.top());  // mvMat of sun
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(sin(((float) currentTime)/2) * 25.0, 0.0f,
					cos(((float) currentTime)/2) * 25.0));  // posicion de jupiter
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 1.0, 0.0));  // rotacion de jupiter
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(2.5f, 2.5f, 2.5f));  // tamaño de jupiter		

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, jupiterTexture);
	
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 1026);   //36
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();
	
	// saturno coordenada
	mvStack.push(mvStack.top());  // mvMat of sun
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(sin(((float) currentTime)/4) * 35.0, 0.0f,
					cos(((float) currentTime)/4) * 35.0));  // posicion de saturno 
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 1.0, 0.0));  // rotacion de saturno
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(2.5f, 2.5f, 2.5f));  // tamaño de saturno		

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, saturnTexture);
	
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 1026);   //36
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	//mvStack.pop();

	// anillos coordenada
	for(float i = 1.0f; i<1.5f; i=i+0.02f){
		mvStack.push(mvStack.top());  // mvMat of sun
		mvStack.top() *= glm::translate(glm::mat4(1.0f),
				glm::vec3(0.0f, 0.0f, 0.0f));  // posicion de saturno 
		mvStack.push(mvStack.top());  // duplicating
		mvStack.top() *= glm::rotate(glm::mat4(1.0f), toRadians(90.0f),
				glm::vec3(1.0, 0.0, 0.0));  // rotacion de saturno
		mvStack.top() *= glm::scale(glm::mat4(1.0f),
				glm::vec3(i, i, i));  // tamaño de saturno		

		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
		glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		
		////
		glDrawArrays(GL_LINES, 0, 360);
		mvStack.pop();
		mvStack.pop();
	}
	mvStack.pop();
	
	// urano coordenada
	mvStack.push(mvStack.top());  // mvMat of sun
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(sin(((float) currentTime)/8) * 45.0, 0.0f,
					cos(((float) currentTime)/8) * 45.0));  // posicion de urano 
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 1.0, 0.0));  // rotacion de urano
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(1.5f, 1.5f, 1.5f));  // tamaño de urano		

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, uranusTexture);
	
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 1026);   //36
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();
	
	// neptuno coordenada
	mvStack.push(mvStack.top());  // mvMat of sun
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(sin(((float) currentTime)/12) * 50.0, 0.0f,
					cos(((float) currentTime)/12) * 50.0));  // posicion de neptuno
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 1.0, 0.0));  // planet rotation
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(1.5f, 1.5f, 1.5f));  // make the moon smaller		

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, neptuneTexture);
	

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();

	// tierra coordenada
	mvStack.push(mvStack.top());  // mvMat of sun
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(sin((float) currentTime + 3.1416) * 16.0, 0.0f,
					cos((float) currentTime + 3.1416) * 16.0));  // planet position
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 1.0, 0.0));  // planet rotation
	
	mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(2.0f, 2.0f, 2.0f));  // make the moon smaller		

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 1026);   //36
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();

	// luna coordenada 
	mvStack.push(mvStack.top());  // mvMat of planet
	mvStack.top() *= glm::translate(glm::mat4(1.0f),
			glm::vec3(0.0f, sin((float) currentTime) * 4.0,
					cos((float) currentTime) * 4.0));  // moon position
	mvStack.push(mvStack.top());  // duplicating
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime,
			glm::vec3(0.0, 0.0, 1.0));  // moon rotation

       mvStack.top() *= glm::scale(glm::mat4(1.0f),
			glm::vec3(0.25f, 0.25f, 0.25f));  // make the moon smaller
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	////
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	/////
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, moonTexture);
	
	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
    aspect = (float)newWidth / (float)newHeight;
    glViewport(0, 0, newWidth, newHeight);
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);     //
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); 	// Resizable option.

	GLFWwindow *window = glfwCreateWindow(1400, 1050, "Sistema Solar", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		exit(EXIT_FAILURE);
	}
	glfwSwapInterval(1);
	init(window);

	// the rate at which display() is called is referred to as the frame rate
	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
