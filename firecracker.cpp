//-----------------------------------------------------------------------------
//   Control Keys: Up         - View moves forward
//                 Down       - View moves backward
//                 Left       - View strafes left
//                 Right      - View strafes Right
//                 Left Mouse - Perform looking
//                 Mouse      - Look about the scene
//                 F1         - Change active particle system
//                 F2         - Increase particle size
//                 F3         - Decrease particle size
//-----------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <GL\glaux.h>  // GL 폴더 추가, 추가 종속성에 glaux.lib 추가
#include <GL\glut.h>
#include <GL\glu.h>
#include "matrix_4x4f.h"    // 헤더파일에 추가 -> 기존 항목
#include "particlesystem.h"
// 추가 종속성에 legacy_stdio_definitions.lib
// 속성 -> 고급 -> 문자집합 -> 멀티바이트 문자집합

GLuint g_textureID = -1;

static POINT	ptLastMousePosit;
static POINT	ptCurrentMousePosit;
static bool		bMousing = false;

//float g_fDistance = -4.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

float  g_fMoveSpeed = 100.0f; // 25.0f;
float  g_fElpasedTime = 0.001f;

vector3f g_vEye(0.0f, 3.0f, 15.0f);  // Eye Position
vector3f g_vLook(0.0f, 0.0f, -1.0f); // Look Vector
vector3f g_vUp(0.0f, 1.0f, 0.0f);    // Up Vector
vector3f g_vRight(1.0f, 0.0f, 0.0f); // Right Vector

struct Vertex {
    float tu, tv;
    float x, y, z;
};

Vertex g_floorVertices[] = {
    { 0.0f,1.0f, -25.0f, 0.0f, -15.0f },
    { 1.0f,1.0f,  25.0f, 0.0f, -15.0f },
    { 1.0f,0.0f,  25.0f, 0.0f,  15.0f },
    { 0.0f,0.0f, -25.0f, 0.0f,  15.0f }
};

int g_nActiveSystem = 1;
CParticleSystem* g_pParticleSystems;

void loadTexture(void) {
    AUX_RGBImageRec* pTextureImage = auxDIBImageLoad("seoul.bmp");

    if (pTextureImage != NULL) {
        glGenTextures(1, &g_textureID);

        glBindTexture(GL_TEXTURE_2D, g_textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX, pTextureImage->sizeY, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data);
    }

    if (pTextureImage) {
        if (pTextureImage->data)
            free(pTextureImage->data);

        free(pTextureImage);
    }
}

void Init(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    loadTexture();
}

void InitParticles(void) {
    g_pParticleSystems = new CParticleSystem();

    g_pParticleSystems->SetTexture((char *)"particle.bmp");
    g_pParticleSystems->SetMaxParticles(500);
    g_pParticleSystems->SetNumToRelease(500);
    g_pParticleSystems->SetReleaseInterval(0.1f);
    g_pParticleSystems->SetLifeCycle(1.0f);
    g_pParticleSystems->SetSize(30.0f);
    g_pParticleSystems->SetColor(vector3f(1.0f, 0.0f, 0.0f));
    g_pParticleSystems->SetPosition(vector3f(0.0f, 5.0f, 0.0f));
    g_pParticleSystems->SetVelocity(vector3f(0.0f, 0.0f, 0.0f));
    g_pParticleSystems->SetGravity(vector3f(0.0f, 0.0f, 0.0f));
    g_pParticleSystems->SetWind(vector3f(0.0f, 0.0f, 0.0f));
    g_pParticleSystems->SetVelocityVar(10.0f);

    g_pParticleSystems->Init();
}

//-----------------------------------------------------------------------------
// Name : updateViewMatrix() 
// Desc : Builds a view matrix suitable for OpenGL.
//
// Here's what the final view matrix should look like:(교재 501 페이지)
//
//  |  rx   ry   rz  -(r.e) |
//  |  ux   uy   uz  -(u.e) |
//  | -lx  -ly  -lz   (l.e) |
//  |   0    0    0     1   |
//
// Where r = Right vector
//       u = Up vector
//       l = Look vector
//       e = Eye position in world space
//       . = Dot-product operation
//
//-----------------------------------------------------------------------------
void updateViewMatrix(void) {
    matrix4x4f view;
    view.identity();

    g_vLook.normalize();

    g_vRight = vector3f::crossProduct(g_vLook, g_vUp);
    g_vRight.normalize();

    g_vUp = vector3f::crossProduct(g_vRight, g_vLook);
    g_vUp.normalize();

    view.m[0] = g_vRight.x;
    view.m[1] = g_vUp.x;
    view.m[2] = -g_vLook.x;
    view.m[3] = 0.0f;

    view.m[4] = g_vRight.y;
    view.m[5] = g_vUp.y;
    view.m[6] = -g_vLook.y;
    view.m[7] = 0.0f;

    view.m[8] = g_vRight.z;
    view.m[9] = g_vUp.z;
    view.m[10] = -g_vLook.z;
    view.m[11] = 0.0f;

    view.m[12] = -vector3f::dotProduct(g_vRight, g_vEye);
    view.m[13] = -vector3f::dotProduct(g_vUp, g_vEye);
    view.m[14] = vector3f::dotProduct(g_vLook, g_vEye);
    view.m[15] = 1.0f;

    glMultMatrixf(view.m);
}

void MyDisplay(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //  glTranslatef( 0.0f, 0.0f, g_fDistance );
    glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
    glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

    updateViewMatrix();

    // The particle system will need to know how much time has passed since 
    // the last time it was updated, so we'll need to keep track of how much   
    // time has elapsed since the last frame update...
    g_pParticleSystems->Update((float)g_fElpasedTime);

    // Render the floor...
    glBindTexture(GL_TEXTURE_2D, g_textureID);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glTranslatef(0.0, 3.0, -15.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glInterleavedArrays(GL_T2F_V3F, 0, g_floorVertices);
    glDrawArrays(GL_QUADS, 0, 4);

    // Prepare to render particle system
    // Enabling GL_DEPTH_TEST and setting glDepthMask to GL_FALSE makes the 
    // Z-Buffer read-only, which helps remove graphical artifacts generated 
    // from  rendering a list of particles that haven't been sorted by 
    // distance to the eye.
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // Enabling GL_BLEND and setting glBlendFunc to GL_DST_ALPHA with GL_ONE 
    // allows particles, which overlap, to alpha blend with each other correctly.
    glEnable(GL_BLEND);
    glBlendFunc(GL_DST_ALPHA, GL_ONE);

    // Render particle system
    glBindTexture(GL_TEXTURE_2D, g_pParticleSystems->GetTextureID());
    g_pParticleSystems->Render();

    // Reset OpenGL states...
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glutPostRedisplay();
    glutSwapBuffers();
}

void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

void MySpecial(int key, int x, int y) {
    vector3f tmpLook = g_vLook;
    vector3f tmpRight = g_vRight;

    if (key == GLUT_KEY_F1) {
        ++g_nActiveSystem;
        if (g_nActiveSystem >= 6) g_nActiveSystem = 0;
        g_pParticleSystems->RestartParticleSystem();
    }
    else if (key == GLUT_KEY_F2) {
        float fSize = g_pParticleSystems->GetSize();
        fSize += 1.0f;
        if (fSize >= g_pParticleSystems->GetMaxPointSize())
            fSize = g_pParticleSystems->GetMaxPointSize();
        g_pParticleSystems->SetSize(fSize);
    }
    else if (key == GLUT_KEY_F3) {
        float fSize = g_pParticleSystems->GetSize();
        fSize -= 1.0f;
        if (fSize <= 1.0f) fSize = 1.0f;
        g_pParticleSystems->SetSize(fSize);
    }
    else if (key == GLUT_KEY_UP) {
        g_vEye -= tmpLook * -g_fMoveSpeed * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_DOWN) {
        g_vEye += (tmpLook * -g_fMoveSpeed) * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_LEFT) {
        g_vEye -= (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_RIGHT) {
        g_vEye += (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_HOME) {
        g_vEye.y += g_fMoveSpeed * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_END) {
        g_vEye.y -= g_fMoveSpeed * g_fElpasedTime;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Particle - Firecracker");
    Init();
    InitParticles();
    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape);
    glutSpecialFunc(MySpecial);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);

    glutMainLoop();
    return 0;
}