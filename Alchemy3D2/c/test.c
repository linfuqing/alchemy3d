#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 400
#define HEIGHT 400

static GLfloat angle = 0.0f;
const int n = 20;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;


double CalFrequency()
{
	static int count;
	static double save;
	static clock_t last, current;
	double timegap;

	++count;
	if( count <= 50 )
		return save;
	count = 0;
	last = current;
	current = clock();
	timegap = (current-last)/(double)CLK_TCK;
	save = 50.0/timegap;
	return save;
}

void myDisplay(void)
{
	int i;
	double FPS = CalFrequency();

	printf("FPS = %f\n", FPS);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// 创建透视效果视图
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, 1.7f, 10.0f, 2000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -500.0, 0.0, 1.0, 0.0);

	glBegin(GL_TRIANGLES);

	glVertex3f(0.0f, 0.0f, -200.0f);
	glVertex3f(100.0f, 0.0f, -200.0f);
	glVertex3f(100.0f, 100.0f, -200.0f);

	glEnd();
	glFlush();

	glutSwapBuffers();
}

void myIdle(void)
{
	//myDisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL光照演示");
	glutDisplayFunc(&myDisplay);
	//glutIdleFunc(&myIdle);
	glutMainLoop();
	return 0;
}
