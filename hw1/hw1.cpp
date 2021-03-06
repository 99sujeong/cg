#include <GL\glut.h>
#include <GL\glu.h>

// 전역 변수 선언 및 초기화
GLfloat tXr = 0.0, tYr = 0.0; // 사각형
GLfloat tXt = 0.0, tYt = 0.0; // 삼각형
GLfloat tXs = 0.0, tYs = 0.0; // 별
GLboolean isRec = true, isTri=false, isStar = false;
GLfloat Delta = 0.0;	// Timer
GLUquadricObj *qobj = gluNewQuadric();

void MyDisplay() { // openGL을 담당
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBegin(GL_POLYGON); // 사각형 그리기
	glColor3f(0.5, 1.0, 0.0); // Green
	glVertex3f(-1.5 + tXr, 0.5 + tYr, 0.0);
	glVertex3f(-0.3 + tXr, 0.5 + tYr, 0.0);
	glColor3f(0.5, 0.0, 1.0);   // purple
	glVertex3f(-0.3 + tXr, 1.3 + tYr, 0.0);
	glVertex3f(-1.5 + tXr, 1.3 + tYr, 0.0);
	glEnd();

	glBegin(GL_TRIANGLES); // 삼각형 그리기
	glColor3f(0.5, 0.1, 1.0);   // purple
	glVertex3f(0.5 + tXt, -0.5 + tYt, 0.0);
	glVertex3f(1.5 + tXt, -0.5 + tYt, 0.0);
	glVertex3f(1.0 + tXt, 0.5 + tYt, 0.0);
	glEnd();
	
	glBegin(GL_POLYGON); // 별 그리기
	glColor3f(1.0, 0.9, 0.0); // Yellow
	glVertex3f(1.0 + tXs, 0.7 + tYs, 0.0);
	glVertex3f(1.3 + tXs, 0.4 + tYs, 0.0);
	glVertex3f(1.2 + tXs, 0.9 + tYs, 0.0);
	glVertex3f(1.5 + tXs, 1.1 + tYs, 0.0);
	glVertex3f(1.1 + tXs, 1.1 + tYs, 0.0);
	glVertex3f(1.0 + tXs, 1.5 + tYs, 0.0);
	glVertex3f(0.9 + tXs, 1.1 + tYs, 0.0);
	glVertex3f(0.5 + tXs, 1.1 + tYs, 0.0);
	glVertex3f(0.8 + tXs, 0.9 + tYs, 0.0);
	glVertex3f(0.7 + tXs, 0.4 + tYs, 0.0);
	glEnd();

	glPushMatrix(); // 주전자의 위치를 변경하기 위해 push, pop matrix 사용
	glColor3f(0.8, 0.0 + Delta, 0.7); // 색 변경
	glTranslatef(-1.5, -0.5, 0.0); // 주전자의 위치 변경
	glutSolidTeapot(0.5); // 주전자 그리기
	glPopMatrix();

	glColor3f(1.0, 1.0, 1.0);
	gluSphere(qobj, 0.3, 5, 5); // 오각형

	glutSwapBuffers(); // => double buffer

}

void MyReshape(int w, int h) {
	// => 창의 크기에 비례해서 창 안의 모든 내용이 모델링
	// 창의 크기에 관계없이 창안의 내용은 그대로 유지
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-2.5, 2.5, -2.5 * (GLfloat)h / (GLfloat)w, 2.5 * (GLfloat)h / (GLfloat)w, -2.5, 2.5); // 윈도우 좌표축 설정

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MyKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'Q':
	case 'q':
	case 27: // ESC
		exit(0); break; // window 종료 
	}

	glutPostRedisplay(); // => 직접 호출 하는 것이 아나라 함수를 통해 간접 호출
}

void MySpecial(int key, int x, int y) {
	// 창 안에 있는 도형을 움직이는 거 => 키를 눌렀을  키에 의해 MyDisplau에 그려져 있는 도형을 움직이고 싶은 것.
	// 도형을 움직이고 싶으면 전역 변수 선언해 변수 값을 바뀜
	switch (key) {
	case GLUT_KEY_DOWN:	// 아래방향 화살표
		if (isRec)	tYr -= 0.1;
		else if (isTri)		tYt -= 0.1;
		else if (isStar)	tYs -= 0.1;
		break;
	case GLUT_KEY_UP:	// 위방향 화살표
		if (isRec)	tYr += 0.1;
		else if (isTri)		tYt += 0.1;
		else if (isStar)		tYs += 0.1;
		break;
	case GLUT_KEY_LEFT:	// 왼쪽방향 화살표
		if (isRec)	tXr -= 0.1;
		else if (isTri)		tXt -= 0.1;
		else if (isStar)		tXs -= 0.1;
		break;
	case GLUT_KEY_RIGHT:// 오른쪽방향 화살표
		if (isRec)	tXr += 0.1;
		else if (isTri)		tXt += 0.1;
		else if (isStar)		tXs += 0.1;
		break;
	}
	glutPostRedisplay(); // 바뀐 변수 값으로 그림을 다시 그림 => 변수 값이 바뀔 때마다 도형을 다시 그림
}

void MyModelMenu(int entryID) {
	if (entryID == 1) { // 사각형 선택
		// 사각형 선택되었다고 알려줌
		isRec = true;
		isTri = false;
		isStar = false;
	}
	else if (entryID == 2) { // 삼각형 선택
		isRec = false;
		isTri = true;
		isStar = false;
	}
	else if (entryID == 3) { // 별 선택
		isRec = false;
		isTri = false;
		isStar = true;
	}
	else if (entryID == 4) { // 주전자 선택
		isRec = false;
		isTri = false;
		isStar = false;
	}
	glutPostRedisplay();
}

void MyMainMenu(int entryID) {
	if (entryID == 9) exit(0);
}

void MyTimer(int Value) {
	Delta += 0.01;
	if (Delta > 1.0) Delta = 0.0;
	glutPostRedisplay(); // 바뀐 델타 값으로 창에 그림.
	glutTimerFunc(40, MyTimer, 1); // 계속해서 자동적으로 애니메이션 효과 줌 => 다른 키를 누르지 않는 이상 계속
}

int main(int argc, char** argv) {
	glutInit(&argc, argv); // Window OS와 Session 연결 / GLUT Library를 초기화
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); 
	glutInitWindowSize(900, 900); // Window 크기 설정
	glutInitWindowPosition(200, 100); // 모니터에서 Window 시작점 위치 설정

	glutCreateWindow("OpenGL HW1"); // 새로운 Window 생성
	glClearColor(0.0, 0.0, 0.0, 1.0); // 창의 배경색 - black

	// 
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecial);

	// 메뉴 만들기
	GLint MyModelID = glutCreateMenu(MyModelMenu); //
	// ID 중복되지 않게 선언
	glutAddMenuEntry("사각형", 1); 
	glutAddMenuEntry("삼각형", 2);
	glutAddMenuEntry("별", 3);

	glutCreateMenu(MyMainMenu);

	glutAddSubMenu("모델", MyModelID); // 기존 메뉴를 서브 메뉴로 사용하고 싶을 때
	glutAddMenuEntry("Exit", 9); // 서브 메뉴에 들어가는 있는 것들은 처리할 필요없고 AddMenu에 들어온 것만 처리하면 된다. 

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutTimerFunc(40, MyTimer, 1); // 40msec(0.04초)
	glutMainLoop(); // GLUT 프로그램을 무한 이벤트 루프에 넣음
	return 0;
}
