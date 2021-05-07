#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <vector>
#include "Angel.h"

using namespace std;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// Rubik's Cube variables
const int numCubes = 8;
const int cubeFaces = 6;
const int faceVertices = 6;
const int numOneCubeVertices = faceVertices * cubeFaces;
const int numCubeVertices = numOneCubeVertices * numCubes;

// Animation speed
const int animationSpeed = 10;

// Default view aspect of the cube
double aspectX = 135.0;
double aspectZ = -45.0;
double scaleFactor = 1.0;

enum { axisX = 0, axisY = 1, axisZ = 2, numAxis = 3 };
float Theta[numAxis] = { 0.0, 0.0, 0.0 };

mat4 rotationMatrix[numCubes];
color4 colorSelector[numCubes];
color4 colors[numCubeVertices];
point4 vertices[numCubeVertices];

// Cube position
int currentCubePos[2][2][2];
int nextCubePos[2][2][2];

// Cube rotation variables
int rotationAxis = axisY; 
int currentBlock = 1;
int rotateAngle = 0;

// Uniform location
int ModelView;
int Projection;
int edge;
int currentCube;
int vRotation[numCubes];

// Variables for the shader
int selectedCube;
int selectedFace;
int cubeColor[numCubes];

// Function definitions
int cubeSelector(int a, int b, int key);
int performRotation(int currentX, int currentY, int currentZ, int nextX, int nextY, int nextZ, int key);
int setParameters(int axis, int firstAxis, int secondAxis, int key);
void cubeCreator(point4 *cube, color4 *color);
void rubiksCube();
void randomizeCube(int a);
void setNextPositions(int direction, int subcube, int axis);
void quad( point4 *cube, point4 *vertices, int a, int b, int c, int d );

// MARK: - Cube Vertices
void cubeCreator(point4 *cube, color4 *color) {
	point4 vertices[8];

	vertices[0] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[6] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	for (int i = 0 ; i < 6 ; i++) {
		color[i] = color4(1.0,0.0,0.0,1.0);     // red
        color[i+6] = color4(1.0,1.0,0.0,1.0);   // yellow
		color[i+12] = color4(0.0,1.0,0.0,1.0);  // green
		color[i+18] = color4(0.0,1.0,1.0,1.0);  // cyan
		color[i+24] = color4(0.0,0.0,1.0,1.0);  // blue
		color[i+30] = color4(1.0,0.0,1.0,1.0);  // magenta
	}

	cube[0] = vertices[0];
	cube[1] = vertices[1];
	cube[2] = vertices[2];
	cube[3] = vertices[2];
	cube[4] = vertices[3];
	cube[5] = vertices[0];

	cube[6] = vertices[4];
	cube[7] = vertices[5];
	cube[8] = vertices[6];
	cube[9] = vertices[6];
	cube[10] = vertices[7];
	cube[11] = vertices[4];

	cube[12] = vertices[0];
	cube[13] = vertices[3];
	cube[14] = vertices[7];
	cube[15] = vertices[7];
	cube[16] = vertices[4];
	cube[17] = vertices[0];

	cube[18] = vertices[1];
	cube[19] = vertices[2];
	cube[20] = vertices[6];
	cube[21] = vertices[6];
	cube[22] = vertices[5];
	cube[23] = vertices[1];

	cube[24] = vertices[0];
	cube[25] = vertices[1];
	cube[26] = vertices[5];
	cube[27] = vertices[5];
	cube[28] = vertices[4];
	cube[29] = vertices[0];

	cube[30] = vertices[3];
	cube[31] = vertices[2];
	cube[32] = vertices[6];
	cube[33] = vertices[6];
	cube[34] = vertices[7];
	cube[35] = vertices[3];
}


