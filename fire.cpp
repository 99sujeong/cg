#include <windows.h>          
#include <stdio.h>    
#include <math.h>            
#include <GL\glaux.h>   // GL 폴더 추가, 추가 종속성에 glaux.lib 추가
#include <GL\glut.h>                     
#include <GL\glu.h>
// 추가 종속성에 legacy_stdio_definitions.lib
// 속성 -> C/C++ -> 전처리기 -> 전처리기 정의 : _CRT_SECURE_NO_WARNINGS 추가

#define MAX_PARTICLES   1000                   
float slowdown = 2.0f, xspeed, yspeed, zoom = -40.0f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

typedef struct {
    bool    active;
    float   life;
    float   fade;
    float   r;
    float   g;
    float   b;
    float   x;
    float   y;
    float   z;
    float   xi;
    float   yi;
    float   zi;
    float   xg;
    float   yg;
    float   zg;
}
particles;
particles particle[MAX_PARTICLES];

GLuint      texture[1];

AUX_RGBImageRec* LoadBMP(CHAR* Filename) {
    FILE* File = NULL;
    if (!Filename) {
        return NULL;
    }
    File = fopen(Filename, "r");
    if (File) {
        fclose(File);
        return auxDIBImageLoadA(Filename);
    }
    return NULL;
}

void LoadGLTextures() {
    AUX_RGBImageRec* TextureImage[1];
    memset(TextureImage, 0, sizeof(void*) * 1);

    if (TextureImage[0] = LoadBMP((char *)"Particle.bmp")) {
        glGenTextures(1, &texture[0]);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    if (TextureImage[0]) {
        if (TextureImage[0]->data) {
            free(TextureImage[0]->data);
        }

        free(TextureImage[0]);
    }
}

int InitGL(GLvoid) {
    LoadGLTextures();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glDisable(GL_DEPTH_TEST);

    for (int loop = 0; loop < MAX_PARTICLES; loop++) {
        particle[loop].active = true;
        particle[loop].life = 1.0f;
        particle[loop].fade = float(rand() % 80) / 1000.0f + 0.002f;
        particle[loop].r = 1.0f;
        particle[loop].g = 0.3f;
        particle[loop].b = 0.2f;
        particle[loop].xi = float((rand() % 50) - 26.0f);
        particle[loop].yi = float((rand() % 50) - 25.0f);
        particle[loop].zi = float((rand() % 50) - 25.0f);
        particle[loop].xg = 0.0f;   // 불 방향 : 위(y축)로 향함
        particle[loop].yg = 0.6f;
        particle[loop].zg = 0.0f;
    }
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    return TRUE;
}

void DrawGLScene(GLvoid) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
    glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

    for (int loop = 0; loop < MAX_PARTICLES; loop++) {
        if (particle[loop].active) {
            float x = particle[loop].x;
            float y = particle[loop].y;
            float z = particle[loop].z + zoom;

            glBindTexture(GL_TEXTURE_2D, texture[0]);
            glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);

            glBegin(GL_TRIANGLE_FAN);
            glTexCoord2d(0, 0); glVertex3f(x - 0.7f, y - 2.7f, z);
            glTexCoord2d(1, 0); glVertex3f(x + 0.7f, y - 2.7f, z);
            glTexCoord2d(1, 1); glVertex3f(x + 0.7f, y + 0.7f, z);
            glTexCoord2d(0, 1); glVertex3f(x - 0.7f, y + 0.7f, z);
            glEnd();

            particle[loop].x += particle[loop].xi / (slowdown * 1000);
            particle[loop].y += particle[loop].yi / (slowdown * 1000);
            particle[loop].z += particle[loop].zi / (slowdown * 1000);
            particle[loop].xi += particle[loop].xg;
            particle[loop].yi += particle[loop].yg;
            particle[loop].zi += particle[loop].zg;
            particle[loop].life -= particle[loop].fade;

            if (particle[loop].life < 0.0f) {
                particle[loop].life = 1.0f;
                particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;
                if (loop % 2 == 0) {
                    particle[loop].x = 1.0f;
                    particle[loop].y = 0.0f;
                    particle[loop].z = 0.0f;
                }
                if (loop % 2 == 1) {
                    particle[loop].x = 0.0f;
                    particle[loop].y = 0.0f;
                    particle[loop].z = 0.0f;
                }
                particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);
                particle[loop].yi = yspeed + float((rand() % 60) - 30.0f);
                particle[loop].zi = float((rand() % 60) - 30.0f);
                particle[loop].r = 1.0f;
                particle[loop].g = 0.3f;
                particle[loop].b = 0.2f;
            }
        }
    }

    glutPostRedisplay();
    glutSwapBuffers();
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

void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Particle - Fire");
    InitGL();
    glutDisplayFunc(DrawGLScene);
    glutReshapeFunc(MyReshape);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);
    glutMainLoop();
}