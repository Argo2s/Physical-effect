#include "stdafx.h"

// standard
#include <assert.h>
#include <math.h>

// glut
#include <GL/glut.h>

//================================
// global variables
//================================
// screen size
int g_screenWidth = 0;
int g_screenHeight = 0;

static int ball = 6;

GLfloat ballcoordinate[6][3] = { {7.0f,12.0f,4.0f},{1.0f,2.0f,-2.0f},{-9.0f,0.0f,0.0f},{-0.5f,0.0f,0.0f},{5.0f,0.0f,0.0f},{5.0f,0.0f,8.0f}
};
GLfloat initvelocity[6][3] = { {-2.0f,-2.0f,-1.0f},{1.0f,3.0f,2.0f},{4.0f,0.0f,0.0f},{0.5f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-3.0f}
};

GLfloat velocity[6][3] = { 0 };//This matrix is used to store the current velocity
GLfloat pos[6][3] = { 0 };//This matrix is used to store the current position; 
GLfloat pos_n[6][3];//This matrix is used to store the next position;
GLfloat dt = 0.02f;//The delta time is 0.02;
GLfloat g[3] = { 0.0f,-3.0f,0.0f };//The gravity is set to 4.0 per delta time
GLfloat ballro[6][16];//The matrix used to store the position.
GLfloat ro[16] = { 0 };//The matrix usde to implement in glMatrixMult;
GLfloat dampingidx = 0.7f;//The index used to represent that each collision with ground will become 0.65 times velocity.


void Normalize(GLfloat NM[3]) { //Normalization is used to offset the error that may be accumelate.
	GLfloat vectorlen = sqrt(NM[0] * NM[0] + NM[1] * NM[1] + NM[2] * NM[2]);
	if (vectorlen != 0) {//to prevent dividing by 0
		NM[0] /= vectorlen;
		NM[1] /= vectorlen;
		NM[2] /= vectorlen;
	}
}
GLfloat DotProduct(GLfloat V1[3], GLfloat V2[3]) {//Vector dot product
	GLfloat Result = (V1[0] * V2[0]) + (V1[1] * V2[1]) + (V1[2] * V2[2]);
	return Result;
}

GLfloat balldistance(GLfloat Vector1[3], GLfloat Vector2[3]) { //Calculate the distance between 2 balls
	GLfloat dx = Vector1[0] - Vector2[0];
	GLfloat dy = Vector1[1] - Vector2[1];
	GLfloat dz = Vector1[2] - Vector2[2];
	GLfloat d = sqrt(dx * dx + dy * dy + dz * dz);
	return d;
}

//================================
// init
//================================
void init() {
	for (int i = 0; i < ball; i++) {//initial the matrix and store the position inside the matrix
		ballro[i][0] = 1.0f;
		ballro[i][5] = 1.0f;
		ballro[i][10] = 1.0f;
		for (int j = 0; j < 3; j++) {
			ballro[i][j + 12] = ballcoordinate[i][j];
			pos[i][j] = ballro[i][j + 12];
		}
		ballro[i][15] = 1.0f;
	}
}
void Groundcol(int number) {
	if (pos[number][1] < 0.4) { //The radius is 0.4.
		initvelocity[number][1] = -dampingidx * initvelocity[number][1];//Loss speed when collide with ground.

	}
}
void Collision(int bi)
{
	// detect collision between current ball and all the other balls
	for (int i = bi + 1; i < ball; i++)
	{
		if (balldistance(pos[bi], pos[i]) < 0.8) // r of ball is 0.4, when distance < 2r, ball collision happens.
		{
			GLfloat axis[3];  //The collision axis for ball bi
			GLfloat axis2[3]; //The collision axis for ball i
			GLfloat va1[3]; //bi velocity along the axis
			GLfloat vpa1[3];//bi velocity perpendicular with the axis
			GLfloat va2[3];//i velocity along the axis
			GLfloat vpa2[3];//i velocity perpendicular with the axis
			for (int j = 0; j < 3; j++) {
				axis[j] = pos[i][j] - pos[bi][j];
				axis2[j] = pos[bi][j] - pos[i][j];
			}
			Normalize(axis);
			Normalize(axis2);
			GLfloat v1axis = DotProduct(axis, initvelocity[bi]);
			GLfloat v2axis = DotProduct(axis2, initvelocity[i]);
			for (int j = 0; j < 3; j++) {
				va1[j] = v1axis * axis[j];
				vpa1[j] = initvelocity[bi][j] - va1[j];
				va2[j] = v2axis * axis2[j];
				vpa2[j] = initvelocity[i][j] - va2[j];
				initvelocity[bi][j] = va2[j] + vpa1[j];
				initvelocity[i][j] = va1[j] + vpa2[j];
			}
			continue;
		}
	}
}


void BallMove(int bi) {
	Groundcol(bi);
	Collision(bi);
	for (int i = 0; i < 3; i++) {//Each frame store the current position and velocity.
		velocity[bi][i] = initvelocity[bi][i] + g[i] * dt;//Next velocity
		initvelocity[bi][i] = velocity[bi][i];  //Next velocity become current velocity
		pos_n[bi][i] = pos[bi][i] + velocity[bi][i] * dt;//Next position
		pos[bi][i] = pos_n[bi][i]; //Next position become current position
		ballro[bi][i + 12] = pos_n[bi][i]; //Store current position to the matrix
	}
}
void Animation() {
	for (int i = 0; i < ball; i++) {
		glPushMatrix();
		BallMove(i);
		for (int j = 0; j < 16; j++) {
			ro[j] = ballro[i][j];
		}
		glMultMatrixf(ro);
		glutSolidSphere(0.4, 40, 40); //Set the radius to 0.4.
		glPopMatrix();
	}
}
void Ground() {//Draw the ground with gridlines
	glBegin(GL_LINES);
	for (GLfloat x = -100; x < 100; x += 5.0f)
	{
		glVertex3f(x, 0, -100); glVertex3f(x, 0, 100);
		glVertex3f(-100, 0, x); glVertex3f(100, 0, x);
	}

	glEnd();
}
//================================
// render
//================================
void render(void) {
	// clear buffer
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// render state
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// light source attributes
	GLfloat LightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightDiffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat LightSpecular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightPosition[] = { 5.0f, 5.0f, 5.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	// surface material attributes
	GLfloat material_Ka[] = { 0.81f, 0.06f, 0.11f, 1.0f };
	GLfloat material_Kd[] = { 0.83f, 0.47f, 0.54f, 1.0f };
	GLfloat material_Ks[] = { 0.33f, 0.33f, 0.52f, 1.0f };
	GLfloat material_Ke[] = { 0.1f , 0.0f , 0.1f , 1.0f };
	GLfloat material_Se = 10;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_Se);

	// modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 16.0, 14.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);



	// render objects
	Ground();
	Animation();

	// disable lighting
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	// swap back and front buffers
	glutSwapBuffers();
}

//================================
// keyboard input
//================================
void keyboard(unsigned char key, int x, int y) {
}

//================================
// reshape : update viewport and projection matrix when the window is resized
//================================
void reshape(int w, int h) {
	// screen size
	g_screenWidth = w;
	g_screenHeight = h;

	// viewport
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	// projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
}


//================================
// timer : triggered every 16ms ( about 60 frames per second )
//================================
void timer(int value) {
	// render
	glutPostRedisplay();

	// reset timer
	// 16 ms per frame ( about 60 frames per second )
	glutTimerFunc(16, timer, 0);
}

//================================
// main
//================================
int main(int argc, char** argv) {
	// create opengL window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("lab3");

	// init
	init();

	// set callback functions
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(16, timer, 0);

	// main loop
	glutMainLoop();

	return 0;
}

