#include <iostream>
#include <iomanip>
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

#define TEXTURE_NUM 6 // 이미지 갯수만큼 정의
GLuint texture[TEXTURE_NUM];
GLfloat Delta = 0.0, tmp=0.0, sDelta=0.0, stmp=0.0;	// Animation 효과
const GLfloat light_pos[] = { 0.0, 5.0, 0.5, 1.0 }; // 조명의 위치
GLfloat move_y = 0.0;
GLfloat move_z = 0.0;
GLfloat const_att = 1.0;
const GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };

vector3f g_vEye(-0.3f, 0.0f, 0.0f);  // Eye Position
vector3f g_vLook(0.0f, 0.0f, -1.0f); // Look Vector
vector3f g_vUp(0.0f, 1.0f, 0.0f);    // Up Vector
vector3f g_vRight(1.0f, 0.0f, 0.0f); // Right Vector

// particle - snow
int screennum = 0, winddir = 0; // 바람의 방향에 따라 눈이 내린 방향 설정
GLfloat windspeed = 0.0005f; // 눈이 얼마나 빨리 내리는지에 영향을 주기 위한 바람의 속도 설정
// 공간 이미지
char texture_name[TEXTURE_NUM][20] = { "icewall.bmp", "icewall2.bmp", "icefloor.bmp", "base.bmp", "bump.bmp", "Lawn2.bmp"}; // 주제이미지, 벽, 바닥 이미지 
float g_fDistance = -4.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

float  g_fMoveSpeed = 100.0f; // 25.0f;
float  g_fElpasedTime = 0.001f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

// particle - snow
// 눈에 해당하는 Particle을 구조체로 선언
struct particle {
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat xd, yd, zd;
	GLfloat cs;
} p[1000];

GLuint g_textureID[3];

// 카메라 시점 이동 update
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
// snow
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

	// 조명 설정
	glPushMatrix();
	glTranslatef(0.0, move_y, move_z);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, const_att);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glDisable(GL_LIGHTING);
	glutSolidSphere(0.01, 15, 15);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	updateViewMatrix();

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

	// 바닥 - 빙판길 표현
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-1.5f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.5f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.0f, 1.0f);
	glEnd();
	 
	// 사람1 - 키가 큰 사람  (어른 표현)
	// 머리
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON); // 사각형 그리기
	glTexCoord2f(0.5f, 0.0f); glVertex3f(0.0f, -0.2f + tmp, 1.0f-Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.2f, -0.2f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.2f, 0.0f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, 0.0f + tmp, 1.0f - Delta);
	glEnd();

	//목
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.05f, -0.2f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.15f, -0.2f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.15f, -0.25f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.05f, -0.25f + tmp, 1.0f - Delta);
	glEnd();
	//몸통
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.3f, -0.25f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, -0.7f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(0.1f, -0.7f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(0.1f, -0.25f + tmp, 1.0f - Delta);
	glEnd();
	//다리
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.2f, -0.7f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.2f + tmp, -1.0f, 1.0f - Delta + tmp);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.15f + tmp, -1.0f, 1.0f - Delta + tmp);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.15f, -0.7f + tmp, 1.0f - Delta);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.05f, -0.7f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.05f + tmp, -1.0f, 1.0f - Delta + tmp);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(0.0f + tmp, -1.0f, 1.0f - Delta + tmp);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, -0.7f + tmp, 1.0f - Delta);
	glEnd();
	//팔
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.3f, -0.3f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.3f, -0.4f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.4f, -0.6f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.4f, -0.5f + tmp, 1.0f - Delta);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(0.1f, -0.3f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(0.1f, -0.4f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.2f, -0.6f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(0.2f, -0.5f + tmp, 1.0f - Delta);
	glEnd();

	// 사람2 - 작은 사람 (아이 표현)
	// 머리
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON); // 사각형 그리기
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.6f, -0.5f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.8f, -0.5f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.8f, -0.3f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.6f, -0.3f + stmp, -1.0f + sDelta);
	glEnd();
	//목
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.65f, -0.5f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.75f, -0.5f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.75f, -0.55f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.65f, -0.55f + stmp, -1.0f + sDelta);
	glEnd();
	//몸통
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.85f, -0.55f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.85f, -0.9f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.55f, -0.9f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.55f, -0.55f + stmp, -1.0f + sDelta);
	glEnd();
	//다리
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.8f, -0.9f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.8f + stmp, -1.0f, -1.0f + sDelta - stmp);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.75f + stmp, -1.0f, -1.0f + sDelta - stmp);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.75f, -0.9f + stmp, -1.0f + sDelta);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.65f, -0.9f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.65f + stmp, -1.0f, -1.0f + sDelta - stmp);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.6f + stmp, -1.0f, -1.0f + sDelta - stmp);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.6f, -0.9f + stmp, -1.0f + sDelta);
	glEnd();
	//팔
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.85f, -0.65f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.85f, -0.70f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.9f, -0.90f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.9f, -0.80f + stmp, -1.0f + sDelta);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.55f, -0.65f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.55f, -0.70f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.50f, -0.90f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.50f, -0.80f + stmp, -1.0f + sDelta);
	glEnd();

	// Particle - snow 그리기
	glPushMatrix();
	// 바람의 방향과 속도에 따른 눈의 Particle들을 위한 
	// 구조체 배열 p의 각 속성들에 대한 값 설정
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

	// 눈의 Particle들을 위한 서로 다른 크기의 다각형을 생성
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

int LoadGLTextures() {   // 파일을 로드하고 텍스쳐로 변환
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
	// 눈의 Particle들을 위한 구조체 배열 p의
	// 각 속성들에 대한 초기값 설정
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
	glClearDepth(1.0f); // Depth 버퍼에 대한 명확한 값 지정
	glEnable(GL_TEXTURE_2D); // 2D Texture Mapping이 가능하도록 활성화
	glEnable(GL_BLEND); // Blending을 활성화
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Blending을 수행 / Particle들이 Alpha Blending으로 제대로 겹쳐질 수 있도록 하는 기능 수행
	glEnable(GL_DEPTH_TEST); // Depth Test 활성화
	glDepthFunc(GL_LEQUAL); // Depth 버퍼 비교를 위해 사용된 값 지정
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // 구현 고유의 힌트를 지정

	return true;
}

// 조명 설정
void MyInit() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glEnable(GL_DEPTH_TEST);
}

void MySpecial(int key, int x, int y) { // 키보드 
	vector3f tmpLook = g_vLook;
	vector3f tmpRight = g_vRight;
	vector3f tmpUp = g_vUp;

	if (key == GLUT_KEY_UP) { // 위로 이동
		g_vEye += tmpUp * -g_fMoveSpeed * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_DOWN) { // 아래로 이동
		g_vEye -= (tmpUp * -g_fMoveSpeed) * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_LEFT) { // 왼쪽으로 이동
		g_vEye += (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_RIGHT) { // 오른쪽으로 이동
		g_vEye -= (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_PAGE_UP) { // Zoom In
		g_vEye -= tmpLook * -g_fMoveSpeed * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_PAGE_DOWN) { // Zoom Out
		g_vEye += (tmpLook * -g_fMoveSpeed) * g_fElpasedTime;
	}

	glutPostRedisplay();
}

void spinDisplay(void)
{
	Delta += 0.001; // 큰 사람 z 좌표 값 증가
	if (Delta > 0.5) { // 델타 값이 0.5가 넘으면 tmp로 사람이 넘어지는 것을 표현
		tmp -= 0.001;
		if (tmp < -0.3) { // 땅에 사람이 닿으면 원상태로 돌아가 반복 
			Delta = 0.0; tmp = 0.0;
		}
	}
	sDelta += 0.001;
	if (sDelta > 0.5) {
		stmp -= 0.001;
		if (stmp < -0.1) {
			sDelta = 0.0; stmp = 0.0;
		}
	}
	glutPostRedisplay(); // 바뀐 델타 값으로 창에 그림.
}

void MyMouse(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) { // Left Button 클릭할 경우 Play
			glutIdleFunc(spinDisplay);
		}
		else
			bMousing = false;
		break;
	case GLUT_MIDDLE_BUTTON:
	case GLUT_RIGHT_BUTTON: // Right Button 클릭할 경우 Pause
		if (state == GLUT_DOWN) {
			glutIdleFunc(NULL);
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void MyTimer(int Value) {
	glutTimerFunc(50, MyTimer, 1); // 계속해서 자동적으로 애니메이션 효과 줌 => 다른 키를 누르지 않는 이상 계속
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(1024, 768); // 해상도 1024*768
	glutCreateWindow("OpenGL hw#3_20181028");
	InitGL();
	MyInit();
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutSpecialFunc(MySpecial);
	glutMouseFunc(MyMouse);
	LoadGLTextures();

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glutTimerFunc(50, MyTimer, 1); // 50msec(0.05초)
	glutMainLoop();

	return 0;
}