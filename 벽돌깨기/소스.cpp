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

// bar ���� ����
int bar_width = 200, bar_height = 30;
int bar_X = 350, bar_Y = 25;
int bar_speed = 20;

// brick ���� ����
int brickWidth = 50, brickHeight = 40;
int percent, chk;

// �浹 ���� �迭
int	collision_count[100];

// ball ������
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

void Modeling_Bar(void) {	// ���� ƨ�� ����� �׸���
	glBegin(GL_QUADS);
	glVertex2f(bar_X, bar_Y);
	glVertex2f(bar_X, bar_Y + bar_height);
	glVertex2f(bar_X + bar_width, bar_Y + bar_height);
	glVertex2f(bar_X + bar_width, bar_Y);
	glEnd();
}

void buff_Timer(int value) {	// ���� �ð�
	bar_width -= 100;
	chk = 0;
}
	
void draw_bricks(void) {	// ���� �׸���
	srand(time(NULL));
	int determination = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 16; x++) {
			if (collision_count[determination] == 0) {	// �浹 ������ ���� ��� �׸���
				glBegin(GL_POLYGON);
				percent = rand() % 100;
				if (percent > 5) {
					chk = 0;
					glColor3f(0.65, 0, 0);
				}
				else {
					chk = 1;
					glColor3f(1, 1, 1);
				}
				glVertex2d(x * brickWidth, height - (y * brickHeight));
				glVertex2d(x * brickWidth, height - ((y + 1) * brickHeight));
				glVertex2d((x + 1) * brickWidth, height - ((y + 1) * brickHeight));
				glVertex2d((x + 1) * brickWidth, height - (y * brickHeight));
				glEnd();

				glLineWidth(5.0);
				glBegin(GL_LINE_LOOP);
				glColor3f(0.0, 1.0, 0.0);
				glVertex2d(x * brickWidth, height - (y * brickHeight));
				glVertex2d(x * brickWidth, height - ((y + 1) * brickHeight));
				glVertex2d((x + 1) * brickWidth, height - ((y + 1) * brickHeight));
				glVertex2d((x + 1) * brickWidth, height - (y * brickHeight));
				glEnd();
			}

			determination++;
		}
	}
}
void Collision_Detection_to_Walls(void) {	//���� �� �浹 �Լ�
	if (bottom + height < (moving_ball.y + moving_ball_radius)) {	//��� �浹
		//printf("�� ����浹\n");
		velocity.y *= -1;
	}
	else if (left > (moving_ball.x + moving_ball_radius)) {	//���� �浹
		//printf("�� �����浹\n");
		velocity.x *= -1;
	}

	else if (left + width < (moving_ball.x + moving_ball_radius)) {	//������ �浹
		//printf("�� �������浹\n");
		velocity.x *= -1;
	}

	else if (bottom > (moving_ball.y + moving_ball_radius)) { //�ϴ� �浹 : �ʱⰪ���� ����
		//printf("�� �ϴ��浹\n");
		moving_ball.x = width / 2;
		moving_ball.y = height / 4;
		velocity.x = 0;
		velocity.y = 0;
	}
}

void Collision_Detection_to_Bar(void) {		//���� ����� �浹 �Լ�
	if (moving_ball.x >= bar_X && moving_ball.x <= bar_X + bar_width) {
		if ((bar_Y + bar_height) > (moving_ball.y - moving_ball_radius)) {	//��� �浹
			//printf("�� ����浹\n");
			if (moving_ball.x > bar_X + (bar_width / 2) && velocity.x < 0) {
				//	printf("�� ������ ����浹\n");
				velocity.x *= -1;
			}
			velocity.y *= -1;
		}
	}

	if (moving_ball.y > bar_Y && moving_ball.y <= bar_Y + bar_height) {
		if ((bar_X + bar_width) > (moving_ball.x + moving_ball_radius)) {	// ���� �浹
		//	printf("�� �������浹\n");
			velocity.x *= -1;
		}
		if (bar_X > (moving_ball.x + moving_ball_radius)) {	// ���� �浹 
		//	printf("�� �����浹\n");
			velocity.x *= -1;
		}
	}
}

void Collision_Detection_to_Bricks(void) {	//���� ���� �浹 �Լ�
	int determination = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 16; x++) {
			if (collision_count[determination] == 1) {	// �浹 ������ ���� ��� ������ �������� �ʴ´�.
				determination++;
				continue;
			}
			if (moving_ball.x > (brickWidth * x) && moving_ball.x < (brickWidth * (x + 1))) {
				/*if ((moving_ball.y - moving_ball_radius) > height - (brickHeight * y)) {	//��
					printf("���� �����浹\n");
					velocity.y *= -1;
					collision_count[determination] = 1;
				}*/
				if ((moving_ball.y + moving_ball_radius) > height - (brickHeight * (y + 1))) { //	�Ʒ�
					printf("���� �Ʒ����浹\n");
					velocity.y *= -1;
					collision_count[determination] = 1;
					if (chk == 1 && bar_width == 200) {
						printf("�Ͼ� ���� �浹\n");
						glutTimerFunc(5000, buff_Timer, 1);
						bar_width += 100;
					}
				}
			}
			determination++;
		}
	}
}

void RenderScene(void) {
	glClearColor(0.0, 1.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// �浹 ó�� �κ�
	Collision_Detection_to_Walls();		// ���� �� �浹 �Լ�
	Collision_Detection_to_Bar();		// ���� ����� �浹 �Լ�
	Collision_Detection_to_Bricks();

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

/*
if( Ư�� ������ ���� or �ʿ� �����Ǵ� �������� �Ծ��� ���)
	1. �����
	- �� �߾ӿ��� x�� �������θ� �̵��ϴ� ��ֹ� �� ����.
	- ���� ���� 1/2�� ���� : ���� �� �� �浹 ������ ���� ��� ������ ����.
	2. ����
	- �ϴܹ��� ũ�Ⱑ �þ or �������� ���� �� ���� �����.
	- ���� ���� 2�� ���� : �浹 ���� �� ������ �翷 ������ ���� ����
*/