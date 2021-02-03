#include <windows.h>		
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;
#include <GL\glaux.h>		// GL 폴더 추가, 추가 종속성에 glaux.lib 추가
#include <GL\glut.h>	
#include <GL\glu.h>	
// 속성 -> 고급 -> 문자집합 -> 멀티바이트 문자집합
// 추가 종속성에 legacy_stdio_definitions.lib

int screennum = 0, winddir = 45;
GLfloat windspeed = 0.0005f;

float g_fDistance = -2.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

struct particle {
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat xd, yd, zd;
	GLfloat cs;
} p[1000];

GLuint g_textureID;

void LoadTexture(void) {
	AUX_RGBImageRec* pTextureImage;
	//	memset(pTextureImage, 0, sizeof(void*)*3);

	pTextureImage = auxDIBImageLoad("Lawn2.bmp");

	glGenTextures(1, &g_textureID);
	glBindTexture(GL_TEXTURE_2D, g_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX, pTextureImage->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data);

	if (pTextureImage) {
		if (pTextureImage->data)		free(pTextureImage->data);
		free(pTextureImage);
	}
}

void DrawGLScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	glTranslatef(0.0f, 0.0f, g_fDistance);
	glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
	glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

	glPushMatrix();
	glColor4ub(255, 255, 255, 255);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_textureID);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.5f, -0.5f, 2.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(-1.5f, -0.5f, -2.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.5f, -0.5f, -2.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(1.5f, -0.5f, 2.0f);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	for (int i = 0; i < 1000; i++) {
		p[i].x += cos(winddir * .0174532925f) * windspeed;
		p[i].y += p[i].yd;
		p[i].z += sin(winddir * .0174532925f) * windspeed;
		p[i].yd -= rand() / 32767.0f / 100000.0f;

		if (p[i].y <= -0.5f) {
			p[i].xd = -(rand() / 32767.0f - 0.5f) / 200.0f;
			p[i].zd = -(rand() / 32767.0f - 0.5f) / 200.0f;
			p[i].yd = -rand() / 32767.0f / 100.0f;
			p[i].x = (rand() / 32767.0f - 1.0f);
			p[i].y = 1.0f;
			p[i].z = (rand() / 32767.0f - 1.0f);
			p[i].b = rand() / 32767.0f;
			p[i].g = p[i].b;
			p[i].r = p[i].b;
		}
	}

	for (int i = 0; i < 1000; i++) {
		float difx = 0.0, dify = 0.0;
		if (i % 3 == 0) {
			difx = 0.005;
			dify = 0.01;
		}
		else if (i % 3 == 1) {
			difx = 0.005;
			dify = 0.03;
		}
		else {
			difx = 0.005;
			dify = 0.05;
		}

		glDisable(GL_TEXTURE_2D);
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(p[i].x - difx, p[i].y - dify, p[i].z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(p[i].x, p[i].y - dify, p[i].z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(p[i].x, p[i].y, p[i].z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(p[i].x - difx, p[i].y, p[i].z);
		glEnd();
	}
	glPopMatrix();

	glutPostRedisplay();
	glutSwapBuffers();
}

int InitGL(void) {
	GLfloat fogColor[] = { 0.6f, 0.6f, 0.6f, 1.0f };

	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.3f);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_START, 2.5f);
	glFogf(GL_FOG_END, 3.0f);
	glEnable(GL_FOG);

	LoadTexture();

	for (int i = 0; i < 1000; i++) {
		p[i].xd = -(rand() / 32767.0f - 0.5f) / 200.0f;
		p[i].zd = -(rand() / 32767.0f - 0.5f) / 200.0f;
		p[i].yd = -rand() / 32767.0f / 100.0f;
		p[i].x = (rand() / 32767.0f - 1.0f);
		p[i].y = 1.0f;
		p[i].z = (rand() / 32767.0f - 1.0f);
		p[i].b = rand() / 32767.0f;
		p[i].g = p[i].b;
		p[i].r = p[i].b;
	}

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return true;
}

void MySpecial(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		g_fDistance -= 0.1f;
	}
	else if (key == GLUT_KEY_DOWN) {
		g_fDistance += 0.1f;
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
	glutCreateWindow("Particle - Rain");
	InitGL();
	glutDisplayFunc(DrawGLScene);
	glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	glutSpecialFunc(MySpecial);
	glutMainLoop();
}
