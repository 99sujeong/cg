#include <GL\glut.h>

// ���� ���� ���� �� �ʱ�ȭ
GLfloat tXr = 0.0, tYr = 0.0; // �簢��
GLfloat tXt = 0.0, tYt = 0.0; // �ﰢ��
GLboolean isRec = true, isTri = false;
GLfloat Delta = 0.0;	// Timer

void MyDisplay() { // openGL�� ���
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POLYGON);
	glColor3f(0.0, 1.0, 0.0); // Green
	glVertex3f(-1.5+ tXr, 0.5 + tYr, 0.0);
	glVertex3f(-0.3 + tXr, 0.5 + tYr, 0.0);
	glColor3f(0.0, 0.0, 1.0);   // Blue
	glVertex3f(-0.3 + tXr, 1.3 + tYr, 0.0);
	glVertex3f(-1.5 + tXr, 1.3 + tYr, 0.0);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3f(0.5 + tXt, -0.5 + tYt, 0.0);
	glVertex3f(1.5 + tXt, -0.5 + tYt, 0.0);
	glVertex3f(1.0 + tXt, 0.5 + tYt, 0.0);
	glEnd();

	glColor3f(0.8, 0.0 + Delta, 0.7);
	glutSolidTeapot(0.5 + Delta); // ũ�� ����

	//glFlush(); // => single buffer
	glutSwapBuffers(); // => double buffer

}

void MyReshape(int w, int h) {
	// => â�� ũ�⿡ ����ؼ� â ���� ��� ������ �𵨸�
	// â�� ũ�⿡ ������� â���� ������ �״�� ����
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-2.5, 2.5, -2.5 * (GLfloat)h / (GLfloat)w, 2.5 * (GLfloat)h / (GLfloat)w, -2.5, 2.5); 
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MyKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'Q':
	case 'q':
	case 27: // ESC
		exit(0); break; // window ���� 
	}
	
	//MyDisplay(); // => �̷��� ���� ȣ���ϴ� ���� �� ��
	glutPostRedisplay(); // => ���� ȣ�� �ϴ� ���� �Ƴ��� �Լ��� ���� ���� ȣ��
}

void MySpecial(int key, int x, int y) {
	// â �ȿ� �ִ� ������ �����̴� �� => Ű�� ������ �� Ű�� ���� MyDisplau�� �׷��� �ִ� ������ �����̰� ���� ��.
	// ������ �����̰� ������ ���� ���� ������ ���� ���� �ٲ�
	switch (key) {
	case GLUT_KEY_DOWN:	// �Ʒ����� ȭ��ǥ
		if(isRec)	tYr -= 0.1;
		else if (isTri)		tYt -= 0.1;
		break;
	case GLUT_KEY_UP:	// ������ ȭ��ǥ
		if (isRec)	tYr += 0.1;
		else if (isTri)		tYt += 0.1;
		break;
	case GLUT_KEY_LEFT:	// ���ʹ��� ȭ��ǥ
		if (isRec)	tXr -= 0.1;
		else if (isTri)		tXt -= 0.1;
		break;
	case GLUT_KEY_RIGHT:// �����ʹ��� ȭ��ǥ
		if (isRec)	tXr += 0.1;
		else if (isTri)		tXt += 0.1;
		break;
	}
	glutPostRedisplay(); // �ٲ� ���� ������ �׸��� �ٽ� �׸� => ���� ���� �ٲ� ������ ������ �ٽ� �׸�
}

void MyModelMenu(int entryID) {
	if (entryID == 1) { // �簢�� ����
		// �簢�� ���õǾ��ٰ� �˷���
		isRec = true;
		isTri = false;
	}
	else if (entryID == 2) { // �ﰢ�� ����
		isRec = false;
		isTri = true;
	}
	glutPostRedisplay();
}

void MyMainMenu(int entryID) {
	if (entryID == 9) exit(0);
}

void MyTimer(int Value) {
	Delta += 0.01;
	if (Delta > 1.0) Delta = 0.0;
	glutPostRedisplay(); // �ٲ� ��Ÿ ������ â�� �׸�.
	glutTimerFunc(40, MyTimer, 1); // ����ؼ� �ڵ������� �ִϸ��̼� ȿ�� �� => �ٸ� Ű�� ������ �ʴ� �̻� ���
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowSize(900, 900);
	glutInitWindowPosition(200, 100);

	glutCreateWindow("My First Creative Graphics - OpenGL HW1");
	glClearColor(0.9, 1.0, 0.8, 1.0); // â�� ����

	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecial);
	GLint MyModelID = glutCreateMenu(MyModelMenu); // menu �����
	glutAddMenuEntry("�簢��", 1); // 1 �Ǵ� 2. ID �ߺ����� �ʰ� ����
	glutAddMenuEntry("�ﰢ��", 2);

	glutCreateMenu(MyMainMenu);

	glutAddSubMenu("��", MyModelID); // ���� �޴��� ���� �޴��� ����ϰ� ���� ��
	glutAddMenuEntry("Exit", 9); // ���� �޴��� ���� �ִ� �͵��� ó���� �ʿ���� AddMenu�� ���� �͸� ó���ϸ� �ȴ�. 

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutTimerFunc(40, MyTimer, 1); // 40msec(0.04��)
	glutMainLoop();
	return 0;
}
