//assignment1 - sitare arslanturk 
#define GL_SILENCE_DEPRECATION

#include "Angel.h"
#include <iostream>
#include <fstream>

typedef vec4 color4;
typedef vec4 point4;

//Update the xPosition and yPosition according to velocities.
//initial positions and variables
float initvalue = 2;
float x_pos = -initvalue;
float y_pos = initvalue;
float gravity = 0.0009;
const float friction = 0.009;
float v_x = 0.005;
float v_y = 0;

//pink
int ColorChoose = 7;
// initial type is sphere
int objectType = 1;

// MARK: - Cube

const int NumVerticesCube = 36;
point4 points[NumVerticesCube];
color4 colors[NumVerticesCube];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
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
int  Axis = Xaxis;
GLfloat Theta[NumAxes] = { 0.0, 0.0, 0.0 };

// Model-view, projection and color matrices uniform location
GLuint ModelView, Projection, ColorChange;
GLuint vao[3];

// quad generates two triangles for each face
int Index = 0;
void quad( int a, int b, int c, int d ) {
    points[Index] = vertices[a]; Index++;
    points[Index] = vertices[b]; Index++;
    points[Index] = vertices[c]; Index++;
    points[Index] = vertices[a]; Index++;
    points[Index] = vertices[c]; Index++;
    points[Index] = vertices[d]; Index++;
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

// MARK: - Sphere

const int NumTimesToSubdivide = 5;
const int NumTriangles = 4096;
const int NumVerticesSphere = 3 * NumTriangles;

point4 pointsSphere[NumVerticesSphere];
vec3 normals[NumVerticesSphere];

int IndexSphere = 0;

void triangle( const point4& a, const point4& b, const point4& c ) {
    vec3 normal = normalize( cross(b - a, c - b) );
    normals[IndexSphere] = normal; pointsSphere[IndexSphere] = a; IndexSphere++;
    normals[IndexSphere] = normal; pointsSphere[IndexSphere] = b; IndexSphere++;
    normals[IndexSphere] = normal; pointsSphere[IndexSphere] = c; IndexSphere++;
}

point4 unit( const point4& p ) {
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    point4 t;
    if ( len > DivideByZeroTolerance ) {
        t = p / sqrt(len);
        t.w = 1.0;
    }
    return t;
}

void divide_triangle( const point4& a, const point4& b,
                     const point4& c, int count )
{
    if ( count > 0 ) {
        point4 v1 = unit( a + b );
        point4 v2 = unit( a + c );
        point4 v3 = unit( b + c );
        divide_triangle( a, v1, v2, count - 1 );
        divide_triangle( c, v2, v3, count - 1 );
        divide_triangle( b, v3, v1, count - 1 );
        divide_triangle( v1, v3, v2, count - 1 );
    }
    else {
        triangle( a, b, c );
    }
}

void tetrahedron( int count ) {
    point4 v[4] = {
        vec4( 0.0, 0.0, 1.0, 1.0 ),
        vec4( 0.0, 0.942809, -0.33333, 1.0 ),
        vec4( -0.816497, -0.471405, -0.33333, 1.0 ),
        vec4( 0.816497, -0.471405, -0.33333, 1.0 )
    };
    divide_triangle( v[0], v[1], v[2], count );
    divide_triangle( v[3], v[2], v[1], count );
    divide_triangle( v[0], v[3], v[1], count );
    divide_triangle( v[0], v[2], v[3], count );
}

// MARK: BUNNY
const int NumVerticesBunny = 9840 * 3; // (number of triangles in bunny.off) * 3
point4 bunnyPoints[NumVerticesBunny];
GLfloat bunnyScale;

void bunny() {
    std::ifstream bunnyFile("bunny.off");
            
    GLuint numTris = 0, numVerts = 0;
    point4 vertices[4922];

    std::string dump = "";        // dummy string to dump unneeded data

    bunnyFile >> dump;        // OFF
    bunnyFile
        >> numVerts            // 4922
        >> numTris            // 9840
        >> dump;            // don't need number of edges

    GLfloat maxDim = -10000;
    GLfloat x = 0, y = 0, z = 0;
    for (int i = 0; i < numVerts; i++) {
        bunnyFile >> x >> y >> z;
        vertices[i] = point4(x, y, z, 1.0);

        // finding the maximum dimension for scaling
        maxDim = fmax(abs(x), maxDim);
        maxDim = fmax(abs(y), maxDim);
        maxDim = fmax(abs(z), maxDim);
    }
    bunnyScale = 1.0 / maxDim;    // to "normalize" bunny while viewing
    bunnyScale *= 4;

    int v1 = 0, v2 = 0, v3 = 0;
    for (int i = 0; i < numTris; i++) {
        // we know there are only triangles in the model
        bunnyFile >> dump >> v1 >> v2 >> v3;
        bunnyPoints[3*i] = vertices[v1];
        bunnyPoints[3*i + 1] = vertices[v2];
        bunnyPoints[3*i + 2] = vertices[v3];
    }
}

// MARK: - Initialization

void init() {
    //shaders
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    
    // MARK: Cube Init
    cube();
    
    // Create and initialize a buffer object
    GLuint buffer1;
    glGenBuffers( 1, &buffer1 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer1 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
    
    // Create a vertex array object
    glGenVertexArrays( 3, vao);
    glBindVertexArray( vao[0] );
    
    // Set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
     
    // MARK: Sphere Init
    tetrahedron( NumTimesToSubdivide );
    glBindVertexArray( vao[1] );
    
    // Create and initialize a buffer object with different name
    GLuint buffer2;
    glGenBuffers( 1, &buffer2 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer2 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(normals), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsSphere), pointsSphere );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsSphere), sizeof(normals), normals );
    
    // Set up vertex arrays
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
        glEnableVertexAttribArray( vNormal );
        glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
                BUFFER_OFFSET(sizeof(pointsSphere)) );
    
    // TODO: Bunny Init
    bunny();
    glBindVertexArray( vao[2] );
    
    // Create and initialize a buffer object with different name
    GLuint buffer3;
    glGenBuffers( 3, &buffer3 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer3 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
   
    // Set up vertex arrays
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    
    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );
    ColorChange = glGetUniformLocation( program, "ColorChange" );
    
    
    glUseProgram( program );
    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

int numVertices[3] = { NumVerticesCube, NumVerticesSphere, NumVerticesBunny };

// MARK: - Display

void display( void ) {
    glBindVertexArray( vao[objectType] );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4 model_view;
    
    vec3 displacement( x_pos, y_pos, 0.0 );
    
    model_view = ( Scale(0.25, 0.25, 0.25) * Translate( displacement ) * RotateX( Theta[Xaxis] ) );
    
    //sets the color
    glUniform4fv( ColorChange, 1, vertex_colors[ColorChoose] );
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
    glDrawArrays( GL_TRIANGLES, 0, numVertices[objectType] );
    glutSwapBuffers();
}

// MARK: - Reshape

void reshape( int w, int h ) {
    glViewport( 0, 0, w, h );
    
    // Set the projection matrix
    mat4  projection;
    if (w <= h) {
        projection = Ortho(-1.0, 1.0, -1.0 * ((GLfloat)h/(GLfloat)w), 1.0 * ((GLfloat)h/(GLfloat)w), -1.0, 1.0);
    } else {
        projection = Ortho(-1.0* ((GLfloat)w/(GLfloat)h), 1.0 *((GLfloat)w/(GLfloat)h), -1.0, 1.0, -1.0, 1.0);
    }
    
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
    glDrawArrays( GL_TRIANGLES, 0, numVertices[objectType] );
    glutSwapBuffers();
}

// MARK: - Keyboard

void keyboard( unsigned char key, int x, int y ) {
    if (key == 'Q' | key == 'q')
        exit(0);
    
    if (key == 'I' | key == 'i') {
        //reset to initial state
        x_pos = -initvalue;
        v_x = 0.003;
        y_pos = initvalue;
        v_y = 0;
        gravity = 0.0009;
    }
    
    if (key == 'h' | key == 'H') {
        // help menu
        std::cout<< " The Amazing Bouncing Cube/Sphere"<<std::endl;
        std::cout<< " Choose the object type, fill style and color"<<std::endl;
        std::cout<< " -Press Q to exit" << std::endl;
        std::cout<< " -Press I to reset" << std::endl;
        std::cout<< " -Press H to get help" <<std::endl;
        std::cout<< " -Press left mouse button to reach menu" <<std::endl;
    }
}

// MARK: - Timer

void timer( int t ) {
    x_pos += v_x;
    
    if (y_pos<-(2*initvalue)) {
        v_y *= -1;
        v_y -= friction;
        if(v_y < 0){
            gravity = 0;
            v_y = 0;
            v_x = 0;
        }
    }
    
    v_y -= gravity;
    y_pos += v_y;
    
    // trying to rotate
    Theta[Axis] += 2.0;
    if ( Theta[Axis] > 360.0 ) {
        Theta[Axis] -= 360.0;
    }
    
    glutPostRedisplay();
    glutTimerFunc( 5, timer, 0 );
}

// MARK: - Menu

void color_menu( int n ) {
    ColorChoose = n;
}

void object_menu( int n ) {
    objectType = n;
}

void drawing_menu( int n ) {
    if (n == 0) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else if (n == 1) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void mainMenu( int n ){}

// MARK: - Main

int main( int argc, char **argv ) {
    glutInit( &argc, argv );
    glutInitDisplayMode(  GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize( 512, 780 );
    glutInitWindowPosition( 100, 100 );
    glutCreateWindow( "COMP410 Assingment 1" );
    init();
    
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutTimerFunc( 5, timer, 0 );
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    int objectMenu = glutCreateMenu( object_menu );
    glutAddMenuEntry("Cube", 0);
    glutAddMenuEntry("Sphere", 1);
    glutAddMenuEntry("Bunny", 2);
    
    int drawingMenu = glutCreateMenu( drawing_menu );
    glutAddMenuEntry("Line Mode", 0);
    glutAddMenuEntry("Solid Mode", 1);
    
    int colorMenu = glutCreateMenu( color_menu );
    glutAddMenuEntry("Black", 0);
    glutAddMenuEntry("Red", 1);
    glutAddMenuEntry("Yellow", 2);
    glutAddMenuEntry("Green", 3);
    glutAddMenuEntry("Blue", 4);
    glutAddMenuEntry("Magenta", 5);
    glutAddMenuEntry("White", 6);
    glutAddMenuEntry("Cyan", 7);
    
    glutCreateMenu( mainMenu );
    glutAddSubMenu("Object Style", objectMenu);
    glutAddSubMenu("Drawing Mode", drawingMenu);
    glutAddSubMenu("Color", colorMenu);
    
    glutAttachMenu(GLUT_LEFT_BUTTON);
    
    glutMainLoop();
    return 0;
}

