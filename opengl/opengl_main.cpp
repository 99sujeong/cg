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
	// 창이 만들어지기 전에 미리 설정을 해줘야 배경색과 좌표축 선언이 적용됨
	glClearColor(1.0, 1.0, 1.0, 1.0); // 창의 배경색
	glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0); // 창의 좌표축 션언

	glutDisplayFunc(MyDisplay);

	glutMainLoop();
	return 0;
}
