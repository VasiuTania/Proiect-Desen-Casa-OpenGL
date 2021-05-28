#include "GLOS.h"

#include<GL/GL.H>
#include<GL/GLU.H>
#include<GL/GLAux.h>

#include <math.h>
#include <stdio.h>
#include <stdbool.h>

const int x = 0, y = 1, z = 2, w = 3; // used only for indexing
const int a = 0, b = 1, c = 2, d = 3; // used only for indexing

static float lightSourcePosition[] = { -60, 70, -20, 1 };
static float modelX = 0;
static float modelAngle = 0;

void CALLBACK mutaSursaDreapta(AUX_EVENTREC* event) {
	if (lightSourcePosition[0] < 100)
		lightSourcePosition[0] += 5;
}
void CALLBACK mutaSursaStanga(AUX_EVENTREC* event) {
	if (lightSourcePosition[0] > -100)
		lightSourcePosition[0] -= 5;
}

void CALLBACK rotireDreapta() {
	modelAngle -= 2;
}
void CALLBACK rotireStanga() {
	modelAngle += 2;
}

void CALLBACK mutaStanga()
{
	modelX = modelX - 2;
}
void CALLBACK mutaDreapta()
{
	modelX = modelX + 2;
}

void computePlaneCoefficientsFromPoints(float points[3][3], float coefficients[4]) {
	// calculeaza 2 vectori din 3 pct
	float v1[3], v2[3];
	v1[x] = points[0][x] - points[1][x];
	v1[y] = points[0][y] - points[1][y];
	v1[z] = points[0][z] - points[1][z];
	v2[x] = points[1][x] - points[2][x];
	v2[y] = points[1][y] - points[2][y];
	v2[z] = points[1][z] - points[2][z];

	// produsul vectorial al celor 2 vectori => al 3lea vector cu componentele A,B,C chiar coef din ec. planului
	coefficients[a] = v1[y] * v2[z] - v1[z] * v2[y];
	coefficients[b] = v1[z] * v2[x] - v1[x] * v2[z];
	coefficients[c] = v1[x] * v2[y] - v1[y] - v2[x];

	// determinam D - ecuatia planului in punctul random ales trebuie sa fie zero
	int randomPoint = 1;
	coefficients[d] = -(coefficients[a] * points[randomPoint][x] + coefficients[b] * points[randomPoint][y] + coefficients[c] * points[randomPoint][z]);
}

void computeShadowMatrix(float points[3][3], float lightSourcePosition[4], float shadowMatrix[4][4]) {
	// determina coef planului
	float coefficients[4];
	computePlaneCoefficientsFromPoints(points, coefficients);

	// temp = AxL + ByL + CzL + Dw
	float temp =
		coefficients[a] * lightSourcePosition[x] +
		coefficients[b] * lightSourcePosition[y] +
		coefficients[c] * lightSourcePosition[z] +
		coefficients[d] * lightSourcePosition[w];

	//prima coloana
	shadowMatrix[0][0] = temp - coefficients[a] * lightSourcePosition[x];
	shadowMatrix[1][0] = 0.0f - coefficients[b] * lightSourcePosition[x];
	shadowMatrix[2][0] = 0.0f - coefficients[c] * lightSourcePosition[x];
	shadowMatrix[3][0] = 0.0f - coefficients[d] * lightSourcePosition[x];
	//a 2a coloana
	shadowMatrix[0][1] = 0.0f - coefficients[a] * lightSourcePosition[y];
	shadowMatrix[1][1] = temp - coefficients[b] * lightSourcePosition[y];
	shadowMatrix[2][1] = 0.0f - coefficients[c] * lightSourcePosition[y];
	shadowMatrix[3][1] = 0.0f - coefficients[d] * lightSourcePosition[y];
	//a 3a coloana
	shadowMatrix[0][2] = 0.0f - coefficients[a] * lightSourcePosition[z];
	shadowMatrix[1][2] = 0.0f - coefficients[b] * lightSourcePosition[z];
	shadowMatrix[2][2] = temp - coefficients[c] * lightSourcePosition[z];
	shadowMatrix[3][2] = 0.0f - coefficients[d] * lightSourcePosition[z];
	//a 4a coloana
	shadowMatrix[0][3] = 0.0f - coefficients[a] * lightSourcePosition[w];
	shadowMatrix[1][3] = 0.0f - coefficients[b] * lightSourcePosition[w];
	shadowMatrix[2][3] = 0.0f - coefficients[c] * lightSourcePosition[w];
	shadowMatrix[3][3] = temp - coefficients[d] * lightSourcePosition[w];

	return shadowMatrix;
}

void myInit(void) {
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// sursa lumina
	float lightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightSourcePosition);

	// material
	float materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float materialShininess[] = { 128.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	// use current color set with glColor3f() to directly set the material ambient and diffuse
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

void deseneazaPereti() {
	{

		//------------perete fata-----------
		{
			glPointSize(5);

			glBegin(GL_POLYGON);
			glVertex3f(-20, 20, 20);
			glVertex3f(20, 20, 20);
			glVertex3f(20, -20, 20);
			glVertex3f(-20, -20, 20);
			glEnd();
		}
		//------------perete spate-----------
		{

			glBegin(GL_POLYGON);
			glVertex3f(-20, 20, -20);
			glVertex3f(20, 20, -20);
			glVertex3f(20, -20, -20);
			glVertex3f(-20, -20, -20);
			glEnd();
		}

		//------------perete dreapta-----------
		{

			glBegin(GL_POLYGON);
			glVertex3f(-20, 20, 20);
			glVertex3f(-20, 20, -20);
			glVertex3f(-20, -20, -20);
			glVertex3f(-20, -20, 20);

			glEnd();
		}



		//------------perete stanga-----------
		{

			glBegin(GL_POLYGON);
			glVertex3f(20, 20, 20);
			glVertex3f(20, 20, -20);
			glVertex3f(20, -20, -20);
			glVertex3f(20, -20, 20);

			glEnd();
		}
	}
}


void deseneazaAcoperis() {

	//-----------------Acoperis-------------

	{
		//----------------fata-----


		glBegin(GL_TRIANGLES);
		glVertex3f(-20, 20, 20);
		glVertex3f(0, 48, 20);
		glVertex3f(20, 20, 20);
		glEnd();


		//----------------spate-----



		glBegin(GL_TRIANGLES);
		glVertex3f(-20, 20, -20);
		glVertex3f(0, 48, -20);
		glVertex3f(20, 20, -20);
		glEnd();


		//------------stanga-----------
		{

			glBegin(GL_POLYGON);
			glVertex3f(-20, 20, 20);
			glVertex3f(-20, 20, -20);
			glVertex3f(0, 48, -20);
			glVertex3f(0, 48, 20);

			glEnd();
		}

		//------------stanga-----------
		{

			glBegin(GL_POLYGON);
			glVertex3f(20, 20, 20);
			glVertex3f(20, 20, -20);
			glVertex3f(0, 48, -20);
			glVertex3f(0, 48, 20);

			glEnd();
		}

	}
}

void deseneazaGeamuri() {
	//--------geamuri----


	{
		//stang

		glBegin(GL_POLYGON);
		glVertex3f(-5, 10, -20);
		glVertex3f(-5, 0, -20);
		glVertex3f(-15, 0, -20);
		glVertex3f(-15, 10, -20);
		glEnd();

		//drept
		glBegin(GL_POLYGON);
		glVertex3f(5, 10, -20);
		glVertex3f(5, 0, -20);
		glVertex3f(15, 0, -20);
		glVertex3f(15, 10, -20);
		glEnd();

		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(-10, 10, -20);
		glVertex3f(-10, 0, -20);

		glEnd();
		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(-15, 5, -20);
		glVertex3f(-15, 5, -20);


		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(10, 10, -20);
		glVertex3f(10, 0, -20);

		glEnd();
		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(15, 5, -20);
		glVertex3f(5, 5, -20);

		glEnd();






	}

}


void desenareModel(bool umbra) {
	if (umbra) {
		glColor3f(0.05, 0.15, 0.05);

	}
	else {
		glColor3f(0, 0.5, 0.5);
		//glColor3f(0.5, 1.0, 0.5);
		
	}
	glPushMatrix();
	glTranslatef(modelX, -30, 0);
	glRotatef(modelAngle, 1, 1, 0);

	deseneazaPereti();

	if (umbra) {
		glColor3f(0.05, 0.15, 0.05);

	}
	else {
		glColor3f(1.0, 0, 0);
	}

	deseneazaAcoperis();

	if (umbra) {
		glColor3f(0, 0.3, 0);

	}
	else {
		glColor3f(1, 1, 0.9);
	}

	

	deseneazaGeamuri();

	glPopMatrix();
}

void desenareIarba() {
	glPushMatrix();
	glColor3f(0, 0.3, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-150, -60, -150);
	glVertex3f(-150, -60, 150);
	glVertex3f(150, -60, 150);
	glVertex3f(150, -60, -150);
	glEnd();
	glPopMatrix();
}

void deseneazaLumina()
{
	glPushMatrix();
	glTranslatef(lightSourcePosition[x], lightSourcePosition[y], lightSourcePosition[z]);
	glColor3f(1.0, 0.9, 0);
	auxWireSphere(5);
	glPopMatrix();
}

void CALLBACK display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, 0, -150);
	glRotatef(15, 1, 0, 0);

	// pick any 3 points of the plane
	GLfloat puncte[3][3] = {
		{ -150.0f, -60.0f, -150.0f },
		{ -150.0f, -60.0f, 150.0f },
		{ 150.0f, -60.0f, 150.0f } };

	static float shadowMatrix[4][4];
	computeShadowMatrix(puncte, lightSourcePosition, shadowMatrix);

	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, lightSourcePosition);
	desenareIarba();
	desenareModel(false);

	glPopMatrix();

	//deseneaza umbra
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glMultMatrixf((GLfloat*)shadowMatrix); // se inmulteste matricea curenta cu matricea de umbrire
	desenareModel(true);
	glPopMatrix();
	deseneazaLumina();

	glEnable(GL_LIGHTING);

	auxSwapBuffers();
}


void CALLBACK myReshape(GLsizei w, GLsizei h) {
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, 1.0 * (GLfloat)w / (GLfloat)h, 0.1, 300.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
	auxInitDisplayMode(AUX_DOUBLE | AUX_DEPTH16 | AUX_RGB);
	auxInitPosition(10, 10, 1200, 800);
	auxKeyFunc(AUX_RIGHT, mutaDreapta);
	auxKeyFunc(AUX_LEFT, mutaStanga);
	auxKeyFunc(AUX_UP, rotireDreapta);
	auxKeyFunc(AUX_DOWN, rotireStanga);
	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, mutaSursaDreapta);
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEUP, mutaSursaStanga);
	auxInitWindowA("Umbra");
	myInit();
	auxReshapeFunc(myReshape);
	auxMainLoop(display);
	return 0;
}