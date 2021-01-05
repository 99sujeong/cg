#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL\glaux.h> // GL 폴더 추가, 추가 종속성에 glaux.lib 추가
#include <GL\glut.h>
#include <GL\glu.h>
// 추가 종속성에 legacy_stdio_definitions.lib
// 속성 -> 고급 -> 문자집합 -> 멀티바이트 문자집합
// 속성 -> C/C++ -> 전처리기 -> 전처리기 정의 : _CRT_SECURE_NO_WARNINGS 추가

#define DIMX 64
#define DIMZ 128
#define GRX 60
#define GRZ 50
#define EACH 1

float g_fDistance = 0.0f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

int		each = 0, frame = 0;
float	fxx = 0, fzz = 0;

typedef struct {
    double x;
    double y;
    double z;
    double yy;
} Vector;

Vector	buf[DIMX + 1][DIMZ + 1];
GLuint	texture[4];// Storage For 3 Textures
GLUquadricObj* quadratic;// Storage For Our Quadratic Objects ( NEW )

void CreaGrid(void) {
    int x, z;
    double xd, yd, zd;
    double inx, inz;
    double incx, incz;
    double fan, an;

    inx = (double)-GRX / 2;
    inz = (double)-GRZ / 2;
    incz = (double)GRZ / DIMZ;
    incx = (double)GRX / DIMX;
    fan = ((double)360 / DIMX);
    an = 0;

    for (z = 0; z < DIMZ; z++) {
        zd = (double)z * incz + inz;

        for (x = 0; x < DIMX; x++) {
            xd = (double)x * incx + inx;
            buf[x][z].x = xd; //+sin(an*3.141/180)/10;
            buf[x][z].y = sin(an * 3.141 / 180) / 2;
            buf[x][z].yy = -4 + sin(an * 3.141 / 180) / 3;
            buf[x][z].z = zd; //+cos(an*3.141/180)/10;
        }

        an += fan;
    }
}

void ScrollQuad(void) {
    int x, z;
    double tt, tty;

    for (x = 0; x < DIMX; x++) {
        tt = buf[x][0].y;
        tty = buf[x][0].yy;

        for (z = 0; z < DIMZ - 1; z++) {
            buf[x][z].y = buf[x][z + 1].y;
            buf[x][z].yy = buf[x][z + 1].yy;
        }

        buf[x][DIMZ - 1].y = tt;
        buf[x][DIMZ - 1].yy = tty;
    }
}

void ViewFondo() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glBegin(GL_QUADS);// Draw A Quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-40, 1, -18);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-40, 20, -18);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(30, 20, -20);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(30, 1, -20);
    glEnd();// Done Drawing The Quad
    glDisable(GL_TEXTURE_2D);
}

void ViewQuad_line(void) {
    int x, z;

    for (z = 0; z < DIMZ - 1; z++) {
        for (x = 0; x < DIMX - 1; x++) {
            glBegin(GL_LINES);
            glVertex3f(buf[x][z].x, buf[x][z].y, buf[x][z].z);
            glVertex3f(buf[x + 1][z].x, buf[x + 1][z].y, buf[x + 1][z].z);
            glVertex3f(buf[x + 1][z].x, buf[x + 1][z].y, buf[x + 1][z].z);
            glVertex3f(buf[x + 1][z + 1].x, buf[x + 1][z + 1].y, buf[x + 1][z + 1].z);
            glVertex3f(buf[x + 1][z + 1].x, buf[x + 1][z + 1].y, buf[x + 1][z + 1].z);
            glVertex3f(buf[x][z + 1].x, buf[x][z + 1].y, buf[x][z + 1].z);
            glVertex3f(buf[x][z + 1].x, buf[x][z + 1].y, buf[x][z + 1].z);
            glVertex3f(buf[x][z].x, buf[x][z].y, buf[x][z].z);
            glEnd();
        }
    }
}

void ViewQuad_texture(void) {
    int x, z;
    float fx, fz;

    fx = (float)1 / DIMX;
    fz = (float)1 / DIMZ;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    //glBlendFunc( (1,1,1,1), (1,1,1,1));
    //glDepthMask (GL_FALSE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    for (z = 0; z < DIMZ - 1; z++) {
        for (x = 0; x < DIMX - 1; x++) {
            //glEnable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glBegin(GL_QUADS);
            glTexCoord2d((float)x * fx, (float)z * fz);
            //glTexCoord2d( 0,0 );
            glVertex3f(buf[x][z].x, buf[x][z].y, buf[x][z].z);
            glTexCoord2d((float)x * fx + fx, (float)z * fz);
            //glTexCoord2d( 1,0 );
            glVertex3f(buf[x + 1][z].x, buf[x + 1][z].y, buf[x + 1][z].z);
            glTexCoord2d((float)x * fx + fx, (float)z * fz + fz);
            //glTexCoord2d( 1,1 );
            glVertex3f(buf[x + 1][z + 1].x, buf[x + 1][z + 1].y, buf[x + 1][z + 1].z);
            glTexCoord2d((float)x * fx, (float)z * fz + fz);
            //glTexCoord2d( 0,1 );
            glVertex3f(buf[x][z + 1].x, buf[x][z + 1].y, buf[x][z + 1].z);
            glEnd();

            //glDisable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, texture[1]);
            glBegin(GL_QUADS);
            glTexCoord2d(fxx + (float)x * fx, fzz + (float)z * fz);
            //glTexCoord2d( 0,0 );
            glVertex3f(buf[x][z].x, buf[x][z].yy, buf[x][z].z);
            glTexCoord2d(fxx + (float)x * fx + fx, fzz + (float)z * fz);
            //glTexCoord2d( 1,0 );
            glVertex3f(buf[x + 1][z].x, buf[x + 1][z].yy, buf[x + 1][z].z);
            glTexCoord2d(fxx + (float)x * fx + fx, fzz + (float)z * fz + fz);
            //glTexCoord2d( 1,1 );
            glVertex3f(buf[x + 1][z + 1].x, buf[x + 1][z + 1].yy, buf[x + 1][z + 1].z);
            glTexCoord2d(fxx + (float)x * fx, fzz + (float)z * fz + fz);
            //glTexCoord2d( 0,1 );
            glVertex3f(buf[x][z + 1].x, buf[x][z + 1].yy, buf[x][z + 1].z);
            glEnd();
        }
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    //glDepthMask (GL_TRUE);

    //fxx+=0.001;
    //if(fxx==1) fxx=0;
    fzz += 0.001;

    if (fzz == 1)fzz = 0;
}

AUX_RGBImageRec* LoadBMP(char* Filename) {// Loads A Bitmap Image
    FILE* File = NULL;// File Handle

    if (!Filename) { // Make Sure A Filename Was Given
        return NULL;// If Not Return NULL
    }

    File = fopen(Filename, "r");// Check To See If The File Exists

    if (File) { // Does The File Exist?
        fclose(File);// Close The Handle
        return auxDIBImageLoad(Filename);// Load The Bitmap And Return A Pointer
    }

    return NULL;// If Load Failed Return NULL
}

void LoadGLTextures() { // Load Bitmaps And Convert To Textures
    AUX_RGBImageRec* TextureImage[4];// Create Storage Space For The Texture
    memset(TextureImage, 0, sizeof(void*) * 4);           // Set The Pointer To NULL

    // Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
    if ((TextureImage[0] = LoadBMP((char *)"Water.bmp")) &&
        (TextureImage[1] = LoadBMP((char*)"WaterD.bmp")) &&
        (TextureImage[2] = LoadBMP((char*)"Moon.bmp")) &&
        (TextureImage[3] = LoadBMP((char*)"Sky.bmp"))) {

        for (int i = 0; i < 4; i++) {
            glGenTextures(1, &texture[i]);// Create Three Textures

            // Create MipMapped Texture
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                GL_LINEAR_MIPMAP_NEAREST);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[i]->sizeX,
                TextureImage[i]->sizeY, GL_RGB, GL_UNSIGNED_BYTE,
                TextureImage[i]->data);
        }
    }

    for (int i = 0; i < 4; i++) {
        if (TextureImage[i]) {// If Texture Exists
            if (TextureImage[i]->data) // If Texture Image Exists
                free(TextureImage[i]->data);// Free The Texture Image Memory
            free(TextureImage[i]);// Free The Image Structure
        }
    }
}

int InitGL() { // All Setup For OpenGL Goes Here
    GLfloat fgcolor[] = { 0.0f, 0.0f, 0.0f };

    LoadGLTextures();// Jump To Texture Loading Routine
    CreaGrid();

    glShadeModel(GL_SMOOTH);// Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);// Black Background
    glClearDepth(1.0f);// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// Really Nice Perspective Calculations

    glFogfv(GL_FOG_COLOR, fgcolor);
    glFogf(GL_FOG_START, 40.0f);
    glFogf(GL_FOG_END, 56.0f);
    glFogf(GL_FOG_MODE, GL_LINEAR);
    glEnable(GL_FOG);

    quadratic = gluNewQuadric();// Create A Pointer To The Quadric Object (Return 0 If No Memory) (NEW)
    gluQuadricNormals(quadratic, GLU_SMOOTH);// Create Smooth Normals (NEW)
    gluQuadricTexture(quadratic, GL_TRUE);// Create Texture Coords (NEW)

    return TRUE;// Initialization Went OK
}

void DrawGLScene() { // Here's Where We Do All The Drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clear The Screen And The Depth Buffer
    glLoadIdentity();// Reset The View

    glTranslatef(0.0f, 0.0f, g_fDistance);
    glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
    glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

    gluLookAt(5, 8, (float)GRZ / 2 + 10, 0, 0, 0, 0.0f, 1.0f, 0.0f);

    ViewFondo();

    each++;
    if (each == EACH) {
        ScrollQuad();
        each = 0;
    }

    ViewQuad_texture();
    //ViewQuad_line();

    // Moon (luna)
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(-4, 12, -20);
    glRotatef(90, 1, 1, 0);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    gluSphere(quadratic, 4, 16, 16);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    frame++;

    glutPostRedisplay();

    glutSwapBuffers();
}

void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);// Select The Modelview Matrix
    glLoadIdentity();// Reset The Modelview Matrix
}

void MySpecial(int key, int x, int y) {
    if (key == GLUT_KEY_UP) {
        g_fDistance -= 0.5f;
    }
    else if (key == GLUT_KEY_DOWN) {
        g_fDistance += 0.5f;
    }

    glutPostRedisplay();
}

void MyMouse(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            ptLastMousePosit.x = ptCurrentMousePosit.x = x;
            ptLastMousePosit.y = ptCurrentMousePosit.y = y;
            bMousing = true;
        }
        else
            bMousing = false;
        break;
    case GLUT_MIDDLE_BUTTON:
    case GLUT_RIGHT_BUTTON:
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

void MyMotion(int x, int y) {
    ptCurrentMousePosit.x = x;
    ptCurrentMousePosit.y = y;

    if (bMousing) {
        g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
        g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
    }

    ptLastMousePosit.x = ptCurrentMousePosit.x;
    ptLastMousePosit.y = ptCurrentMousePosit.y;

    glutPostRedisplay();
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Water Effect - Wave");
    InitGL();
    glutDisplayFunc(DrawGLScene);
    glutReshapeFunc(MyReshape);
    glutSpecialFunc(MySpecial);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);

    glutMainLoop();
}