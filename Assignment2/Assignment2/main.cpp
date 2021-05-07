// COMP410: Computer Graphics - Assignment 2
// Türkü Bengisu Savran

#define GL_SILENCE_DEPRECATION
#include "main.h"

// MARK: - Rubiks Cube

void rubiksCube() {
    int current = 0;
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    point4 *cube = new point4[numOneCubeVertices];
    color4 *color = new point4[numOneCubeVertices];
    
    // Create 8 sub cubes
    for (int i = 0 ; i <= 1 ; i++) {
        green = 0.0;
        red += 0.25;
        for (int j = 0; j <= 1 ; j++) {
            green += 0.25;
            blue = 0.0;
            for (int k = 0 ; k <= 1 ; k++) {
                blue += 0.25;
                cubeCreator( cube, color );
                
                // Cube translation
                mat4 translation = Translate( vec3(i-0.5, j-0.5, k-0.5) );
                for (int i = 0; i < numOneCubeVertices ; i++) {
                    cube[i] = translation * cube[i];
                }
                
                // Set vertices and colors
                for (int m = 0 ; m < numOneCubeVertices ; m++) {
                    vertices[current * numOneCubeVertices + m] = cube[m];
                    colors[current * numOneCubeVertices + m] = color[m];
                }
                nextCubePos[i][j][k] = current;
                currentCubePos[i][j][k] = current;
                
                colorSelector[current] = color4(red, green, blue, 1.0);
                current++;
            }
        }
    }
}

// MARK: - Rotate S
// Rotate face by angle delta
void cubeSideRotation( int delta ) {
    int currPos = 0;

    // Update rotation matrix
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (rotationAxis == axisX) {
                currPos = currentCubePos[currentBlock][i][j];
                rotationMatrix[currPos] = RotateX(delta) * rotationMatrix[currPos];
            } else if (rotationAxis == axisY) {
                currPos = currentCubePos[i][currentBlock][j];
                rotationMatrix[currPos] = RotateY(delta) * rotationMatrix[currPos];
            } else if (rotationAxis == axisZ) {
                currPos = currentCubePos[i][j][currentBlock];
                rotationMatrix[currPos] = RotateZ(delta) * rotationMatrix[currPos];
            }
            glUniformMatrix4fv(vRotation[currPos], 1, GL_TRUE, rotationMatrix[currPos]);
        }
    }

    rotatePhase = rotatePhase + delta;
    glutPostRedisplay();
    if (rotatePhase == 90) {
        for (int k = 0 ; k < 2 ; k++) {
            for (int l = 0 ; l < 2 ; l++) {
                for (int m = 0 ; m < 2 ; m++) {
                    currentCubePos[k][l][m] = nextCubePos[k][l][m];
                }
            }
        }
        rotatePhase = 0;
    } else {
        glutTimerFunc(animationSpeed, cubeSideRotation, delta);
    }
}

// MARK: - Rotate Rubiks Cube

void rubiksCubeRotation( int direction, int subcube, int axis ) {
    int rotation[2][2];
    if (direction < 0) {
        for (int i = 0 ; i < 2 ; i++) {
            for (int j = 0; j < 2 ; j++) {
                if (axis == axisX) {
                    rotation[j][1-i] = nextCubePos[subcube][i][j];
                } else if (axis == axisY) {
                    rotation[1-j][i] = nextCubePos[i][subcube][j];
                } else if (axis == axisZ) {
                    rotation[j][1-i] = nextCubePos[i][j][subcube];
                }
            }
        }
    } else {
        for (int i = 0 ; i < 2 ; i++) {
            for (int j = 0 ; j < 2 ; j++) {
                if (axis == axisX) {
                    rotation[1-j][i] = nextCubePos[subcube][i][j];
                } else if (axis == axisY) {
                    rotation[j][1-i] = nextCubePos[i][subcube][j];
                } else if (axis == axisZ) {
                    rotation[1-j][i] = nextCubePos[i][j][subcube];
                }
            }
        }
    }

    /*copy back the transposed subcube to nextCubePos*/
    for (int i = 0 ; i < 2 ; i++) {
        for (int j = 0 ; j < 2 ; j++) {
            if (axis == axisX) {
                nextCubePos[subcube][i][j] = rotation[i][j];
            } else if (axis == axisY) {
                nextCubePos[i][subcube][j] = rotation[i][j];
            } else if (axis == axisZ) {
                nextCubePos[i][j][subcube] = rotation[i][j];
            }
        }
    }
}

// MARK: - Set current block, rotation axis and rotation

int rotateAlong( int axis, int nextA, int nextB, int key ) {
    int rotation = 0;

    // Rotate along X
    if (axis == axisX) {
        if (nextB == 0) {
            currentBlock = 0;
            rotation = -1;
            if (nextA == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                    rotation = 1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisY;
                }
            } else if (nextA == 1) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 1;
                    rotationAxis = axisY;
                }
            }
        } else if (nextB == 1) {
            currentBlock = 1;
            rotation = 1;
            if (nextA == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 0;
                    rotationAxis = axisY;
                }
            } else if (nextA == 1) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                    rotation = -1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisY;
                }
            }
        }
    // Rotate along Y
    } else if (axis == axisY) {
        if (nextA == 0) {
            rotation = 1;
            currentBlock = 0;
            if (nextB == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                    rotation = -1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisX;
                }
            } else if (nextB == 1) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 1;
                    rotationAxis = axisX;
                }
            }
        } else if (nextA == 1) {
            rotation = -1;
            currentBlock = 1;
            if (nextB == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                } else if(key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 0;
                    rotationAxis = axisX;
                }
            } else if (nextB == 1) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                    rotation = 1;
                } else if (key == GLUT_RIGHT_BUTTON){
                    rotationAxis = axisX;
                }
            }
        }
    // Rotate along Z
    } else if (axis == axisZ) {
        if (nextA == 0) {
            currentBlock = 0;
            rotation = 1;
            if (nextB == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisX;
                    rotation = -1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisY;
                }
            } else if (nextB == 1) {
                if (key == GLUT_LEFT_BUTTON){
                    rotationAxis = axisX;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 1;
                    rotationAxis = axisY;
                }
            }
        } else if (nextA == 1) {
            rotation = -1;
            currentBlock = 1;
            if (nextB == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisX;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 0;
                    rotationAxis = axisY;
                }
            } else if (nextB == 1) {
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisX;
                    rotation = 1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisY;
                }
            }
        }
    }
    return rotation;
}

// MARK: - Perform Rotation

int performRotation( int currX, int currY, int currZ, int nextX, int nextY, int nextZ, int key ) {
    int rotation = 0;
    if (currX == 0) {
        if (currY == 0) {
            if (currZ == 1) {
                rotation = rotateAlong( axisX, nextY, nextZ, key );
            }
        } else if (currY == 1) {
            if (currZ == 0) {
                rotation = rotateAlong( axisZ, nextX, nextY, key );
            } else if (currZ == 1) {
                rotation = rotateAlong( axisZ, nextX, nextY, key );
                rotation = rotation * -1;
            }
        }
    } else if (currX == 1) {
        if (currY == 0) {
            if (currZ == 0) {
                rotation = rotateAlong( axisY, nextZ, nextX, key );
                rotation = rotation * -1;
            } else if (currZ == 1) {
                rotation = rotateAlong( axisX, nextY, nextZ, key );
                rotation = rotation * -1;
            }
        } else if (currY == 1) {
            if (currZ == 0) {
                rotation = rotateAlong( axisY, nextZ, nextX, key );
            } else if (currZ == 1) {
                rotation = 0;
            }
        }
    }
    return rotation;
}

// MARK: - Select Cube Face

int cubeSelector( int a, int b, int key ) {
    int rotation = 0;
    int nextX = -1;
    int nextY = -1;
    int nextZ = -1;
    int temp = 0;
    unsigned char point[3];
    int cube;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUniform1i( selectedCube, 1 );

    for (int i = 0 ; i < numCubeVertices ; i++) {
        glUniform1i( currentCube, i );
        glDrawArrays( GL_TRIANGLES, temp, numOneCubeVertices );
        temp = temp + numOneCubeVertices;
    }

    glUniform1i( selectedCube, 0 );
    glFlush();
    b = glutGet( GLUT_WINDOW_HEIGHT ) - b;
    
    // Read point pixels from the back buffer
    glReadPixels( a, b, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, point );
    int k = point[0];
    int l = point[1];
    int m = point[2];
    k = ceil(k/64.0) - 1;
    l = ceil(l/64.0) - 1;
    m = ceil(m/64.0) - 1;
    cube = (k * 4) + (l * 2) + m;
   
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUniform1i( selectedFace, 1 );

    temp = 0;
    for (int i = 0 ; i < numCubeVertices ; i++) {
        glUniform1i( currentCube, i );
        glDrawArrays( GL_TRIANGLES, temp, numOneCubeVertices );
        temp = temp + numOneCubeVertices;
    }

    glUniform1i( selectedFace, 0 );
    glFlush();

    // Read point color from the back buffer
    glReadPixels( a, b, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, point );
    
    k = (int)point[0];
    l = (int)point[1];
    m = (int)point[2];
    k = ceil(k/255.0);
    l = ceil(l/255.0);
    m = ceil(m/255.0);
   
    for (int i = 0 ; i < 2 ; i++) {
        for (int j = 0 ; j < 2; j++) {
            for (int k = 0 ; k < 2 ; k++) {
                if (currentCubePos[i][j][k] == cube) {
                    nextX = i;
                    nextY = j;
                    nextZ = k;
                    break;
                }
            }
        }
    }
   
    // Rotate with appropriate axes
    rotation = performRotation( k, l, m, nextX, nextY, nextZ, key );
    
    glutPostRedisplay();

    return rotation;
}

// MARK: - Randomly Initialize Cube

/*void randomInitialization( int numRotations ) {
    int direction;
    int currentPos;
    srand((unsigned int)time(NULL));
    for (int iter = 0 ; iter < numRotations ; iter++) {
        currentBlock = rand() % 2;
        rotationAxis = rand() % 2;
        if(rand() % 2 == 1) {
            direction = 1;
        } else {
            direction = -1;
        }
        rubiksCubeRotation( direction, currentBlock, rotationAxis );
        for (int i = 0 ; i < 2 ; i++) {
            for (int j = 0 ; j < 2 ; j++) {
                if (rotationAxis == axisX) {
                    currentPos = currentCubePos[currentBlock][i][j];
                    rotationMatrix[currentPos] = RotateX(direction*90)*rotationMatrix[currentPos];
                } else if (rotationAxis == axisY) {
                    currentPos = currentCubePos[i][currentBlock][j];
                    rotationMatrix[currentPos] = RotateY(direction*90)*rotationMatrix[currentPos];
                } else if (rotationAxis == axisZ) {
                    currentPos = currentCubePos[i][j][currentBlock];
                    rotationMatrix[currentPos] = RotateZ(direction*90)*rotationMatrix[currentPos];
                }
            }
        }
        for (int k=0 ; k < 2 ; k++) {
            for (int l = 0 ; l < 2 ; l++) {
                for (int m = 0 ; m < 2 ; m++) {
                    currentCubePos[k][l][m] = nextCubePos[k][l][m];
                }
            }
        }
    }
    rotatePhase = 0;
}*/

// MARK: - Initialization

void init() {
    // Rubik's cube init
    rubiksCube();
    
    // Initialize rotation matrix
    for (int i=0; i < numCubes; i++) {
        rotationMatrix[i] = RotateX(0);
    }
    
    // Randomize Rubik's cube
    //randomInitialization(10);
    
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    // Add vertices and colors to buffer
    glBufferData( GL_ARRAY_BUFFER, (sizeof(point4) * numCubeVertices * 2), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4) * numCubeVertices, vertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4) * numCubeVertices, sizeof(color4) * numCubeVertices, colors );

    //Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

    // Set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ));

    GLuint svPosition = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( svPosition );
    glVertexAttribPointer( svPosition, 4, GL_FLOAT, GL_FALSE, 0,
                          BUFFER_OFFSET( sizeof(point4)*numCubeVertices) );

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    currentCube = glGetUniformLocation( program, "currentCube" );
    edge = glGetUniformLocation( program, "edge" );
    selectedCube = glGetUniformLocation( program, "selectedCube" );
    selectedFace = glGetUniformLocation( program, "selectedFace" );

    // Send cube to the shader
    int cubes = 0;
    string iter = "";
    while (cubes < numCubes) {
        iter = to_string(cubes);
        vRotation[cubes] = glGetUniformLocation( program, ("vRotation[" + iter + "]").c_str() );
        glUniformMatrix4fv( vRotation[cubes], 1, GL_TRUE, rotationMatrix[cubes] );
        cubeColor[cubes] = glGetUniformLocation( program, ("cubeColor[" + iter + "]").c_str() );
        glUniform4fv( cubeColor[cubes], 1, colorSelector[cubes] );
        cubes++;
    }
    
    glUseProgram( program ) ;
    glUniform1i( edge, 0 );
    glUniform1i( currentCube, 0 );
    glUniform1i( selectedCube, 0 );
    glUniform1i( selectedFace, 0 );
    glEnable( GL_DEPTH_TEST | GL_LINE_SMOOTH );
    glLineWidth( 4.0 );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

// MARK: - Display

void display() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    const vec3 displacement( 0, 0, 0);
    mat4  model_view = ( Translate( displacement ) *
                        RotateX( Theta[axisX] + rotateX ) *
                        RotateY( Theta[axisY] ) *
                        RotateZ( Theta[axisZ] + rotateZ ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );

    // Render subcubes
    int temp = 0;
    int cubeVertices = 0;
    
    while (cubeVertices < numCubeVertices) {
        glUniform1i( currentCube, temp );
        glDrawArrays( GL_TRIANGLES, cubeVertices, numOneCubeVertices );
        temp++;
        cubeVertices += numOneCubeVertices;
    }

    glUniform1i( edge, 1 );

    // Render cube edges
    temp = -1;
    cubeVertices = 0;
    while (cubeVertices < numCubeVertices) {
        if (cubeVertices % numOneCubeVertices == 0) {
            temp++;
        }
        glUniform1i( currentCube, temp );
        glDrawArrays( GL_LINE_STRIP, cubeVertices, 3 );
        cubeVertices += 3;
    }
    
    glUniform1i( edge, 0 );
    glutSwapBuffers();
}

// MARK: - Reshape

void reshape( int width, int height ) {
    glViewport( 0, 0, width, height );
    
    // Set the projection matrix
    mat4 projection;
    
    if (width <= height)
      projection = Ortho( -4, 4, -4 * ((GLfloat)height/width), 4 * ((GLfloat)height/width), -4, 4);
    else
      projection = Ortho( -4 * ((GLfloat)width/height), 4 * ((GLfloat)width/height), -4, 4, -4, 4);

    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

// MARK: - Help Menu

void helpMenu() {
    printf("Press arrow keys to change the view\n");
    printf("Click to move the faces of the cube\n");
    printf(" 1. For a vertical rotation, perform a right-click\n");
    printf(" 2. For a horizontal rotation, perform a left-click\n");
    printf("Press r or R to randomize the Rubik's cube\n");
    printf("Press i or I  to go to the initial position\n");
    printf("Press h or H for help\n");
    printf("Press q or Q to exit\n\n");
}

// MARK: - Mouse

void mouse( int key, int state, int x, int y ) {
    int direction;
    
    if (state == GLUT_DOWN && rotatePhase == 0) {
        // Set the current block, rotation axis and direction
        direction = cubeSelector(x, y, key);
        if (direction < 0) {
            rubiksCubeRotation(direction, currentBlock, rotationAxis);
            rotatePhase = 180;
            cubeSideRotation(-5);
        } else if (direction > 0) {
            rubiksCubeRotation(direction, currentBlock, rotationAxis);
            rotatePhase = 0;
            cubeSideRotation(5);
        }
    }
}

// MARK: - Arrow Keys

void arrowKeys( int key, int a, int b ) {
    switch (key) {
        case GLUT_KEY_LEFT:
            rotateZ += 4;
            break;
        case GLUT_KEY_RIGHT:
            rotateZ -= 4;
            break;
        case GLUT_KEY_UP:
            rotateX += 4;
            break;
        case GLUT_KEY_DOWN:
            rotateX -= 4;
            break;
    }
    glutPostRedisplay();
}

// MARK: - Keyboard

void keyboard( unsigned char key, int x, int y ) {
    switch (key) {
        case 'i': case 'I':
            rotateX = 135.0;
            rotateZ = -45.0;
            break;
        case 'r': case 'R':
            //randomInitialization(5);
            //break;
        case 'h': case 'H':
            helpMenu();
            break;
        case 'q': case 'Q':
            exit(0);
            break;
    }
    glutPostRedisplay();
}

// MARK: - Main

int main( int agrc, char** agrv ) {
    glutInit( &agrc, agrv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_3_2_CORE_PROFILE );
    glutInitWindowSize( 512, 512 );
    glutInitWindowPosition( 900, 10 );
    glutCreateWindow( "COMP410 Assignment 2" );
    
    #ifdef __APPLE__
    #else // non-Mac OS X operating systems
    #   glewExperimental = GL_TRUE;
    #   glewInit();
    #endif
    
    helpMenu();
    init();
    
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutReshapeFunc( reshape );
    glutSpecialFunc( arrowKeys );
    glutMouseFunc( mouse );

    glutMainLoop();
    return 0;
}

