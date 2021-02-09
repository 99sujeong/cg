#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include <GL\glut.h>

#include "bitmap_fonts.h"
int    g_nWindowWidth = 640;
int    g_nWindowHeight = 480;

GLint fogMode;

float g_fDistance = 0.0f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

void MyInit() {
    GLfloat position[] = { 0.5, 0.5, 3.0, 0.0 };
    glEnable(GL_DEPTH_TEST);
    glLightfv(GL_LIGHT0, GL_POSITION, position); 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    {
        GLfloat mat[3] = { 0.1745, 0.01175, 0.01175 };
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
        mat[0] = 0.61424; mat[1] = 0.04136; mat[2] = 0.04136;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
        mat[0] = 0.727811; mat[1] = 0.626959; mat[2] = 0.626959;
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
        glMaterialf(GL_FRONT, GL_SHININESS, 0.6 * 128.0);
    }
    
    glEnable(GL_FOG);
    {
        GLfloat fogColor[4] = { 0.5, 0.5, 0.5, 1.0 }; //  0.8, 0.8, 0.4,1.0
        fogMode = GL_EXP;
        glFogi(GL_FOG_MODE, fogMode);
        glFogfv(GL_FOG_COLOR, fogColor);
        glFogf(GL_FOG_DENSITY, 0.35);   // 0.9
        glHint(GL_FOG_HINT, GL_DONT_CARE);
        glFogf(GL_FOG_START, 1.0);  // -5.0
        glFogf(GL_FOG_END, 5.0);
    }
    glClearColor(0.5, 0.5, 0.5, 1.0);  // fog color // 0.8, 0.8, 0.4, 1.0 
    
}

void renderSphere(GLfloat x, GLfloat y, GLfloat z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    //glutSolidSphere(0.4, 16, 16);
    glutSolidTeapot(0.4);
    glPopMatrix();
}

void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, g_fDistance);
    glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
    glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

    renderSphere(-2., -0.5, -1.0);
    renderSphere(-1., -0.5, -2.0);
    renderSphere(0., -0.5, -3.0);
    renderSphere(1., -0.5, -4.0);
    renderSphere(2., -0.5, -5.0);

    static char strFogMode[255];
    if (fogMode == GL_EXP) {
        sprintf_s(strFogMode, "GL_FOG_MODE = GL_EXP2   (Change: f,F)");
    }
    else if (fogMode == GL_EXP2) {
        sprintf_s(strFogMode, "GL_FOG_MODE = GL_LINEAR (Change: f,F)");
    }
    else if (fogMode == GL_LINEAR) {
        sprintf_s(strFogMode, "GL_FOG_MODE = GL_EXP    (Change: f,F)");
    }

    beginRenderText(g_nWindowWidth, g_nWindowHeight);
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(5, 15, BITMAP_FONT_TYPE_HELVETICA_12, strFogMode);
    }
    endRenderText();

    glFlush();
}

void MyReshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-2.5, 2.5, -2.5 * (GLfloat)h / (GLfloat)w, 2.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
        glOrtho(-2.5 * (GLfloat)w / (GLfloat)h, 2.5 * (GLfloat)w / (GLfloat)h, -2.5, 2.5, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyKeyboard(unsigned char key, int x, int y) {

    switch (key) {
    case 'f':
    case 'F':
        if (fogMode == GL_EXP) {
            fogMode = GL_EXP2;
        }
        else if (fogMode == GL_EXP2) {
            fogMode = GL_LINEAR;
        }
        else if (fogMode == GL_LINEAR) {
            fogMode = GL_EXP;
        }
        glFogi(GL_FOG_MODE, fogMode);
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }
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

    if (bMousing)
    {
        g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
        g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
    }

    ptLastMousePosit.x = ptCurrentMousePosit.x;
    ptLastMousePosit.y = ptCurrentMousePosit.y;

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Particle - Fog");
    MyInit();
    glutReshapeFunc(MyReshape);
    glutDisplayFunc(MyDisplay);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);
    glutKeyboardFunc(MyKeyboard);

    glutMainLoop();
    return 0;
}
