#include <GL\glut.h>

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	//glColor3f(0.5, 0.4, 0.3);
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(10.0);
	//glLineWidth(5.0);
	glBegin(GL_POLYGON);
	glVertex3f(0.0, 1.0, 0.0);
	glVertex3f(-1.0, 0.5, 0.0);
	glVertex3f(-1.0, -0.5, 0.0);
	glVertex3f(0.0, -1.0, 0.0);
	glVertex3f(1.0, -0.5, 0.0);
	glVertex3f(1.0, 0.5, 0.0);
	glEnd();

	glFlush();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(200, 200);
	//glutCreateWindow("Graphics Primitives");

	glutCreateWindow("My First OpenGL");
	// â�� ��������� ���� �̸� ������ ����� ������ ��ǥ�� ������ �����
	glClearColor(1.0, 1.0, 1.0, 1.0); // â�� ����
	glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0); // â�� ��ǥ�� �Ǿ�

	glutDisplayFunc(MyDisplay);

	glutMainLoop();
	return 0;
}