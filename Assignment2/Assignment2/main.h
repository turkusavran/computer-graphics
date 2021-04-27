#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <vector>
#include "Angel.h"

using namespace std;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

// Rubik's Cube specifications
const int numCubes = 8;
const int cubeFaces = 6;
const int faceVertices = 6;
const int numOneCubeVertices = faceVertices * cubeFaces;
const int numCubeVertices = numOneCubeVertices * numCubes;

// Animation Speed */
const int animationSpeed = 10;

// Default position of the cube
double rotateX = 135.0;
double rotateZ = -45.0;
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

// Cube rotation-level variables; specify the current orientation of the cube
int rotationAxis = axisY; 
int currentBlock = 1;
int rotatePhase = 0;

// Uniform location
int ModelView;
int Projection;
int edge;
int currentCube;
int vRotation[numCubes];

// Variable for the shader
int selectCube;
int selectFace;
int cubeColor[numCubes];

int cubeSelector(int x, int y, int button);
int performRotation(int currX, int currY, int currZ, int nextX, int nextY, int nextZ, int key);
int rotateAlong(int axis, int kj, int lj, int button);
void cubeCreator(point4 *cube, color4 *color);
void cubeTranslation(point4 *cube, vec3 displacement);
void rubiksCube(void);
void randomInitialization(int numRotations);
void rubiksCubeRotation(int direction, int block, int axis);

// MARK: - Cube Vertices

void cubeCreator(point4 *cube, color4 *color) {
    /*
    const int NumVerticesCube = 36;
    point4 pointsCube[NumVerticesCube];
    color4 colors[NumVerticesCube];

    // Vertices of a unit cube
    point4 verticesCube[8] = {
        point4( -0.5, -0.5,  0.5, 1.0 ),
        point4( -0.5,  0.5,  0.5, 1.0 ),
        point4(  0.5,  0.5,  0.5, 1.0 ),
        point4(  0.5, -0.5,  0.5, 1.0 ),
        point4( -0.5, -0.5, -0.5, 1.0 ),
        point4( -0.5,  0.5, -0.5, 1.0 ),
        point4(  0.5,  0.5, -0.5, 1.0 ),
        point4(  0.5, -0.5, -0.5, 1.0 )
    };

    // RGBA colors
    color4 vertex_colors[8] = {
        color4( 0.0, 0.0, 0.0, 1.0 ),  // black
        color4( 1.0, 0.0, 0.0, 1.0 ),  // red
        color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
        color4( 0.0, 1.0, 0.0, 1.0 ),  // green
        color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
        color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
        color4( 1.0, 1.0, 1.0, 1.0 ),  // white
        color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
    };

    // Array of rotation angles (in degrees) for each coordinate axis
    enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
    int Axis = Xaxis;

    GLfloat Theta[NumAxes] = { 0.0, 0.0, 0.0 };

    // Model-view, projection and color matrices uniform location
    GLuint ModelView, Projection, Color;
    GLuint vao[3];

    // quad generates two triangles for each face
    int Index = 0;
    void quad( int a, int b, int c, int d ) {
        pointsCube[Index] = verticesCube[a]; Index++;
        pointsCube[Index] = verticesCube[b]; Index++;
        pointsCube[Index] = verticesCube[c]; Index++;
        pointsCube[Index] = verticesCube[a]; Index++;
        pointsCube[Index] = verticesCube[c]; Index++;
        pointsCube[Index] = verticesCube[d]; Index++;
    }

    // generate 12 triangles: 36 vertices
    void cube() {
        quad( 1, 0, 3, 2 );
        quad( 2, 3, 7, 6 );
        quad( 3, 0, 4, 7 );
        quad( 6, 5, 1, 2 );
        quad( 4, 5, 6, 7 );
        quad( 5, 4, 0, 1 );
    }
*/
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
