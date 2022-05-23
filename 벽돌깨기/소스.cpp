#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#pragma warning(disable:4996)

#define	width 			800
#define	height			600
#define PI				3.1415
#define	polygon_num		50

int	left = 0, bottom = 0;
int bar_width = 200, bar_height = 30;
int bar_X = 350, bar_Y = 25;
int bar_speed = 15;

int brickWidth = 50, brickHeight = 40;

int	collision_count = 0;

float moving_ball_radius;

typedef struct _Point {
	float	x;
	float	y;
} Point;

Point moving_ball, velocity;


void init(void) {
	moving_ball_radius = 10.0;
	moving_ball.x = width / 2;
	moving_ball.y = height / 4;

	velocity.x = 0;
	velocity.y = 0;

	collision_count = 1;
}


void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, left + width, bottom, bottom + height); // ���� ������ �Ʒ� �� �� ��ġ
}

void Modeling_Circle(float radius, Point CC) {	// �� �׸���
	float	delta;

	delta = 2 * PI / polygon_num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
	glEnd();
}

void Modeling_Bar(void) {	// ���� ƨ�� ����� ����
	glBegin(GL_QUADS);
	glVertex2f(bar_X, bar_Y);
	glVertex2f(bar_X, bar_Y + bar_height);
	glVertex2f(bar_X + bar_width, bar_Y + bar_height);
	glVertex2f(bar_X + bar_width, bar_Y);
	glEnd();
}

void Collision_Detection_to_Walls(void) {	//���� �� �浹 �Լ�
	if (bottom + height < (moving_ball.y + moving_ball_radius)) {	//��� �浹
		velocity.y *= -1;
	}
	else if (left > (moving_ball.x + moving_ball_radius)) {	//���� �浹
		velocity.x *= -1;
	}

	else if (left + width < (moving_ball.x + moving_ball_radius)) {	//������ �浹
		velocity.x *= -1;
	}

	else if (bottom > (moving_ball.y + moving_ball_radius)) { //�Ʒ� �浹
		moving_ball.x = width / 2;
		moving_ball.y = height / 4;
		velocity.x = 0;
		velocity.y = 0;
	}
}

void Collision_Detection_to_Bar(void) {		//���� ����� �浹 �Լ�
	if (moving_ball.x >= bar_X && moving_ball.x <= bar_X + bar_width) {
		if ((bar_Y + bar_height) > (moving_ball.y - moving_ball_radius)) {
			velocity.y *= -1;
		}
	}
}

void draw_bricks(void) {
	srand(time(NULL));
	for (int i = 0; i < 3; i+=2) {
		for (int j = 0; j < 16; j+=2) {
			glColor3f((rand() % 10 / 10.0), (rand() % 10 / 10.0), (rand() % 10 / 10.0));

			glBegin(GL_POLYGON);
			glVertex2i(j * brickWidth, height - i * brickHeight);
			glVertex2i(j * brickWidth, height - (i + 1) * brickHeight);
			glVertex2i((j + 1) * brickWidth, height - (i + 1) * brickHeight);
			glVertex2i((j + 1) * brickWidth, height - i * brickHeight);
			glEnd();
		}
	}
}

void RenderScene(void) {
	glClearColor(0.0, 1.0, 0.0, 0.0); // Set display-window color to Yellow
	glClear(GL_COLOR_BUFFER_BIT);

	// �浹 ó�� �κ�
	Collision_Detection_to_Walls();		// ���� �� �浹 �Լ�
	Collision_Detection_to_Bar();		// ���� ����� �浹 �Լ�

	// �����̴� ���� ��ġ ��ȭ 
	moving_ball.x += velocity.x;	// �����̴� ���� x��ǥ
	moving_ball.y += velocity.y;	// �����̴� ���� y��ǥ

	// �����̴� �� �׸��� 
	glColor3f(0.0, 0.0, 1.0);
	Modeling_Circle(moving_ball_radius, moving_ball);

	// �����̴� ����� �׸���
	glColor3f(1, 0.647059, 0);
	Modeling_Bar();

	//���� �׸���
	draw_bricks();

	glutSwapBuffers();
	glFlush();
}

void myKey(int key, int x, int y) {
	if (key == GLUT_KEY_RIGHT) {
		if ((width - bar_width) >= (bar_X + bar_speed)) {
			bar_X += bar_speed;
		}
	}

	else if (key == GLUT_KEY_LEFT) {
		if (0 <= (bar_X - bar_speed)) {
			bar_X -= bar_speed;
		}
	}

	else if (key == GLUT_KEY_F1) {
		velocity.x = 0.07;
		velocity.y = 0.07;
	}
	glutPostRedisplay();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Break Bricks");
	init();
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(myKey);
	glutMainLoop();
}