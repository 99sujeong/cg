#include <stdio.h>
#include <GL\glaux.h>
#include <GL\glut.h>
#include <GL\glu.h>

#define TEXTURE_NUM 3 // 이미지 갯수만큼 정의
GLuint texture[TEXTURE_NUM];

char texture_name[TEXTURE_NUM][20] = { "travel.bmp", "wall.bmp", "floor.bmp" }; // 주제이미지, 벽, 바닥 이미지 
float g_fDistance = -4.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

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

	// 뒷 벽
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.34f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);		// 텍스쳐이미지의 Texel 좌표 4개와 4개의 Vertex로 구성된 사각형 객체의 Vertex 좌표들을 일대일로 연결
	glTexCoord2f(0.684f, 0.0f); glVertex3f(-1.5f, -1.0f, -1.0f);		
	glTexCoord2f(0.684f, 0.5f); glVertex3f(-1.5f, 1.0f, -1.0f);
	glTexCoord2f(0.34f, 0.5f); glVertex3f(1.0f, 1.0f, -1.0f);
	
	// 오른쪽 벽
	glTexCoord2f(0.34f, 0.51f); glVertex3f(1.0f, -1.0f, 1.0f);		
	glTexCoord2f(0.683f, 0.51f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.683f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.34f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	// 바닥
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.8f, 0.0f); glVertex3f(-1.5f, -1.0f, -1.0f);		
	glTexCoord2f(0.8f, 0.5f); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.3f, 0.5f); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.3f, 0.0f); glVertex3f(-1.5f, -1.0f, 1.0f);
	glEnd();

	// 액자 구성하기
	// 액자 테두리 그리기
	// 액자 1 테두리
	glBegin(GL_POLYGON); // 사각형 그리기
	glColor3f(1.0, 0.0, 0.0); // 빨간색 테두리
	glVertex3f(-0.67f, -0.13f, -0.99f); // wall(벽)에서 0.01만큼 띄우기
	glVertex3f(-1.43f, -0.13f, -0.99f);
	glVertex3f(-1.43f, 0.73f, -0.99f);
	glVertex3f(-0.67f, 0.73f, -0.99f);
	glEnd();
	// 액자 2 테두리
	glBegin(GL_POLYGON); // 사각형 그리기
	glColor3f(0.5, 1.0, 0.5); // 탁한 초록색 테두리
	glVertex3f(0.13f, -0.73f, -0.99f);	// wall(벽)에서 0.01만큼 띄우기	
	glVertex3f(-0.63f, -0.73f, -0.99f);
	glVertex3f(-0.63f, 0.03f, -0.99f);
	glVertex3f(0.13f, 0.03f, -0.99f);
	glEnd();
	// 액자 3 테두리
	glBegin(GL_POLYGON); // 사각형 그리기
	glColor3f(0.0, 0.0, 1.0); // 파란색 테두리
	glVertex3f(0.93f, -0.83f, -0.99f);	// wall(벽)에서 0.01만큼 띄우기
	glVertex3f(0.17f, -0.83f, -0.99f);
	glVertex3f(0.17f, 0.83f, -0.99f);
	glVertex3f(0.93f, 0.83f, -0.99f);
	glEnd();
	// 액자 4 테두리
	glBegin(GL_POLYGON); // 사각형 그리기
	glColor3f(0.0, 1.0, 0.0); // 초록색 테두리
	glVertex3f(0.99f, -0.53f, 0.93f);	
	glVertex3f(0.99f, -0.53f, -0.93f);
	glVertex3f(0.99f, 0.53f, -0.93f);
	glVertex3f(0.99f, 0.53f, 0.93f);
	glColor3f(1.0, 1.0, 1.0); // 
	glEnd();


	// 액자 구성
	// 액자 이미지 넣기 
	// 액자 1 이미지 - 마카오1
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.7f, -0.1f, -0.98f); // 액자테두리로부터 0.1f 띄우기 즉, wall에서 0.2 띄움		
	glTexCoord2f(0.5f, 0.0f); glVertex3f(-1.4f, -0.1f, -0.98f);
	glTexCoord2f(0.5f, 0.49f); glVertex3f(-1.4f, 0.7f, -0.98f);
	glTexCoord2f(0.0f, 0.49f); glVertex3f(-0.7f, 0.7f, -0.98f);
	// 액자 2 이미지 - 마카오2
	glTexCoord2f(0.51f, 0.0f); glVertex3f(0.1f, -0.7f, -0.98f);	// 액자테두리로부터 0.1f 띄우기 즉, wall에서 0.2 띄움	
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.6f, -0.7f, -0.98f);
	glTexCoord2f(1.0f, 0.5f); glVertex3f(-0.6f, 0.0f, -0.98f);
	glTexCoord2f(0.51f, 0.5f); glVertex3f(0.1f, 0.0f, -0.98f);
	// 액자 3 이미지 - 마카오3
	glTexCoord2f(0.0, 0.5f); glVertex3f(0.9f, -0.8f, -0.98f);	// 액자테두리로부터 0.1f 띄우기 즉, wall에서 0.2 띄움
	glTexCoord2f(0.5f, 0.5f); glVertex3f(0.2f, -0.8f, -0.98f);
	glTexCoord2f(0.5f, 1.0f); glVertex3f(0.2f, 0.8f, -0.98f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.9f, 0.8f, -0.98f);
	// 액자 4 이미지 - 마카오4
	glTexCoord2f(0.51f, 0.51f); glVertex3f(0.98f, -0.5f, 0.9f);	// 액자테두리로부터 0.1f 띄우기 즉, wall에서 0.2 띄움
	glTexCoord2f(1.0f, 0.51f); glVertex3f(0.98f, -0.5f, -0.9f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.98f, 0.5f, -0.9f);
	glTexCoord2f(0.51f, 1.0f); glVertex3f(0.98f, 0.5f, 0.9f);
	glEnd();

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
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("OpenGL hw#2_20181028");
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
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