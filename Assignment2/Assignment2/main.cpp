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
    for (int i = -1 ; i <= 0 ; i++) {
        green = 0.0;
        red += 0.25;
        for (int j = -1; j <= 0 ; j++) {
            green += 0.25;
            blue = 0.0;
            for (int k = -1 ; k <= 0 ; k++) {
                blue += 0.25;
                cubeCreator( cube, color );
                
                // Cube translation
                mat4 translation = Translate( vec3(i, j, k) );
                for (int i = 0; i < numOneCubeVertices ; i++) {
                    cube[i] = translation * cube[i];
                }
                
                // Set vertices and colors
                for (int m = 0 ; m < numOneCubeVertices ; m++) {
                    vertices[current * numOneCubeVertices + m] = cube[m];
                    colors[current * numOneCubeVertices + m] = color[m];
                }
                nextCubePos[i+1][j+1][k+1] = current;
                currentCubePos[i+1][j+1][k+1] = current;
                
                colorSelector[current] = color4(red, green, blue, 1.0);
                current++;
            }
        }
    }
}

// MARK: - Rotate S
/* Rotate the slab by angle delta */
void cubeSideRotation( int delta ) {
    int currPos = 0;

    /* updates rotation matrix of rotating cubes and send to vshader */
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
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
        for (int k = 0 ; k < 3 ; k++) {
            for (int l = 0 ; l < 3 ; l++) {
                for (int m = 0 ; m < 3 ; m++) {
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
    int rotation[3][3];
    if (direction < 0) {
        for (int i = 0 ; i < 3 ; i++) {
            for (int j = 0; j < 3 ; j++) {
                if (axis == axisX) {
                    rotation[j][2-i] = nextCubePos[subcube][i][j];
                } else if (axis == axisY) {
                    rotation[2-j][i] = nextCubePos[i][subcube][j];
                } else if (axis == axisZ) {
                    rotation[j][2-i] = nextCubePos[i][j][subcube];
                }
            }
        }
    } else {
        for (int i = 0 ; i < 3 ; i++) {
            for (int j = 0 ; j < 3 ; j++) {
                if (axis == axisX) {
                    rotation[2-j][i] = nextCubePos[subcube][i][j];
                } else if (axis == axisY) {
                    rotation[j][2-i] = nextCubePos[i][subcube][j];
                } else if (axis == axisZ) {
                    rotation[2-j][i] = nextCubePos[i][j][subcube];
                }
            }
        }
    }

    /*copy back the transposed subcube to nextCubePos*/
    for (int i = 0 ; i < 3 ; i++) {
        for (int l = 0 ; l < 3 ; l++) {
            if (axis == axisX) {
                nextCubePos[subcube][i][l] = rotation[i][l];
            } else if (axis == axisY) {
                nextCubePos[i][subcube][l] = rotation[i][l];
            } else if (axis == axisZ) {
                nextCubePos[i][l][subcube] = rotation[i][l];
            }
        }
    }
}

// MARK: - Rotate the Side Along the Axis

int rotateAlong( int axis, int nextA, int nextB, int key ) {
    int rotation = 0;

    /* Rotation Along X*/
    if (axis == axisX) {
        if (nextB == 0) {
            if (nextA == 0) {
                currentBlock = 0;
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                    rotation = 1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisY;
                    rotation = -1;
                }
            } else if (nextA == 1) {
                rotationAxis = axisY;
                currentBlock = 1;
                rotation = -1;
            } else if (nextA == 2) {
                if (key == GLUT_LEFT_BUTTON) {
                    currentBlock = 0;
                    rotationAxis = axisZ;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 2;
                    rotationAxis = axisY;
                }
                rotation = -1;
            }
        } else if (nextB == 1) {
            if (nextA == 0) {
                rotationAxis = axisZ;
                currentBlock = 1;
                rotation = 1;
            } else if (nextA == 2) {
                rotationAxis = axisZ;
                currentBlock = 1;
                rotation = -1;
            }
        } else if (nextB == 2) {
            if (nextA == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    currentBlock = 2;
                    rotationAxis = axisZ;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 0;
                    rotationAxis = axisY;
                }
                rotation = 1;
            } else if (nextA == 1) {
                rotationAxis = axisY;
                currentBlock = 1;
                rotation = 1;
            } else if (nextA == 2) {
                currentBlock = 2;
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                    rotation = -1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisY;
                    rotation = 1;
                }
            }
        }
    /* Rotation Along Y*/
    } else if (axis == axisY) {
        if (nextA == 0) {
            if (nextB == 0) {
                currentBlock = 0;
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                    rotation = -1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisX;
                    rotation = 1;
                }
            } else if (nextB == 1) {
                rotationAxis = axisX;
                currentBlock = 1;
                rotation = 1;
            } else if (nextB == 2) {
                if (key == GLUT_LEFT_BUTTON) {
                    currentBlock = 0;
                    rotationAxis = axisZ;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 2;
                    rotationAxis = axisX;
                }
                rotation = 1;
            }
        } else if (nextA == 1) {
            if (nextB == 0) {
                rotationAxis = axisZ;
                currentBlock = 1;
                rotation = -1;
            } else if (nextB == 1) {

            } else if (nextB == 2) {
                rotationAxis = axisZ;
                currentBlock = 1;
                rotation = 1;
            }
        } else if (nextA == 2) {
            if (nextB == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    currentBlock = 2;
                    rotationAxis = axisZ;
                } else if(key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 0;
                    rotationAxis = axisX;
                }
                rotation = -1;
            } else if (nextB == 1) {
                rotationAxis = axisX;
                currentBlock = 1;
                rotation = -1;
            } else if (nextB == 2) {
                currentBlock = 2;
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisZ;
                    rotation = 1;
                } else if (key == GLUT_RIGHT_BUTTON){
                    rotationAxis = axisX;
                    rotation = -1;
                }
            }
        }
    /* Rotation Along Z */
    } else if (axis == axisZ) {
        if (nextA == 0) {
            if (nextB == 0) {
                currentBlock = 0;
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisX;
                    rotation = -1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisY;
                    rotation = 1;
                }
            } else if (nextB == 1) {
                rotationAxis = axisY;
                currentBlock = 1;
                rotation = 1;
            } else if (nextB == 2) {
                if (key == GLUT_LEFT_BUTTON){
                    currentBlock = 0;
                    rotationAxis = axisX;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 2;
                    rotationAxis = axisY;
                }
                rotation = 1;
            }
        } else if (nextA == 1) {
            if (nextB == 0) {
                rotationAxis = axisX;
                currentBlock = 1;
                rotation = -1;
            } else if (nextB == 2) {
                rotationAxis = axisX;
                currentBlock = 1;
                rotation = 1;
            }
        } else if (nextA == 2) {
            if (nextB == 0) {
                if (key == GLUT_LEFT_BUTTON) {
                    currentBlock = 2;
                    rotationAxis = axisX;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    currentBlock = 0;
                    rotationAxis = axisY;
                }
                rotation = -1;
            } else if (nextB == 1) {
                rotationAxis = axisY;
                currentBlock = 1;
                rotation = -1;
            } else if (nextB == 2) {
                currentBlock = 2;
                if (key == GLUT_LEFT_BUTTON) {
                    rotationAxis = axisX;
                    rotation = 1;
                } else if (key == GLUT_RIGHT_BUTTON) {
                    rotationAxis = axisY;
                    rotation = -1;
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
    unsigned char point[4];
    int cube;
    int cubeVert = 0;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUniform1i( selectCube, 1 );

    while (cubeVert < numCubeVertices) {
        glUniform1i( currentCube, cubeVert );
        glDrawArrays( GL_TRIANGLES, temp, numOneCubeVertices );
        temp = temp + numOneCubeVertices;
        cubeVert++;
    }

    glUniform1i( selectCube, 0 );
    glFlush();
    b = glutGet( GLUT_WINDOW_HEIGHT ) - b;
    
    // Read point color from the back buffer
    glReadPixels( a, b, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, point );

    int k = (int)point[0];
    int l = (int)point[1];
    int m = (int)point[2];
    k = ceil(k/64.0)-1;
    l = ceil(l/64.0)-1;
    m = ceil(m/64.0)-1;
    cube = (k * 9) + (l * 3) + m;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUniform1i( selectFace, 1 );

    temp = 0;
    cubeVert = 0;
    while (cubeVert < numCubeVertices) {
        glUniform1i( currentCube, cubeVert );
        glDrawArrays( GL_TRIANGLES, temp, numOneCubeVertices );
        temp = temp + numOneCubeVertices;
        cubeVert++;
    }

    glUniform1i( selectFace, 0 );
    glFlush();

    // Retrieve point color from the back buffer
    glReadPixels( a, b, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, point );
    
    k = (int)point[0];
    l = (int)point[1];
    m = (int)point[2];
    k = ceil(k/255.0);
    l = ceil(l/255.0);
    m = ceil(m/255.0);

    for (int i = 0 ; i < 3 ; i++) {
        for (int j = 0 ; j < 3 ; j++) {
            for (int k = 0 ; k < 3 ; k++) {
                if (currentCubePos[i][j][k] == cube) {
                    nextX = i;
                    nextY = j;
                    nextZ = k;
                    break;
                }
            }
        }
    }

    // Perform rotation with appropriate axes
    rotation = performRotation( k, l, m, nextX, nextY, nextZ, key );
    
    glutPostRedisplay();

    return rotation;
}

// MARK: - Randomly Initialize Cube

void randomInitialization( int numRotations ) {
    int direction;
    int currPos;

    for (int iter = 0 ; iter < numRotations ; iter++) {
        currentBlock = rand() % 3;
        rotationAxis = rand() % 3;
        if(rand() % 2 == 1) {
            direction = 1;
        } else {
            direction = -1;
        }
        rubiksCubeRotation( direction, currentBlock, rotationAxis );
        for (int i = 0 ; i < 3 ; i++) {
            for (int j = 0 ; j < 3 ; j++) {
                if (rotationAxis == axisX) {
                    currPos = currentCubePos[currentBlock][i][j];
                    rotationMatrix[currPos] = RotateX(direction*90)*rotationMatrix[currPos];
                } else if (rotationAxis == axisY) {
                    currPos = currentCubePos[i][currentBlock][j];
                    rotationMatrix[currPos] = RotateY(direction*90)*rotationMatrix[currPos];
                } else if (rotationAxis == axisZ) {
                    currPos = currentCubePos[i][j][currentBlock];
                    rotationMatrix[currPos] = RotateZ(direction*90)*rotationMatrix[currPos];
                }
            }
        }
        for (int k=0 ; k < 3 ; k++) {
            for (int l = 0 ; l < 3 ; l++) {
                for (int m = 0 ; m < 3 ; m++) {
                    currentCubePos[k][l][m] = nextCubePos[k][l][m];
                }
            }
        }
    }
    rotatePhase = 0;
}

// MARK: - Initialization

void init() {
    // Rubik's cube init
    rubiksCube();
    
    // Randomize Rubik's cube
    // randomInitialization(5);
    
    // Initialize rotation matrix
    for (int i=0; i < numCubes; i++) {
        rotationMatrix[i] = RotateX(0);
    }
    
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
    selectCube = glGetUniformLocation( program, "selectCube" );
    selectFace = glGetUniformLocation( program, "selectFace" );

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
    glUniform1i( selectCube, 0 );
    glUniform1i( selectFace, 0 );
    glEnable( GL_DEPTH_TEST | GL_LINE_SMOOTH );
    glLineWidth( 4.0 );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

// MARK: - Display

void display() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    const vec3 displacement( 0.5, -0.5, 0.5 );
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
      projection = Ortho( -4, 4, -4/(GLfloat(width)/height ), 4/(GLfloat(width)/height), -4, 4);
    else
      projection = Ortho( -4*(GLfloat(width)/height ), 4*(GLfloat(width)/height), -4, 4, -4, 4);

    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

// MARK: - Help Menu

void helpMenu() {
    printf("Press arrow keys to rotate the cube\n");
    printf("Click to move the faces of the cube\n");
    printf(" - For a vertical rotation, perform a right-click\n");
    printf(" - For a horizontal rotation, perform a left-click\n");
    printf("Press r or R to randomize the Rubik's cube\n");
    printf("Press z or Z to zoom in the cube\n");
    printf("Press x or X to zoom out the cube\n");
    printf("Press i or I  to go to the initial position\n");
    printf("Press h or H for help\n");
    printf("Press q or Q to exit\n\n");
}

// MARK: - Mouse

void mouseAction( int key, int state, int x, int y ) {
    int direction;
    
    if (state == GLUT_DOWN && rotatePhase == 0) {
        /*Set the currentBlock and rotationAxis after picking
        the cube and face and returns the direction of rotation*/
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

void arrowKeysAction( int key, int a, int b ) {
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
            //randominitialize
            break;
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
    glutInitWindowPosition( 100, 100 );
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
    glutSpecialFunc( arrowKeysAction );
    glutMouseFunc( mouseAction );

    glutMainLoop();
    return 0;
}

