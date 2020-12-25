#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL\glu.h>	// GL 폴더 추가, 추가 종속성에 glaux.lib 추가
#include <GL\glut.h>
#include <GL\glaux.h>
// 속성 -> 고급 -> 문자집합 -> 멀티바이트 문자집합
// 추가 종속성에 legacy_stdio_definitions.lib
// 속성 -> C/C++ -> 전처리기 -> 전처리기 정의 : _CRT_SECURE_NO_WARNINGS 추가

#define	MAX_PARTICLES	1000		// Number Of Particles To Create

float g_fDistance = -4.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

bool	rainbow = true;				// Rainbow Mode?

float	slowdown = 2.0f;				// Slow Down Particles
float	xspeed;						// Base X Speed (To Allow Keyboard Direction Of Tail)
float	yspeed;						// Base Y Speed (To Allow Keyboard Direction Of Tail)

GLuint	col;						// Current Color Selection
GLuint	delay;					// Rainbow Effect Delay
GLuint	texture[1];					// Storage For Our Particle Texture

typedef struct {					// Create A Structure For Particle
	bool	active;					// Active (Yes/No)
	float	life;					// Particle Life
	float	fade;					// Fade Speed
	float	r;						// Red Value
	float	g;						// Green Value
	float	b;						// Blue Value
	float	x;						// X Position
	float	y;						// Y Position
	float	z;						// Z Position
	float	xi;						// X Direction
	float	yi;						// Y Direction
	float	zi;						// Z Direction
	float	xg;						// X Gravity
	float	yg;						// Y Gravity
	float	zg;						// Z Gravity
} particles;						// Particles Structure

particles particle[MAX_PARTICLES];	// Particle Array (Room For Particle Info)

static GLfloat colors[12][3] =		// Rainbow Of Colors
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

AUX_RGBImageRec* LoadBMP(char* Filename) {				// Loads A Bitmap Image
	FILE* File = NULL;								// File Handle
	if (!Filename) {								// Make Sure A Filename Was Given
		return NULL;							// If Not Return NULL
	}
	File = fopen(Filename, "r");						// Check To See If The File Exists
	if (File) {										// Does The File Exist?
		fclose(File);								// Close The Handle
		return auxDIBImageLoad(Filename);			// Load The Bitmap And Return A Pointer
	}
	return NULL;									// If Load Failed Return NULL
}

int LoadGLTextures() {									// Load Bitmap And Convert To A Texture
	int Status = FALSE;								// Status Indicator
	AUX_RGBImageRec* TextureImage[1];				// Create Storage Space For The Textures
	memset(TextureImage, 0, sizeof(void*) * 1);		// Set The Pointer To NULL

	if (TextureImage[0] = LoadBMP((char *)"Particle.bmp")) {	// Load Particle Texture
		Status = TRUE;								// Set The Status To TRUE
		glGenTextures(1, &texture[0]);				// Create One Texture

		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[0]) {							// If Texture Exists
		if (TextureImage[0]->data) {					// If Texture Image Exists
			free(TextureImage[0]->data);			// Free The Texture Image Memory
		}
		free(TextureImage[0]);						// Free The Image Structure
	}
	return Status;									// Return The Status
}

void Init() {											// All Setup For OpenGL Goes Here
	LoadGLTextures();

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Type Of Blending To Perform
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);				// Really Nice Point Smoothing
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
	glBindTexture(GL_TEXTURE_2D, texture[0]);			// Select Our Texture

	for (int loop = 0; loop < MAX_PARTICLES; loop++) {		// Initials All The Textures
		particle[loop].active = true;									// Make All The Particles Active
		particle[loop].life = 1.0f;									// Give All The Particles Full Life
		particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;		// Random Fade Speed
		particle[loop].r = colors[loop * (12 / MAX_PARTICLES)][0];	// Select Red Rainbow Color
		particle[loop].g = colors[loop * (12 / MAX_PARTICLES)][1];	// Select Red Rainbow Color
		particle[loop].b = colors[loop * (12 / MAX_PARTICLES)][2];	// Select Red Rainbow Color
		particle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f;		// Random Speed On X Axis
		particle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;		// Random Speed On Y Axis
		particle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;		// Random Speed On Z Axis
		particle[loop].xg = 0.0f;									// Set Horizontal Pull To Zero
		particle[loop].yg = -0.8f;									// Set Vertical Pull Downward
		particle[loop].zg = 0.0f;									// Set Pull On Z Axis To Zero
	}
}

void MyDisplay() {											// Here's Where We Do All The Drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();										// Reset The ModelView Matrix

	glTranslatef(0.0f, 0.0f, g_fDistance);
	glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
	glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

	for (int loop = 0; loop < MAX_PARTICLES; loop++) {			// Loop Through All The Particles
		if (particle[loop].active) {						// If The Particle Is Active
			float x = particle[loop].x;						// Grab Our Particle X Position
			float y = particle[loop].y;						// Grab Our Particle Y Position
			float z = particle[loop].z;						// Particle Z Pos + Zoom

			// Draw The Particle Using Our RGB Values, Fade The Particle Based On It's Life
			glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);

			glBegin(GL_TRIANGLE_STRIP);						// Build Quad From A Triangle Strip
			glTexCoord2d(1, 1); glVertex3f(x + 0.5f, y + 0.5f, z); // Top Right
			glTexCoord2d(0, 1); glVertex3f(x - 0.5f, y + 0.5f, z); // Top Left
			glTexCoord2d(1, 0); glVertex3f(x + 0.5f, y - 0.5f, z); // Bottom Right
			glTexCoord2d(0, 0); glVertex3f(x - 0.5f, y - 0.5f, z); // Bottom Left
			glEnd();											// Done Building Triangle Strip

			particle[loop].x += particle[loop].xi / (slowdown * 1000);// Move On The X Axis By X Speed
			particle[loop].y += particle[loop].yi / (slowdown * 1000);// Move On The Y Axis By Y Speed
			particle[loop].z += particle[loop].zi / (slowdown * 1000);// Move On The Z Axis By Z Speed

			particle[loop].xi += particle[loop].xg;			// Take Pull On X Axis Into Account
			particle[loop].yi += particle[loop].yg;			// Take Pull On Y Axis Into Account
			particle[loop].zi += particle[loop].zg;			// Take Pull On Z Axis Into Account
			particle[loop].life -= particle[loop].fade;			// Reduce Particles Life By 'Fade'

			if (particle[loop].life < 0.0f) {					// If Particle Is Burned Out
				particle[loop].life = 1.0f;						// Give It New Life
				particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;	// Random Fade Value
				particle[loop].x = 0.0f;						// Center On X Axis
				particle[loop].y = 0.0f;						// Center On Y Axis
				particle[loop].z = 0.0f;						// Center On Z Axis
				particle[loop].xi = xspeed + float((rand() % 60) - 32.0f);	// X Axis Speed And Direction
				particle[loop].yi = yspeed + float((rand() % 60) - 30.0f);	// Y Axis Speed And Direction
				particle[loop].zi = float((rand() % 60) - 30.0f);	// Z Axis Speed And Direction
				particle[loop].r = colors[col][0];			// Select Red From Color Table
				particle[loop].g = colors[col][1];			// Select Green From Color Table
				particle[loop].b = colors[col][2];			// Select Blue From Color Table
			}
		}
	}

	glutPostRedisplay();
	glutSwapBuffers();
}

void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
		if (state == GLUT_DOWN && rainbow == true) {
			if (delay > 25) delay = 0; else delay++;
			if (col > 11) col = 0; else col++;
			rainbow = false;
		}
		else if (state == GLUT_DOWN && rainbow == false) {
			rainbow = true;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void MyMotion(int x, int y) {
	ptCurrentMousePosit.x = x;
	ptCurrentMousePosit.y = y;

	if (bMousing) {
		g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
		g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
	}

	ptLastMousePosit.x = ptCurrentMousePosit.x;
	ptLastMousePosit.y = ptCurrentMousePosit.y;

	glutPostRedisplay();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Particle - Flames");
	Init();
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);

	glutMainLoop();
	return 0;
}