#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mmsystem.h>
#include <fstream>
using namespace std;
#include <stdio.h>
#include <GL\glaux.h>
#include <GL\glut.h>
#include <GL\glu.h>
#include "matrix_4x4f.h"
#include "particlesystem.h"

#define TEXTURE_NUM 3 // 이미지 갯수만큼 정의
GLuint texture[TEXTURE_NUM];

vector3f g_vEye(-0.3f, 0.0f, 0.0f);  // Eye Position
vector3f g_vLook(0.0f, 0.0f, -1.0f); // Look Vector
vector3f g_vUp(0.0f, 1.0f, 0.0f);    // Up Vector
vector3f g_vRight(1.0f, 0.0f, 0.0f); // Right Vector

// Bump mapping
/*
GLuint     filter = 1;
GLuint     texture[3];
GLuint     bump[3];
GLuint     invbump[3];

GLfloat Gray[] = { 0.5f,0.5f,0.5f,1.0f };
*/
// particle - snow
int screennum = 0, winddir = 0; 
GLfloat windspeed = 0.0005f;

char texture_name[TEXTURE_NUM][20] = { "icewall.bmp", "icewall2.bmp", "icefloor.bmp" }; // 주제이미지, 벽, 바닥 이미지 
float g_fDistance = -4.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

float  g_fMoveSpeed = 100.0f; // 25.0f;
float  g_fElpasedTime = 0.001f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

/* // bump
GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f };
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f };

GLfloat data[] = {
	// FRONT FACE
	0.0f, 0.0f,-1.0f, -1.0f, +1.0f,
	1.0f, 0.0f,+1.0f, -1.0f, +1.0f,
	1.0f, 1.0f,+1.0f, +1.0f, +1.0f,
	0.0f, 1.0f,-1.0f, +1.0f, +1.0f,

	// BACK FACE
	1.0f, 0.0f,-1.0f, -1.0f, -1.0f,
	1.0f, 1.0f,-1.0f, +1.0f, -1.0f,
	0.0f, 1.0f,+1.0f, +1.0f, -1.0f,
	0.0f, 0.0f,+1.0f, -1.0f, -1.0f,

	// Top Face
	0.0f, 1.0f,-1.0f, +1.0f, -1.0f,
	0.0f, 0.0f,-1.0f, +1.0f, +1.0f,
	1.0f, 0.0f,+1.0f, +1.0f, +1.0f,
	1.0f, 1.0f,+1.0f, +1.0f, -1.0f,

	// Bottom Face
	1.0f, 1.0f,-1.0f, -1.0f, -1.0f,
	0.0f, 1.0f,+1.0f, -1.0f, -1.0f,
	0.0f, 0.0f,+1.0f, -1.0f, +1.0f,
	1.0f, 0.0f,-1.0f, -1.0f, +1.0f,

	// Right Face
	1.0f, 0.0f,+1.0f, -1.0f, -1.0f,
	1.0f, 1.0f,+1.0f, +1.0f, -1.0f,
	0.0f, 1.0f,+1.0f, +1.0f, +1.0f,
	0.0f, 0.0f,+1.0f, -1.0f, +1.0f,

	// Left Face
	0.0f, 0.0f,-1.0f, -1.0f, -1.0f,
	1.0f, 0.0f,-1.0f, -1.0f,  1.0f,
	1.0f, 1.0f,-1.0f,  1.0f,  1.0f,
	0.0f, 1.0f,-1.0f,  1.0f, -1.0f
};
*/

// snow
struct particle {
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat xd, yd, zd;
	GLfloat cs;
} p[1000];

GLuint g_textureID[3];

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

void LoadTexture(void) {
	AUX_RGBImageRec* pTextureImage[3];
	memset(pTextureImage, 0, sizeof(void*) * 3);

	if ((pTextureImage[0] = auxDIBImageLoad("SnowSmall.bmp")) &&
		(pTextureImage[1] = auxDIBImageLoad("SnowMiddle.bmp")) &&
		(pTextureImage[2] = auxDIBImageLoad("SnowLarge.bmp"))) {

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

void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	glTranslatef(0.0f, 0.0f, g_fDistance);
	glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
	glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

	updateViewMatrix();

	glPushMatrix();
	// 뒷 벽
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);		// 텍스쳐이미지의 Texel 좌표 4개와 4개의 Vertex로 구성된 사각형 객체의 Vertex 좌표들을 일대일로 연결
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.5f, 1.0f, -1.0f);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glEnd();

	// 오른쪽 벽
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.8f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.8f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.8f, 0.8f); glVertex3f(1.0f, 1.0f, 1.0f);

	// 왼쪽 벽
	glTexCoord2f(0.8f, 0.8f); glVertex3f(-1.5f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.8f); glVertex3f(-1.5f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.0f, 1.0f);
	glTexCoord2f(0.8f, 0.0f); glVertex3f(-1.5f, -1.0f, -1.0f);
	glEnd();

	// 바닥
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-1.5f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.5f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.0f, 1.0f);
	glEnd();

	glPopMatrix();
	/*
	glPushMatrix();
	glBegin(GL_POLYGON); // 사각형 그리기
	glVertex3f(-0.67f, -0.13f, 1.0f); // wall(벽)에서 0.01만큼 띄우기
	glVertex3f(-1.43f, -0.13f, 1.0f);
	glVertex3f(-1.43f, 0.73f, 1.0f);
	glVertex3f(-0.67f, 0.73f, 1.0f);
	glEnd();
	glPopMatrix();
	*/
	// snow
	glPushMatrix();
	for (int i = 0; i < 1000; i++) {
		p[i].x += cos(winddir * .0174532925f) * windspeed;
		p[i].y += p[i].yd;
		p[i].z += sin(winddir * .0174532925f) * windspeed;

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

AUX_RGBImageRec * LoadBMP(char *Filename) {
	FILE * File = NULL;
	if (!Filename) return NULL;
	File = fopen(Filename, "r");
	if (File) {
		fclose(File);
		return auxDIBImageLoad(Filename);	     // 파일로부터 메모리로
	}
	return NULL;
}

int LoadGLTextures() {                                   // 파일을 로드하고 텍스쳐로 변환
	int Status = TRUE, i;
	AUX_RGBImageRec *TextureImage;

	for (i = 0; i < TEXTURE_NUM; i++) {
		TextureImage = NULL;
		if (TextureImage = LoadBMP(texture_name[i])) {
			glGenTextures(1, &texture[i]);
			glBindTexture(GL_TEXTURE_2D, texture[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY,
				0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
		}
		else {
			MessageBox(NULL, "Image file has a error !", texture_name[i], MB_ICONERROR);
			Status = false;
		}

		if (TextureImage) {
			if (TextureImage->data) free(TextureImage->data);
			free(TextureImage);
		}
	}
	return Status;
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
	vector3f tmpLook = g_vLook;
	vector3f tmpRight = g_vRight;
	vector3f tmpUp = g_vUp;

	if (key == GLUT_KEY_UP) {
		//g_vEye += tmpUp * -g_fMoveSpeed * g_fElpasedTime;
		g_vEye -= tmpLook * -g_fMoveSpeed * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_DOWN) {
		//g_vEye -= (tmpUp * -g_fMoveSpeed) * g_fElpasedTime;
		g_vEye += (tmpLook * -g_fMoveSpeed) * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_LEFT) {
		g_vEye += (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_RIGHT) {
		g_vEye -= (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_PAGE_UP) {
		g_vEye -= tmpLook * -g_fMoveSpeed * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_PAGE_DOWN) {
		g_vEye += (tmpLook * -g_fMoveSpeed) * g_fElpasedTime;
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

void main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(1024, 768); // 해상도 1024*768
	glutCreateWindow("OpenGL hw#3_20181028");
	InitGL();
	//InitParticle();
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutSpecialFunc(MySpecial);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	LoadGLTextures();

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glutMainLoop();
}