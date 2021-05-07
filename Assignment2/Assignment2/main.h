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
void cubeCreator(point4 *pointsCube, color4 *color) {
	point4 verticesCube[8];

	verticesCube[0] = point4(-0.5, 0.5, -0.5, 1.0);
	verticesCube[1] = point4(-0.5, 0.5, 0.5, 1.0);
	verticesCube[2] = point4(0.5, 0.5, 0.5, 1.0);
	verticesCube[3] = point4(0.5, 0.5, -0.5, 1.0);
	verticesCube[4] = point4(-0.5, -0.5, -0.5, 1.0);
	verticesCube[5] = point4(-0.5, -0.5, 0.5, 1.0);
	verticesCube[6] = point4(0.5, -0.5, 0.5, 1.0);
	verticesCube[7] = point4(0.5, -0.5, -0.5, 1.0);

	for (int i = 0 ; i < 6 ; i++) {
		color[i] = color4(1.0,0.0,0.0,1.0);     // red
        color[i+6] = color4(1.0,1.0,0.0,1.0);   // yellow
		color[i+12] = color4(0.0,1.0,0.0,1.0);  // green
		color[i+18] = color4(0.0,1.0,1.0,1.0);  // cyan
		color[i+24] = color4(0.0,0.0,1.0,1.0);  // blue
		color[i+30] = color4(1.0,0.0,1.0,1.0);  // magenta
	}

	pointsCube[0] = verticesCube[0];
	pointsCube[1] = verticesCube[1];
	pointsCube[2] = verticesCube[2];
	pointsCube[3] = verticesCube[2];
	pointsCube[4] = verticesCube[3];
	pointsCube[5] = verticesCube[0];

	pointsCube[6] = verticesCube[4];
	pointsCube[7] = verticesCube[5];
	pointsCube[8] = verticesCube[6];
	pointsCube[9] = verticesCube[6];
	pointsCube[10] = verticesCube[7];
	pointsCube[11] = verticesCube[4];

	pointsCube[12] = verticesCube[0];
	pointsCube[13] = verticesCube[3];
	pointsCube[14] = verticesCube[7];
	pointsCube[15] = verticesCube[7];
	pointsCube[16] = verticesCube[4];
	pointsCube[17] = verticesCube[0];

	pointsCube[18] = verticesCube[1];
	pointsCube[19] = verticesCube[2];
	pointsCube[20] = verticesCube[6];
	pointsCube[21] = verticesCube[6];
	pointsCube[22] = verticesCube[5];
	pointsCube[23] = verticesCube[1];

	pointsCube[24] = verticesCube[0];
	pointsCube[25] = verticesCube[1];
	pointsCube[26] = verticesCube[5];
	pointsCube[27] = verticesCube[5];
	pointsCube[28] = verticesCube[4];
	pointsCube[29] = verticesCube[0];

	pointsCube[30] = verticesCube[3];
	pointsCube[31] = verticesCube[2];
	pointsCube[32] = verticesCube[6];
	pointsCube[33] = verticesCube[6];
	pointsCube[34] = verticesCube[7];
	pointsCube[35] = verticesCube[3];
}


