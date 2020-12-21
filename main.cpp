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

#define TEXTURE_NUM 6 // �̹��� ������ŭ ����
GLuint texture[TEXTURE_NUM];
GLfloat Delta = 0.0, tmp=0.0, sDelta=0.0, stmp=0.0;	// Animation ȿ��
const GLfloat light_pos[] = { 0.0, 5.0, 0.5, 1.0 }; // ������ ��ġ
GLfloat move_y = 0.0;
GLfloat move_z = 0.0;
GLfloat const_att = 1.0;
const GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };

vector3f g_vEye(-0.3f, 0.0f, 0.0f);  // Eye Position
vector3f g_vLook(0.0f, 0.0f, -1.0f); // Look Vector
vector3f g_vUp(0.0f, 1.0f, 0.0f);    // Up Vector
vector3f g_vRight(1.0f, 0.0f, 0.0f); // Right Vector

// particle - snow
int screennum = 0, winddir = 0; // �ٶ��� ���⿡ ���� ���� ���� ���� ����
GLfloat windspeed = 0.0005f; // ���� �󸶳� ���� ���������� ������ �ֱ� ���� �ٶ��� �ӵ� ����
// ���� �̹���
char texture_name[TEXTURE_NUM][20] = { "icewall.bmp", "icewall2.bmp", "icefloor.bmp", "base.bmp", "bump.bmp", "Lawn2.bmp"}; // �����̹���, ��, �ٴ� �̹��� 
float g_fDistance = -4.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

float  g_fMoveSpeed = 100.0f; // 25.0f;
float  g_fElpasedTime = 0.001f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

// particle - snow
// ���� �ش��ϴ� Particle�� ����ü�� ����
struct particle {
	GLfloat x, y, z;
	GLfloat r, g, b;
	GLfloat xd, yd, zd;
	GLfloat cs;
} p[1000];

GLuint g_textureID[3];

// ī�޶� ���� �̵� update
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

	// ���� ����
	glPushMatrix();
	glTranslatef(0.0, move_y, move_z);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, const_att);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glDisable(GL_LIGHTING);
	glutSolidSphere(0.01, 15, 15);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	updateViewMatrix();

	// �� ��
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);		// �ؽ����̹����� Texel ��ǥ 4���� 4���� Vertex�� ������ �簢�� ��ü�� Vertex ��ǥ���� �ϴ��Ϸ� ����
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.5f, 1.0f, -1.0f);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glEnd();

	// ������ ��
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.8f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.8f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.8f, 0.8f); glVertex3f(1.0f, 1.0f, 1.0f);

	// ���� ��
	glTexCoord2f(0.8f, 0.8f); glVertex3f(-1.5f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.8f); glVertex3f(-1.5f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.0f, 1.0f);
	glTexCoord2f(0.8f, 0.0f); glVertex3f(-1.5f, -1.0f, -1.0f);
	glEnd();

	// �ٴ� - ���Ǳ� ǥ��
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-1.5f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.5f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.0f, 1.0f);
	glEnd();
	 
	// ���1 - Ű�� ū ���  (� ǥ��)
	// �Ӹ�
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON); // �簢�� �׸���
	glTexCoord2f(0.5f, 0.0f); glVertex3f(0.0f, -0.2f + tmp, 1.0f-Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.2f, -0.2f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.2f, 0.0f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, 0.0f + tmp, 1.0f - Delta);
	glEnd();

	//��
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.05f, -0.2f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.15f, -0.2f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.15f, -0.25f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.05f, -0.25f + tmp, 1.0f - Delta);
	glEnd();
	//����
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.3f, -0.25f + tmp, 1.0f - Delta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, -0.7f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(0.1f, -0.7f + tmp, 1.0f - Delta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(0.1f, -0.25f + tmp, 1.0f - Delta);
	glEnd();
	//�ٸ�
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
	//��
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

	// ���2 - ���� ��� (���� ǥ��)
	// �Ӹ�
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON); // �簢�� �׸���
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.6f, -0.5f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.8f, -0.5f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.8f, -0.3f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.6f, -0.3f + stmp, -1.0f + sDelta);
	glEnd();
	//��
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.65f, -0.5f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.75f, -0.5f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.75f, -0.55f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.65f, -0.55f + stmp, -1.0f + sDelta);
	glEnd();
	//����
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.5f); glVertex3f(-0.85f, -0.55f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.85f, -0.9f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-0.55f, -0.9f + stmp, -1.0f + sDelta);
	glTexCoord2f(0.5f, 0.5f); glVertex3f(-0.55f, -0.55f + stmp, -1.0f + sDelta);
	glEnd();
	//�ٸ�
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
	//��
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

	// Particle - snow �׸���
	glPushMatrix();
	// �ٶ��� ����� �ӵ��� ���� ���� Particle���� ���� 
	// ����ü �迭 p�� �� �Ӽ��鿡 ���� �� ����
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

	// ���� Particle���� ���� ���� �ٸ� ũ���� �ٰ����� ����
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
		return auxDIBImageLoad(Filename);	     // ���Ϸκ��� �޸𸮷�
	}
	return NULL;
}

int LoadGLTextures() {   // ������ �ε��ϰ� �ؽ��ķ� ��ȯ
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
	// ���� Particle���� ���� ����ü �迭 p��
	// �� �Ӽ��鿡 ���� �ʱⰪ ����
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
	glClearDepth(1.0f); // Depth ���ۿ� ���� ��Ȯ�� �� ����
	glEnable(GL_TEXTURE_2D); // 2D Texture Mapping�� �����ϵ��� Ȱ��ȭ
	glEnable(GL_BLEND); // Blending�� Ȱ��ȭ
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Blending�� ���� / Particle���� Alpha Blending���� ����� ������ �� �ֵ��� �ϴ� ��� ����
	glEnable(GL_DEPTH_TEST); // Depth Test Ȱ��ȭ
	glDepthFunc(GL_LEQUAL); // Depth ���� �񱳸� ���� ���� �� ����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // ���� ������ ��Ʈ�� ����

	return true;
}

// ���� ����
void MyInit() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glEnable(GL_DEPTH_TEST);
}

void MySpecial(int key, int x, int y) { // Ű���� 
	vector3f tmpLook = g_vLook;
	vector3f tmpRight = g_vRight;
	vector3f tmpUp = g_vUp;

	if (key == GLUT_KEY_UP) { // ���� �̵�
		g_vEye += tmpUp * -g_fMoveSpeed * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_DOWN) { // �Ʒ��� �̵�
		g_vEye -= (tmpUp * -g_fMoveSpeed) * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_LEFT) { // �������� �̵�
		g_vEye += (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
	}
	else if (key == GLUT_KEY_RIGHT) { // ���������� �̵�
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
	Delta += 0.001; // ū ��� z ��ǥ �� ����
	if (Delta > 0.5) { // ��Ÿ ���� 0.5�� ������ tmp�� ����� �Ѿ����� ���� ǥ��
		tmp -= 0.001;
		if (tmp < -0.3) { // ���� ����� ������ �����·� ���ư� �ݺ� 
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
	glutPostRedisplay(); // �ٲ� ��Ÿ ������ â�� �׸�.
}

void MyMouse(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) { // Left Button Ŭ���� ��� Play
			glutIdleFunc(spinDisplay);
		}
		else
			bMousing = false;
		break;
	case GLUT_MIDDLE_BUTTON:
	case GLUT_RIGHT_BUTTON: // Right Button Ŭ���� ��� Pause
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
	glutTimerFunc(50, MyTimer, 1); // ����ؼ� �ڵ������� �ִϸ��̼� ȿ�� �� => �ٸ� Ű�� ������ �ʴ� �̻� ���
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(1024, 768); // �ػ� 1024*768
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
	glutTimerFunc(50, MyTimer, 1); // 50msec(0.05��)
	glutMainLoop();

	return 0;
}