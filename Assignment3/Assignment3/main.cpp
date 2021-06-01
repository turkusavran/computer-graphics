// COMP410: Computer Graphics - Assignment 3
// Türkü Bengisu Savran

#define GL_SILENCE_DEPRECATION
#include <fstream>
#include <iostream>
#include <stdio.h>
#include "Angel.h"

using namespace std;

typedef vec4  color4;
typedef vec4  point4;
typedef vec2  point2;

double SCALE_FACTOR = 1.0;

// Right click menu initialization
GLint menuProjectionMode, menuDrawingMode, menuBackColor, menuShading, menuPhong;
GLint menuLightMovement, menuLightSwitch, menuMaterialType, menuLightColor;
enum {OrthographicProjectionMode = 0, PerspectiveProjectionMode = 1};
enum {Off = 0, On = 1};
enum {Wireframe = 0, Gouraud = 1, Phong = 2, ModifiedPhong = 3, Texture = 4};
enum {Plastic = 5, Metallic = 10};
enum {LightNotMoving = 0, LightMoving = 1};
enum {PointLightOn = 0, PointLightOff = 1};
enum {DirectionalLightOn = 2, DirectionalLightOff = 3};

int PROJECTION_TYPE_GLOBAL = OrthographicProjectionMode;
int DRAWING_MODE_GLOBAL = Gouraud;
int SECONDARY_MODE_GLOBAL = Off;
int Zclose = 1, Zaway = 5;
vec3 displacement( 0, 0, 0 );

GLdouble xA = -1, yA = 1, zA = -1;
GLdouble xB = 1, yB = -1, zB = 1;

double rotX = 0.0, rotY = 0.0, rotZ = 0.0;

// Axes details
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int Axis = Xaxis;
GLfloat Theta[NumAxes] = { 0.0, 0.0, 0.0 };

GLuint program, vPosition, vNormal, vCoords;
GLuint ModelView, ModelView2, Projection;
GLuint DrawingMode, Secondary_Mode;
GLuint AmbientProduct, DiffuseProduct, SpecularProduct, Light1Position, Light2Position, Shininess;
GLuint LightToggle1, LightToggle2, customTexture;
GLuint textures[2];

// variables for loading from OFFX file
int numVerticesOFF, numTriangles;
point4 *vertices, *faceVertex;

point2 *texture, *v_coords;
vec3 *normal, *faceNormal;
int *faceIndex;

// Sizes of texture and texture location
int sizeX, sizeY;
GLubyte *textureImage;

point4 LightPosition1( -1.0, 1.0, 0.0, 1.0 );
point4 LightPosition2( -1.0*-1.0, 1.0*-1.0, 0.0+1.0, 0.0 );
color4 LightColor( 1.0, 1.0, 1.0, 1.0 );

int LightsMovement = LightNotMoving, LightIsOn1 = 1, LightIsOn2 = 1;

color4 material_ambient( 0.1, 0.1, 0.1, 1.0 );
color4 material_diffuse( 0.4, 0.4, 0.4, 1.0 );
color4 material_specular( 1.0, 1.0, 1.0, 1.0 );
float  material_shininess = Metallic;

color4 ambientP = LightColor * material_ambient;
color4 diffuseP = LightColor * material_diffuse;
color4 specularP = LightColor * material_specular;

// MARK: - Sphere

const int NumTimesToSubdivide = 5;
const int NumTrianglesSphere = 4096;
const int NumVerticesSphere = 3 * NumTrianglesSphere;

point4 pointsSphere[NumVerticesSphere];
vec3 normals[NumVerticesSphere];

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

void sphere( int count ) {
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

// MARK: - Initialization

void init() {

    program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // MARK: Sphere Init
    sphere( NumTimesToSubdivide );
    
    // Create and initialize buffer object
    // Create and initialize a buffer object for sphere
    GLuint buffer1;
    glGenBuffers( 1, &buffer1 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer1 );
    glBufferData( GL_ARRAY_BUFFER, sizeof(pointsSphere) + sizeof(normals), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(pointsSphere), pointsSphere );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(pointsSphere), sizeof(normals), normals );
    
    // Set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    vNormal = glGetAttribLocation(program, "vNormal");
    vCoords = glGetAttribLocation(program, "vCoords");
    AmbientProduct = glGetUniformLocation(program, "AmbientProduct");
    DiffuseProduct = glGetUniformLocation(program, "DiffuseProduct");
    SpecularProduct = glGetUniformLocation(program, "SpecularProduct");
    Light1Position = glGetUniformLocation(program, "Light1Position");
    Light2Position = glGetUniformLocation(program, "Light2Position");
    LightToggle1 = glGetUniformLocation(program, "L1_Toggle");
    LightToggle2 = glGetUniformLocation(program, "L2_Toggle");
    Shininess = glGetUniformLocation(program, "Shininess");
    DrawingMode = glGetUniformLocation(program, "DRAWING_MODE_GLOBAL");
    Secondary_Mode = glGetUniformLocation(program, "SECONDARY_MODE_GLOBAL");
    
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*3*numTriangles));
    glEnableVertexAttribArray(vCoords);
    glVertexAttribPointer(vCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*3*numTriangles+sizeof(vec3)*3*numTriangles));
    glUniform4fv(AmbientProduct, 1, ambientP);
    glUniform4fv(DiffuseProduct, 1, diffuseP);
    glUniform4fv(SpecularProduct, 1, specularP);
    glUniform4fv(Light1Position, 1, LightPosition1);
    glUniform4fv(Light2Position, 1, LightPosition2);
    glUniform1i(LightToggle1, LightIsOn1);
    glUniform1i(LightToggle2, LightIsOn2);
    glUniform1f(Shininess, material_shininess);
    glUniform1i(DrawingMode, DRAWING_MODE_GLOBAL);
    glUniform1i(Secondary_Mode, SECONDARY_MODE_GLOBAL);

    // Retrieve transformation uniform variable locations
    ModelView = glGetUniformLocation(program, "ModelView");
    ModelView2 = glGetUniformLocation(program, "ModelView2");
    Projection = glGetUniformLocation(program, "Projection");

    glGenTextures(1, &customTexture );
    glBindTexture(GL_TEXTURE_2D, customTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImage);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Default background
    glClearColor(1.0, 1.0, 1.0, 1.0);   //white background
   
}

// MARK: - Light Movement

void moveLight(int offset, int axis) {
    if (axis == Xaxis) {
        LightPosition1 = RotateX(offset) * LightPosition1;
        LightPosition2 = RotateX(offset) * LightPosition2;
        glUniform4fv(Light1Position, 1, Translate( displacement )*LightPosition1);
        glUniform4fv(Light2Position, 1, Translate( displacement )*LightPosition2);
    } else if (axis == Yaxis) {
        LightPosition1 = RotateY(offset) * LightPosition1;
        LightPosition2 = RotateY(offset) * LightPosition2;
        glUniform4fv(Light1Position, 1, Translate( displacement )*LightPosition1);
        glUniform4fv(Light2Position, 1, Translate( displacement )*LightPosition2);
    } else if (axis == Zaxis) {
        LightPosition1 = RotateZ(offset) * LightPosition1;
        LightPosition2 = RotateZ(offset) * LightPosition2;
        glUniform4fv(Light1Position, 1, Translate( displacement )*LightPosition1);
        glUniform4fv(Light2Position, 1, Translate( displacement )*LightPosition2);
    }
}

// MARK: - Display

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 model_view = (Translate( displacement ) *
                        Scale(1.0*SCALE_FACTOR,1.0*SCALE_FACTOR,1.0*SCALE_FACTOR) *
                        RotateX( Theta[Xaxis] + rotX ) *
                        RotateY( Theta[Yaxis] + rotY ) *
                        RotateZ( Theta[Zaxis] + rotZ )  );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view );
    mat4  model_viewl = (Scale(1.0*SCALE_FACTOR,1.0*SCALE_FACTOR,1.0*SCALE_FACTOR) *
                        RotateX( Theta[Xaxis] + rotX ) *
                        RotateY( Theta[Yaxis] + rotY ) *
                        RotateZ( Theta[Zaxis] + rotZ )  );

    glUniformMatrix4fv( ModelView2, 1, GL_TRUE, model_viewl );


    if (DRAWING_MODE_GLOBAL == Wireframe) {
        for(int i = 0 ; i < numTriangles*3 ; i+=3) {
            glDrawArrays(GL_LINE_LOOP, i, 3);
        }
    } else if (DRAWING_MODE_GLOBAL == Gouraud) {
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
    } else if (DRAWING_MODE_GLOBAL == Phong) {
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
    } else if (DRAWING_MODE_GLOBAL == ModifiedPhong) {
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
    } else if (DRAWING_MODE_GLOBAL == Texture) {
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*3);
    }

    glutSwapBuffers();
}

// MARK: - Read PPM File

void readPPM(const char* ppmFile, GLubyte* image) {
    /*FILE *fd;
    int k, n, m, t;
    char c;
    char b[100];
    float s;
    int red, green, blue;

    fd = fopen("earth.ppm", "r");
    t = fscanf(fd, "%[^\n] ", b);

    if (b[0] != 'P'|| b[1] != '3') {
        exit(0);
    }

    t = fscanf(fd, "%c", &c);
    while(c == '#') {
        t = fscanf(fd, "%[^\n] ", b);
        printf("%s\n", b);
        t = fscanf(fd, "%c", &c);
    }
    ungetc(c, fd);

    t = fscanf(fd, "%d %d %d", &n, &m, &k);
    sizeX = n;
    sizeY = m;

    textureImage = new GLubyte[sizeX*sizeY*3];

    for (int i = n-1 ; i >= 0 ; i--) {
        for (int j = 0 ; j < m ; j++) {
            t = fscanf(fd, "%d %d %d", &red, &green, &blue);
            textureImage[(i*sizeY*3)+(j*3)+0] = (GLubyte)red;
            textureImage[(i*sizeY*3)+(j*3)+1] = (GLubyte)green;
            textureImage[(i*sizeY*3)+(j*3)+2] = (GLubyte)blue;
        }
    }

    fclose(fd);*/
    FILE *fd;
    int k, nm, n, m;
    char c;
    int i;
    char b[100];
    float s;
    int red, green, blue;
    printf("enter file name\n");
    scanf("%s", b);
    fd = fopen(b, "r");
    fscanf(fd,"%[^\n] ",b);
    if(b[0]!='P'|| b[1] != '3'){
        printf("%s is not a PPM file!\n", b);
        exit(0);
    }
    printf("%s is a PPM file\n",b);
    fscanf(fd, "%c",&c);
    while(c == '#')
    {
        fscanf(fd, "%[^\n] ", b);
        printf("%s\n",b);
        fscanf(fd, "%c",&c);
        ungetc(c,fd);
    }
    
    fscanf(fd, "%d %d %d", &n, &m, &k);
    printf("%d rows %d columns max value= %d\n",n,m,k);
    nm = n*m;
    GLubyte* earth;
    GLubyte* basketball;
    for(i=nm;i>0;i--)
    {
        fscanf(fd,"%d %d %d",&red, &green, &blue );
        image[3*nm-3*i]=red;
        image[3*nm-3*i+1]=green;
        image[3*nm-3*i+2]=blue;
    }
}

// MARK: - Reshape

void reshape( int width, int height ) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    glViewport(0, 0, width, height);
    mat4 projection;
    GLfloat aspect = GLfloat(width)/height;
    if(PROJECTION_TYPE_GLOBAL != OrthographicProjectionMode) {
        projection = Perspective(45, aspect, Zclose, Zaway);
    } else {
        if(width <= height) {
            projection = Ortho(xA, yA, zA/aspect, xB/aspect, yB, zB);
        } else {
            projection = Ortho(xA*aspect, yA*aspect, zA, xB, yB, zB);
        }
    }

    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);
}

// MARK: - Menu

void mainMenu(int n) {
}

void materialMenu(int n) {
    material_shininess = n;
    glUniform1f( Shininess, material_shininess );

    if (n == Metallic) {
        material_ambient = color4( 0.1, 0.1, 0.1, 1.0 );
        material_diffuse = color4( 0.4, 0.4, 0.4, 1.0 );
        material_specular = color4( 0.7, 0.7, 0.7, 1.0 );
    } else if (n == Plastic) {
        material_ambient = color4( 0.01, 0.01, 0.01, 1.0 );
        material_diffuse = color4( 0.4, 0.4, 0.4, 1.0 );
        material_specular = color4( 0.5, 0.5, 0.5, 1.0 );
    }

    ambientP = LightColor * material_ambient;
    diffuseP = LightColor * material_diffuse;
    specularP = LightColor * material_specular;

    glUniform4fv(AmbientProduct, 1, ambientP);
    glUniform4fv(DiffuseProduct, 1, diffuseP);
    glUniform4fv(SpecularProduct, 1, specularP);

    glutPostRedisplay();
}

void phongShadingToggle(int n) {
    DRAWING_MODE_GLOBAL = n;
    glUniform1i(DrawingMode, DRAWING_MODE_GLOBAL);
    glutPostRedisplay();
}

void shadingToggle(int n) {
    DRAWING_MODE_GLOBAL = n;
    glUniform1i(DrawingMode, DRAWING_MODE_GLOBAL);
    glutPostRedisplay();
}

void projectionType(int n) {
    PROJECTION_TYPE_GLOBAL = n;

    mat4 projection;
    GLfloat aspect = GLfloat(WINDOW_WIDTH)/WINDOW_HEIGHT;
    if(PROJECTION_TYPE_GLOBAL != OrthographicProjectionMode) {
        displacement = vec3(0, 0, -2.5);
        projection = Perspective(45, aspect, Zclose, Zaway);
    } else {
        displacement = vec3(0,0,0);
        if(WINDOW_WIDTH > WINDOW_HEIGHT) {
            projection = Ortho(xA*aspect, yA*aspect, zA, xB, yB, zB);
        } else {
            projection = Ortho(xA, yA, zA/aspect, xB/aspect, yB, zB);
        }
    }

    glUniform4fv(Light1Position, 1, Translate( displacement )*LightPosition1);
    glUniform4fv(Light2Position, 1, Translate( displacement )*LightPosition2);

    glUniformMatrix4fv(Projection, 1, GL_TRUE, projection);

    glutPostRedisplay();
}

void lightProperty(int n) {
    LightsMovement = n;
}

void lightSwitch(int n) {
    if (n == DirectionalLightOn) {
        LightIsOn2 = 1;
    } else if (n == DirectionalLightOff) {
        LightIsOn2 = 0;
    } else if (n == PointLightOn) {
        LightIsOn1 = 1;
    } else if (n == PointLightOff) {
        LightIsOn1 = 0;
    }

    glUniform1i(LightToggle1, LightIsOn1);
    glUniform1i(LightToggle2, LightIsOn2);
    glutPostRedisplay();
}

void drawingMode(int n) {
    DRAWING_MODE_GLOBAL = n;
    glUniform1i(DrawingMode, DRAWING_MODE_GLOBAL);
    glutPostRedisplay();
}

void menu() {
    
}
// MARK: - Rotation Keys

void rotationKeys(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        rotZ += 2;
        if(LightsMovement == LightMoving) {
            moveLight(2, Zaxis);
        }
    } else if (key == GLUT_KEY_RIGHT) {
        rotZ -= 2;
        if(LightsMovement == LightMoving) {
            moveLight(-2, Zaxis);
        }
    } else if (key == GLUT_KEY_UP) {
        rotX += 2;
        if(LightsMovement == LightMoving) {
            moveLight(2, Xaxis);
        }
    } else if (key == GLUT_KEY_DOWN) {
        rotX -= 2;
        if(LightsMovement == LightMoving) {
            moveLight(-2, Xaxis);
        }
    }

    glutPostRedisplay();
}

//MARK: - Keyboard

void keyboard(unsigned char key, int x, int y) {
    if (key == 'z') {
        SCALE_FACTOR -= SCALE_FACTOR*0.1;
    } else if (key == 'Z') {
        SCALE_FACTOR += SCALE_FACTOR*0.1;
    } else if (key == 'i') {
        rotX = 0.0;
        rotY = 0.0;
        rotZ = 0.0;
        SCALE_FACTOR = 1.0;
    } else if (key == 'b') {
        glClearColor(0.0, 0.0, 0.0, 1.0);
    } else if (key == 'w') {
        glClearColor(1.0, 1.0, 1.0, 1.0);
    } else if (key == 'g') {
        glClearColor(0.5, 0.5, 0.5, 1.0);
    } else if (key == 'h') {
        printf("Right click to access popup menu\n");
        printf("Press i key to go to the initial position\n");
        printf("Press z or Z key for zoom in and zoom out\n");
        printf("Press up or down arrow keys for rotation in x-axis\n");
        printf("Press a or s keys for rotation in y-axis\n");
        printf("Press left or right arrow keys for rotation in z-axis\n");
        printf("Press b, g or w for Black, Grey or White background color respectively\n");
        printf("Press h or H key for help\n");
        printf("Press q or Q key to exit application\n\n");
    } else if (key == 'q') {
        exit(0);
    } else if (key == 'a') {
        rotY -= 2;
        if(LightsMovement == LightMoving) {
            moveLight(-2, Yaxis);
        }
    } else if (key == 's') {
        rotY += 2;
        if (LightsMovement == LightMoving) {
            moveLight(2, Yaxis);
        }
    }

    glutPostRedisplay();
}

// MARK: - Main

int main(int agrc, char** agrv) {
    glutInit(&agrc, agrv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(900, 720);
    glutInitWindowPosition( 100, 100 );
    glutCreateWindow("COMP410 Assignment 3");

    #ifdef __APPLE__
    #else // non-Mac OS X operating systems
    #   glewExperimental = GL_TRUE;
    #   glewInit();
    #endif

   //readPPM();
    init();
    help();
    
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc( rotationKeys );
    glutReshapeFunc( reshape );
    
    // Menu
    menuProjectionMode = glutCreateMenu(projectionType);
    glutAddMenuEntry("Orthographic" , OrthographicProjectionMode);
    glutAddMenuEntry("Perspective" , PerspectiveProjectionMode);

    menuPhong = glutCreateMenu(phongShadingToggle);
    glutAddMenuEntry("Regular Phong" , Phong);
    glutAddMenuEntry("Modified Phong" , ModifiedPhong);

    menuMaterialType = glutCreateMenu(materialMenu);
    glutAddMenuEntry("Metallic" , Metallic);
    glutAddMenuEntry("Plastic" , Plastic);

    menuShading = glutCreateMenu(shadingToggle);
    glutAddMenuEntry("Gouraud" , Gouraud);
    glutAddSubMenu("Phong" , menuPhong);

    menuDrawingMode = glutCreateMenu(drawingMode);
    glutAddMenuEntry("Wireframe" , Wireframe);
    glutAddSubMenu("Shading" , menuShading);
    glutAddMenuEntry("Texture" , Texture);

    menuLightMovement = glutCreateMenu(lightProperty);
    glutAddMenuEntry("Move with Object" , LightMoving);
    glutAddMenuEntry("Don't move with Object" , LightNotMoving);

    menuLightSwitch = glutCreateMenu(lightSwitch);
    glutAddMenuEntry("Directional On" , DirectionalLightOn);
    glutAddMenuEntry("Point On" , PointLightOn);
    glutAddMenuEntry("Directional Off" , DirectionalLightOff);
    glutAddMenuEntry("Point Off" , PointLightOff);

    glutCreateMenu(mainMenu);
    glutAddSubMenu("Projection Toggle", menuProjectionMode);
    glutAddSubMenu("Drawing Mode Toggle", menuDrawingMode);
    glutAddSubMenu("Light Movement", menuLightMovement);
    glutAddSubMenu("Light Toggle", menuLightSwitch);
    glutAddSubMenu("Material Toggle", menuMaterialType);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    glutMainLoop();
    return 0;
}

