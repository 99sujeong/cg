#include <windows.h>		
#include <stdlib.h>
#include <math.h>
#include <fstream>
using namespace std;
#include <GL\glaux.h>	// GL 폴더 추가, 추가 종속성에 glaux.lib 추가	
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

GLuint g_textureID[3];

void LoadTexture(void) {
	AUX_RGBImageRec* pTextureImage[3];
	memset(pTextureImage, 0, sizeof(void*) * 3);

	if ((pTextureImage[0] = auxDIBImageLoad("SnowSmall.bmp")) &&
		(pTextureImage[1] = auxDIBImageLoad("SnowMiddle.bmp")) &&
		(pTextureImage[2] = auxDIBImageLoad("SnowLarge.bmp"))  ) {

		for (int i = 0; i < 3; i++) {
			glGenTextures(1, &g_textureID[i]);
			glBindTexture(GL_TEXTURE_2D, g_textureID[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage[i]->sizeX, pTextureImage[i]->sizeY, 0,
				GL_RGB, GL_UNSIGNED_BYTE, pTextureImage[i]->data);
		}
	}

	for (int i = 0; i < 3; i++) {
		if (pTextureImage[i]) {
			if (pTextureImage[i]->data)		free(pTextureImage[i]->data);
			free(pTextureImage[i]);
		}
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
	glBindTexture(GL_TEXTURE_2D, g_textureID[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, -0.5f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(-1.0f, -0.5f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, -0.5f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(1.0f, -0.5f, 1.0f);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	for (int i = 0; i < 1000; i++) {
		p[i].x += cos(winddir * .0174532925f) * windspeed;
		p[i].y += p[i].yd;
		p[i].z += sin(winddir * .0174532925f) * windspeed;
		//			p[i].yd	-= rand()/32767.0f/100000.0f;	

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
		float dif = 0.0;
		if (i % 3 == 0)		dif = 0.01;
		else if (i % 3 == 1)	dif = 0.02;
		else				dif = 0.03;
		//			glColor3f(p[i].r, p[i].g, p[i].b);
		glEnable(GL_BLEND);
		glDepthFunc(GL_LEQUAL);
		glBlendFunc(GL_ONE, GL_ONE);
		glBindTexture(GL_TEXTURE_2D, g_textureID[i % 3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(p[i].x - dif, p[i].y - dif, p[i].z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(p[i].x, p[i].y - dif, p[i].z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(p[i].x, p[i].y, p[i].z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(p[i].x - dif, p[i].y, p[i].z);
		glEnd();
	}
	glPopMatrix();

	glutPostRedisplay();
	glutSwapBuffers();
}

int InitGL(void) {
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
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	//    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
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
	glutCreateWindow("Particle - Snow");
	InitGL();
	glutDisplayFunc(DrawGLScene);
	glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	glutSpecialFunc(MySpecial);
	glutMainLoop();
}
