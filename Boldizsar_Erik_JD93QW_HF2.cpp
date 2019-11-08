#include <GL/glut.h>
#include <bevgrafmath2017.h>
#include <math.h>
#include <iostream>
using namespace std;

GLint winWidth = 800 , winHeight = 600; // ablakméret
vec2 hermitepoints[4] = { {100, 100}, { 150, 200 }, { 160, 300 }, { 250, 200 } };
vec2 bezierpoints[4] = { { hermitepoints[2] },{0,0}, {360,310},  { 400, 450} };
vec2 bezierpoints2[5] = { {bezierpoints[3]},{0,0}, {600,580}, { 780, 500},{760,310}};
GLint h = size(hermitepoints);
GLint b1 = size(bezierpoints);
GLint b2 = size(bezierpoints2);
GLint dragged = -1;
mat24 G;
mat4 M;
vec2 Q;
bool showControlPolygon = false;
bool showDeCasteljau = false;
GLint select;
vec2 Points[5];
vec2 deCasteljauPoints[10];
GLfloat deCastParam = (1.0f / 3.0f);
GLint valaszt = 1;
vec2 windowPosition = { -1,-1 };
vec2 windowSize = { 3,3};
vec2 viewportPosition = { winWidth/2.0f, 0 };
vec2 viewportSize = { winWidth/2.0f, winHeight/2.0f };
mat3 W2V = windowToViewport2(windowPosition, windowSize, viewportPosition, viewportSize);

void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0); // Feketére állítom a hátteret.
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, winWidth, 0.0, winHeight);
	glEnable(GL_POINT_SMOOTH); // kört rajzol pontnál négyzet helyett
	glEnable(GL_LINE); // vonal, nem pedig szaggatott.
	glPointSize(10.0); // pontméret
	glLineWidth(2.0); // vonal vastagság
	glShadeModel(GL_FLAT);
}

vec4 T[4];
vec4 t = { {0.0f}, {0.5f}, {1.0f}, {0.0f} };
bool erinto[4] = { false, false, false, true };
void calculateT() // T-kiszámítása attól függően, hogy érintőről vagy pedig pontról van szó.
{
	GLint i;
	for (i = 0; i < 4; i++)
	{
		if (!erinto[i])
		{
			T[i] = { t[i] * t[i] * t[i], t[i] * t[i], t[i], 1 };
		}
		else
		{
			T[i] = { (3 * (t[i] * t[i] * t[i])), 2 * t[i], 1, 0 };
		}
	}
}
vec4 calculateNewT(float i)
{
	vec4 a;
	for (GLint j = 0; j < 4; j++)
	{
		if (i == t[j])
		{
			return T[j];
		}
	}
	a = { i * i * i, i * i, i, 1 };
	return a;
}
void calculateM()
{ // M mátrix kiszámítása
	GLint i, j;
	for (i = 0; i < 4; i++) 
	{
		for (j = 0; j < 4; j++) 
		{
			M[i][j] = T[j][i];
		}
	}
	M = inverse(M);
}

GLint factorial(GLint n) // Faktoriális
{
	return n > 1 ? n * factorial(n - 1) : 1;
}

GLint binomial(GLint n, GLint i) // Binomiális
{
	return factorial(n) / (factorial(i) * factorial(n - i));
}

GLfloat calculateBt(GLint n, GLint i, GLfloat t) // A sulyfüggvényhez számítás
{
	return  (GLfloat)factorial(n) / (factorial(i) * factorial(n - i))* (GLfloat)pow(t, i) * (GLfloat)pow(1 - t, n - i);
}

void hermit_rajz()
{
	GLint choose = -1;

	vec4 A;
	for (GLint k = 0; k < 4; k++)
	{
		choose = k;
		switch (choose)
		{
		case 0:
			glColor3f(1.0f, 0.0f, 1.0f); //lila
			break;
		case 1:
			glColor3f(0.8f, 0.0f, 0.0f); //piros
			break;
		case 2:
			glColor3f(0.0f, 0.8f, 0.0f); //zöld
			break;
		case 3:
			glColor3f(1.0f, 1.0f, 0.0f); //sárga
			break;
		case 4:
			glColor3f(0.0f, 0.0f, 1.0f); //kék
			break;
		}
		glBegin(GL_LINE_STRIP);
		for (GLfloat x = 0; x <= 1; x += 0.001f)
		{
			A = M * calculateNewT(x);
			vec3 pont = W2V * ihToH(vec2{x,A[k]});
			vec4 pontt = hToIh(pont);
			glVertex2f(pontt.x, pontt.y);
		}
		glEnd();
	}
}

void bezier_rajz()
{
	GLint choose = -1;

	for (GLint k = 0; k < 4; k++)
	{
		choose = k;
		switch (choose)
		{
		case 0:
			glColor3f(1.0f, 0.0f, 1.0f); //lila
			break;
		case 1:
			glColor3f(0.8f, 0.0f, 0.0f); //piros
			break;
		case 2:
			glColor3f(0.0f, 0.8f, 0.0f); //zöld
			break;
		case 3:
			glColor3f(1.0f, 1.0f, 0.0f); //sárga
			break;
		case 4:
			glColor3f(0.0f, 0.0f, 1.0f); //kék
			break;
		}
		glBegin(GL_LINE_STRIP);
		for (GLfloat x = 0; x <= 1; x += 0.001f)
		{
			vec3 pont = W2V * ihToH(vec2{ x, calculateBt(3, k, x)});
			vec4 pontt = hToIh(pont);
			glVertex2f(pontt.x,pontt.y);
		}
		glEnd();
	}
}

void bezier2_rajz()
{
	GLint choose = -1;

	for (GLint k = 0; k < 5; k++)
	{
		choose = k;
		switch (choose)
		{
		case 0:
			glColor3f(1.0f, 0.0f, 1.0f); //lila
			break;
		case 1:
			glColor3f(0.8f, 0.0f, 0.0f); //piros
			break;
		case 2:
			glColor3f(0.0f, 0.8f, 0.0f); //zöld
			break;
		case 3:
			glColor3f(1.0f, 1.0f, 0.0f); //sárga
			break;
		case 4:
			glColor3f(0.0f, 0.0f, 1.0f); //kék
			break;
		}
		glBegin(GL_LINE_STRIP);
		for (GLfloat x = 0; x <= 1; x += 0.001f)
		{ // Mindig a k.adik ponthoz tartozó súlyfüggvény
			vec3 pont = W2V * ihToH(vec2{ x, calculateBt(4, k, x) });
			vec4 pontt = hToIh(pont);
			glVertex2f(pontt.x, pontt.y);
		}
		glEnd();
	}
}

void hermite() // Hermite görbe
{
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
	for (GLfloat t = 0; t <= 1; t += 0.001f)
	{
		Q = G * M * calculateNewT(t);
		glVertex2f(Q.x, Q.y);
	}
	glEnd();

	// Érintő vektorok kirajzolása:
	GLfloat td = 1;
	vec2 erintopont = { 0,0 };
	vec4 Tderivalt = { 3 * (td * td), 2 * td, 1, 0 };
	erintopont = G * M * Tderivalt; // A Hermite ív 2.érintővektorának végpontja
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	// 1.:
	glVertex2f(hermitepoints[0].x, hermitepoints[0].y);
	glVertex2f(hermitepoints[3].x, hermitepoints[3].y);
	// 2.:
	glVertex2f(hermitepoints[2].x, hermitepoints[2].y);
	glVertex2f(hermitepoints[2].x+erintopont.x, hermitepoints[2].y+erintopont.y);

	glEnd();

	glBegin(GL_POINTS); // Hermite ív pontok kirazjolása
	glColor3f(0.0f, 0.0f, 1.0f);
	for (GLint i = 0; i < 4; i++)
		glVertex2f(hermitepoints[i].x, hermitepoints[i].y);
	glEnd();
}

vec2 bezier(GLint firstPointIndex, GLint n, GLdouble t) // Bernstein polinom, első pont indexe, a pontok száma, és a paraméterek.
{
	vec2 curvePoint = { 0, 0 };
	GLint i;
	for (i = 0; i <= n; i++)
	{
		curvePoint += binomial(n, i) * pow(t, i) * pow(1 - t, n - i) * bezierpoints[firstPointIndex + i];
	}
	return curvePoint;
}

vec2 deCasteljau(GLint n, GLdouble t)
{
	glColor3f(0.0f, 0.0f, 1.0f);
	GLint i, r;
	for (i = 0; i < n; i++)
	{
		// Külső rész:
		Points[i] = *(bezierpoints2 + i);
	}
	GLint countedPIndex = 0;
	for (r = 1; r < n; r++)
	{
		for (i = 0; i < n - r; i++)
		{
			vec2 A = Points[i];
			vec2 B = Points[i + 1];
			Points[i] = (1 - t) * A + t * B;
			if (showDeCasteljau)
			{
				deCasteljauPoints[countedPIndex] = Points[i];
				countedPIndex++;
			}
		}
	}
	return Points[0];
}

void calcG()
{ // G-mátrix számítása:
	G = { { hermitepoints[0].x, hermitepoints[0].y }, { hermitepoints[1].x,hermitepoints[1].y }, { hermitepoints[2].x,hermitepoints[2].y }, { hermitepoints[3].x - hermitepoints[0].x, hermitepoints[3].y - hermitepoints[0].y } };
}
vec2 updateC1(GLfloat t, GLint n) // A t-amilyen paraméter szerint kell deriválni az érintőt, n pedig, hogy hanyadfokú lesz a beziergörbe.
{
	n = n - 1;
	return bezierpoints[1] = (G * M * vec4{ 3 * (t * t), 2 * t, 1, 0 }) / n + bezierpoints[0];
}
vec2 updateC11(GLint n,GLint m) // n,m - fokszámok
{
	n = n - 1;
	m = m - 1;
	return bezierpoints2[1] = ((n*bezierpoints[n]) + (m* bezierpoints[n]) - ( n* bezierpoints[n-1])) / m;
}
void update(int n)
{
	calcG();
	calculateT();
	calculateM();
	updateC1(1,h);
	updateC11(b1,b2);
	glutPostRedisplay();
	glutTimerFunc(5, update, 0);
}

void keyboard(unsigned char key, int x, int y)
{ 
	switch (key) 
	{
	case 27:
		exit(0);
		break;
	case 'a':
		showControlPolygon = !showControlPolygon;
		break;
	case 'd':
		showDeCasteljau = !showDeCasteljau;
		break;
	case 'w':
		if (deCastParam < 1.0)
		{
			deCastParam += 0.01;
		}
		break;
	case 's':
		if (deCastParam > 0.0)
		{
			deCastParam -= 0.01;
		}
		break;
	}
}

GLint getActivePoint1(vec2 p[], GLint size, GLint sens, GLint x, GLint y)
{ // A p-be mentem el a kurzor pozicióját. 
	GLint i, s = sens * sens;
	vec2 P = { (float)x, (float)y };
// a points tömb, a méret, a sens pedig az epszilon lesz, ami jelen
// esetben 8, az x és az y a kurzor poziciója.
	for (i = 0; i < size; i++)
		if (dist2(p[i], P) < s) // A dist2 négyzetes távolságot számol. A P-ben a pontok koordinátái vannak elmentve.
			return i; // Ugyanugy ha megfogtuk, akkor mentjük az indexet, ha pedig nem, akkor -1 lesz az érték.
	return -1;
}

void processMouse(GLint button, GLint action, GLint xMouse, GLint yMouse)
{ // Nézzük, hogy az adott billentyü milyen állapotban van, le van nyomva vagy pedig fel van engedve
	GLint i;
	if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN)
	{
		if ((i = getActivePoint1(hermitepoints, 4, 8, xMouse, winHeight - yMouse)) != -1) // Nézzük, hogy a lenyomott pozicio, egy szakasz végponton van-e
		{
			dragged = i; // Igaz vagy hamis értékkel tér vissza, ha az egér bal gommbját leütöttük, vagy pedig nem.
			select = 1;
		}
		else if ((i = getActivePoint1(bezierpoints, 4, 8, xMouse, winHeight - yMouse)) != -1)
		{
			dragged = i;
			select = 2;
		}
		else if ((i = getActivePoint1(bezierpoints2, 5, 8, xMouse, winHeight - yMouse)) != -1)
		{
			dragged = i;
			select = 3;
		}
	}
	if (button == GLUT_LEFT_BUTTON && action == GLUT_UP)
		dragged = -1;// -1 ha felengedtük a gombot, ezt azt jelöli hogy éppen nincs megfogva 1 Vertex/vonal sem.
	// Az xMouse és az yMouse a kurzor poziciója, ha éppen egy vonalon van a kurzor, akkor elmentjük a dragged változóba a pont indexét.

	if (button == GLUT_RIGHT_BUTTON && action == GLUT_DOWN)
	{
		if ( (xMouse < 400) && (winHeight - yMouse < 300) )
		{
			valaszt = 1;
		}
		else if ((xMouse < 400) && (winHeight - yMouse < 600) && (winHeight - yMouse > 300))
		{
			valaszt = 2;
		}
		else if ((xMouse < 800) && (xMouse > 400) && (winHeight - yMouse < 600) && (winHeight - yMouse > 300))
		{
			valaszt = 3;
		}
	}
} 

void processMouseActiveMotion(GLint xMouse, GLint yMouse)
{ // Itt az egér mozgását figyeljük, hogy milyen interakció jöjjön létre. 
	if (dragged >= 0)
	{
		//  A pontok mozgatása megfelelően, tehát ha az első görbe 2 indexü pontját fogom meg, akkor
		//mozgassa a 2.görbe első pontját is, mivel ezek megegyeznek.
		switch (select)
		{
		case 1:
			hermitepoints[dragged].x = xMouse; // A kurzor aktuális pozicióját adjuk meg a pont uj koordinátájának, addig amíg a bal gomb le van nyomva.
			hermitepoints[dragged].y = winHeight - yMouse;
			if (dragged == 2) bezierpoints[0] = hermitepoints[dragged];
			break;
		case 2:
			bezierpoints[dragged].x = xMouse; 
			bezierpoints[dragged].y = winHeight - yMouse;
			if (dragged == 0) hermitepoints[2] = bezierpoints[dragged];
			if (dragged == b1 - 1) bezierpoints2[0] = bezierpoints[dragged];
			break;
		case 3:
			bezierpoints2[dragged].x = xMouse; 
			bezierpoints2[dragged].y = winHeight - yMouse;
			if (dragged == 0) bezierpoints[b1 - 1] = bezierpoints2[dragged];
			break;
		}
	
		calcG();
		updateC1(1,h);
		updateC11(b1,b2);
		glutPostRedisplay();
	}
}

void valasztas(GLint valaszt)
{
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	// 4. Sulyfüggvényes ablakban a koordinátarendszer x és y tengelye:
	vec2 X = hToIh(W2V * ihToH( vec2{ -0.3, 0 } ) );
	vec2 Y = hToIh(W2V * ihToH( vec2{ 0, -0.4 } ) );
	vec2 Xmax = hToIh(W2V * ihToH( vec2{ 1.1, 0 } ) );
	vec2 Ymax = hToIh(W2V * ihToH( vec2{ 0, 1.25 } ) );
	glVertex2f(X.x, X.y);
	glVertex2f(Xmax.x, Xmax.y);
	glVertex2f(Y.x, Y.y);
	glVertex2f(Ymax.x, Ymax.y);
	glEnd();

	vec2 rpoints1[2] = { { 0, 0 },{ 400, 300 } };
	vec2 rpoints2[2] = { { 0, 300 },{ 400, 600 } };
	vec2 rpoints3[2] = { { 400, 300 },{ 800, 600 } };

	switch (valaszt)
	{
	case 1:
		glColor3f(1.0f, 0.9f, 0.9f);
		glRecti(rpoints1[0].x, rpoints1[0].y, rpoints1[1].x, rpoints1[1].y);
		hermit_rajz();
		break;
	case 2:
		glColor3f(1.0f, 0.9f, 0.9f);
		glRecti(rpoints2[0].x, rpoints2[0].y, rpoints2[1].x, rpoints2[1].y);
		bezier_rajz();
		break;
	case 3:
		glColor3f(1.0f, 0.9f, 0.9f);
		glRecti(rpoints3[0].x, rpoints3[0].y, rpoints3[1].x, rpoints3[1].y);
		bezier2_rajz();
		break;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Három db négyzet kirajzolása:
	valasztas(valaszt);

	glColor3f(1.0f, 0.0f, 0.0f);
	hermite(); // Hermite ív kirajzolása pontokkal együtt

	glColor3f(1.0f, 0.5f, 0.0f);
	glBegin(GL_LINE_STRIP); // 1. Bézier görbe:
	for (GLfloat t = 0; t <= 1; t += 0.01f) 
	{
		glVertex2f(bezier(0, (b1-1), t).x, bezier(0, (b1-1), t).y);
	}
	glVertex2f(bezier(0, (b1-1), 1).x, bezier(0, (b1-1), 1).y);
	glEnd();

	glBegin(GL_POINTS); // 1.Bezier görbe pontjai:
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(bezierpoints[1].x, bezierpoints[1].y);
	glColor3f(0.0f, 0.0f, 1.0f);
	for (int i = 2; i < b1; i++)
		glVertex2f(bezierpoints[i].x, bezierpoints[i].y);
	glEnd();

	GLdouble t;
	glColor3f(0.0f, 0.0f, 1.0f); // 2.Bezier görbe:
	glBegin(GL_LINE_STRIP);
	vec2 bezier2Point;
	for (t = 0; t <= 1; t += 0.01f)
	{
		bezier2Point = deCasteljau(b2, t);
		glVertex2f(bezier2Point.x, bezier2Point.y);
	}
	bezier2Point = deCasteljau(b2, 1);
	glVertex2f(bezier2Point.x, bezier2Point.y);
	glEnd();

	glBegin(GL_POINTS); // Itt a 2.Bezier görbe pontjai:
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(bezierpoints2[1].x, bezierpoints2[1].y);
	glColor3f(0.0f, 0.0f, 1.0f);
	for (int i = 2; i < b2; i++)
		glVertex2f(bezierpoints2[i].x, bezierpoints2[i].y);
	glEnd();

	if (showDeCasteljau)
	{ // A 2.Bézier görbe, de Casteljau vonalak:
		vec2 curvePoint = deCasteljau(b2, deCastParam);
		vec2 A = bezierpoints2[0];
		vec2 B = bezierpoints2[b2-1];
		GLint i;
		glBegin(GL_LINES);
		for (i = 0; i < b2-1; i++)
		{
			glVertex2f(bezierpoints2[i].x, bezierpoints2[i].y);
			glVertex2f(bezierpoints2[i + 1].x, bezierpoints2[i + 1].y);
		}
		GLint tmpMax = 3, tmp = 0;
		for (i = 0; i < 2*(b2-1); ++i)
		{
			glVertex2f(deCasteljauPoints[i].x, deCasteljauPoints[i].y);
			glVertex2f(deCasteljauPoints[i + 1].x, deCasteljauPoints[i + 1].y);
			
			++tmp;
			if (tmp == tmpMax)
			{
				tmpMax--;
				tmp = 0;
				++i;
			}
		}
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
		glVertex2f(curvePoint.x, curvePoint.y);
		glEnd();
	}

	if (showControlPolygon)
	{
		//Kontrollpoligon kirajzolása
		glLineWidth(3.0);
		glColor3f(0.0f, 0.0f, 0.0f);
		GLint i;
		glBegin(GL_LINE_STRIP);
		for (i = 0; i < b1; i++)
		{
			glVertex2f(bezierpoints[i].x, bezierpoints[i].y);
		}
		for (i = 0; i < b2; i++)
		{
			glVertex2f(bezierpoints2[i].x, bezierpoints2[i].y);
		}
		glEnd();
		glLineWidth(2.0);
	}
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Boldizsar_Erik_JD93QW_2.HF");
	init();
	glutDisplayFunc(display);
	glutMouseFunc(processMouse); // le van nyomva vagy nem az egérgomb.
	glutKeyboardFunc(keyboard); // billentyüzet.
	glutMotionFunc(processMouseActiveMotion); // milyen mozgás van, milyen történés legyen.
	glutTimerFunc(5, update, 0); // Update függvény
	glutMainLoop();
	return 0;
}