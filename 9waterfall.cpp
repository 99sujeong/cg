#include <windows.h>		
#include <stdlib.h>
#include <stdio.h>
#include <olectl.h>
#include <math.h>
#include <time.h>   
#include <GL\glaux.h>		// GL 폴더 추가, 추가 종속성에 glaux.lib 추가
#include <GL\glut.h>	
#include <GL\glu.h>	
// 추가 종속성에 legacy_stdio_definitions.lib
// 속성 -> 고급 -> 문자집합 -> 멀티바이트 문자집합

#define MAX_PARTICLES		1000    
#define MAX_TAIL			100  

typedef struct {
	float   life;
	float   fade;
	float   r, g, b;
	float   x, y, z;
	float   xSpeed, ySpeed, zSpeed;
	float   xg, yg, zg;
} PARTICLES;
PARTICLES particle[MAX_PARTICLES][MAX_TAIL];
PARTICLES bubble[MAX_PARTICLES];

GLint TailLen;
GLuint g_textureID[2];
GLfloat speedFator = 0.033f;

float g_fDistance = 1.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

void LoadTexture(void) {
	AUX_RGBImageRec* pTextureImage[2];
	memset(pTextureImage, 0, sizeof(void*) * 2);

	if ((pTextureImage[0] = auxDIBImageLoad("Particle.bmp")) &&
		(pTextureImage[1] = auxDIBImageLoad("Waterfall.bmp"))) {

		for (int i = 0; i < 2; i++) {
			glGenTextures(1, &g_textureID[i]);
			glBindTexture(GL_TEXTURE_2D, g_textureID[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage[i]->sizeX, pTextureImage[i]->sizeY, 0,
				GL_RGB, GL_UNSIGNED_BYTE, pTextureImage[i]->data);
		}
	}

	for (int i = 0; i < 2; i++) {
		if (pTextureImage[i]) {
			if (pTextureImage[i]->data)		free(pTextureImage[i]->data);
			free(pTextureImage[i]);
		}
	}
}

void DrawBubble(void) {
	float x, y, z;

	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	for (int loop = 0; loop < MAX_PARTICLES; loop++) {
		x = bubble[loop].x + 25.0f;
		y = bubble[loop].y - 40.0f;
		z = bubble[loop].z;

		glColor4f(0.5, 0.5, 0.5, bubble[loop].life);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(x + 0.5f, y + 0.5f, z);
		glTexCoord2f(0.0f, 1.0f);	glVertex3f(x - 0.5f, y + 0.5f, z);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f(x + 0.5f, y - 0.5f, z);
		glTexCoord2f(0.0f, 0.0f);	glVertex3f(x - 0.5f, y - 0.5f, z);
		glEnd();

		bubble[loop].x += bubble[loop].xSpeed / (speedFator * 1000.0f);
		bubble[loop].y += bubble[loop].ySpeed / (speedFator * 1000.0f);
		bubble[loop].z += bubble[loop].zSpeed / (speedFator * 1000.0f);

		bubble[loop].xSpeed += bubble[loop].xg / (speedFator * 1000.0f);
		bubble[loop].ySpeed += bubble[loop].yg / (speedFator * 1000.0f);
		bubble[loop].zSpeed += bubble[loop].zg / (speedFator * 1000.0f);

		bubble[loop].life -= bubble[loop].fade;

		if (bubble[loop].life == 0.0f) {
			bubble[loop].life = 1.0f;
			bubble[loop].fade = ((-1) * float(rand() % 3) + 96) / 10000;

			bubble[loop].x = float(rand() % 35) - 35.0f;
			bubble[loop].y = float(rand() % 2) + 20.0f;
			bubble[loop].z = float(-1 * (rand() % 10));

			bubble[loop].xSpeed = 0.0f + float(rand() % 2);
			bubble[loop].ySpeed = 1.0f;
			bubble[loop].zSpeed = 0.0f;

			bubble[loop].xg = 0.0f;
			bubble[loop].yg = 1.0f;
			bubble[loop].zg = 0.0f;
		}
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}

void DrawParticle(void) {
	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	for (int loop = 0; loop < MAX_PARTICLES; loop++) {
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, g_textureID[0]);

		for (TailLen = MAX_TAIL - 1; TailLen > 0; TailLen--) {
			if (particle[loop][TailLen].y > -23.0f) {
				//glColor4f(0.35, 0.35, 0.8, 0.8);    
				float k = (particle[loop][TailLen].y + 29) * 0.01590909 + 0.00454545; //CHANGE COLOR ACCORDING TO Y-AXES     
				glColor4f(k, k, 0.8, 0.8);
			}
			else {
				glColor4f(0.7, 0.7, 0.9, 0.9);
			}

			glBegin(GL_TRIANGLE_STRIP);     // Draw Tail    
			glTexCoord2f(1.0f, 1.0f); glVertex3f(particle[loop][TailLen].x + 0.3f, particle[loop][TailLen].y + 0.3f, particle[loop][TailLen].z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(particle[loop][TailLen].x - 0.3f, particle[loop][TailLen].y + 0.3f, particle[loop][TailLen].z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(particle[loop][TailLen].x + 0.3f, particle[loop][TailLen].y - 0.3f, particle[loop][TailLen].z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(particle[loop][TailLen].x - 0.3f, particle[loop][TailLen].y - 0.3f, particle[loop][TailLen].z);
			glEnd();

			particle[loop][TailLen].x = particle[loop][TailLen - 1].x;
			particle[loop][TailLen].y = particle[loop][TailLen - 1].y;
			particle[loop][TailLen].z = particle[loop][TailLen - 1].z;
		}
		glEnable(GL_DEPTH_TEST);

		particle[loop][0].x += particle[loop][0].xSpeed / (speedFator * 1000.0f);
		particle[loop][0].y += particle[loop][0].ySpeed / (speedFator * 1000.0f);
		particle[loop][0].z += particle[loop][0].zSpeed / (speedFator * 1000.0f);

		particle[loop][0].xSpeed += particle[loop][0].xg / (speedFator * 1000.0f);
		particle[loop][0].ySpeed += particle[loop][0].yg / (speedFator * 1000.0f);
		particle[loop][0].zSpeed += particle[loop][0].zg / (speedFator * 1000.0f);

		particle[loop][0].life -= particle[loop][0].fade;

		if (particle[loop][TailLen].y < -18) {             // 폭포수가 수면에 도착하는 순간
			particle[loop][TailLen].xSpeed = rand() % 2 + 1;
			particle[loop][TailLen].ySpeed = rand() % 2 + 8;
			particle[loop][TailLen].zSpeed = rand() % 2 + 2;
		}

		if (particle[loop][0].life == 0.0f) {
			particle[loop][0].life = 1.0f;
			particle[loop][0].fade = ((-1) * float(rand() % 3) + 96) / 10000;

			particle[loop][0].x = float(rand() % 35) - 35.0f;
			particle[loop][0].y = float(rand() % 2) + 20;
			particle[loop][0].z = float(-1 * (rand() % 10));

			particle[loop][0].xSpeed = 8.0f + float(rand() % 2);
			particle[loop][0].ySpeed = 0;
			particle[loop][0].zSpeed = 0;

			particle[loop][0].xg = 0.0f;
			particle[loop][0].yg = -9.8f;
			particle[loop][0].zg = 0.0f;
		}
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}

void DrawMountain(void) {
	glPushMatrix();
	//  glBlendFunc(GL_SRC_ALPHA, GL_ONE);    
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, g_textureID[1]);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glColor4f(0.35f, 0.65f, 1.0f, 0.55);
	glTexCoord2f(0.0f, 0.0f);   glVertex3f(-50, -18, -10);
	glTexCoord2f(1.0f, 0.0f);   glVertex3f(50, -18, -10);

	glColor4f(1.0f, 1.0f, 1.0f, 0.75);
	glTexCoord2f(1.0f, 1.0f);   glVertex3f(50, 35, -10);
	glTexCoord2f(0.0f, 1.0f);   glVertex3f(-50, 35, -10);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void DrawPool(void) {
	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glBegin(GL_QUADS);
	glColor4f(0.35f, 0.65f, 1.0f, 0.7);
	glVertex3f(-50.0f, -18.0f, 40.0f);
	glVertex3f(50.0f, -18.0f, 40.0f);

	glColor4f(0.0f, 0.0f, 0.0f, 0.7);
	glVertex3f(50.0f, -18.0f, -10.0f);
	glVertex3f(-50.0f, -18.0f, -10.0f);
	glEnd();

	glDisable(GL_BLEND);
	glPopMatrix();
}

void DrawGLScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	gluLookAt(0.0, 0.0, 75.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glTranslatef(0.0f, 0.0f, g_fDistance);
	glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
	glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

	DrawPool();
	DrawMountain();
	DrawParticle();
	DrawBubble();

	glFlush();
	glutPostRedisplay();
	glutSwapBuffers();
}

void InitGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	srand((unsigned)time(NULL));

	LoadTexture();
	//    ParticleInit();    
	//    BubbleInit();   
}

void MySpecial(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		g_fDistance -= 1.1f;
	}
	else if (key == GLUT_KEY_DOWN) {
		g_fDistance += 1.1f;
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
	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Particle - Waterfall");
	InitGL();
	glutDisplayFunc(DrawGLScene);
	glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	glutSpecialFunc(MySpecial);
	glutMainLoop();
}