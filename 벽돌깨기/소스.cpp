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

// bar 설정 변수
int bar_width = 200, bar_height = 30;
int bar_X = 350, bar_Y = 25;
int bar_speed = 20;

// brick 설정 변수
int brickWidth = 50, brickHeight = 40;

// 충돌 판정 배열
int	collision_count[100];

// ball 반지름
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
	gluOrtho2D(left, left + width, bottom, bottom + height); // 왼쪽 오른쪽 아래 위 벽 위치
}

void Modeling_Circle(float radius, Point CC) {	// 공 그리기
	float	delta;

	delta = 2 * PI / polygon_num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
	glEnd();
}

void Modeling_Bar(void) {	// 공을 튕길 막대기 그리기
	glBegin(GL_QUADS);
	glVertex2f(bar_X, bar_Y);
	glVertex2f(bar_X, bar_Y + bar_height);
	glVertex2f(bar_X + bar_width, bar_Y + bar_height);
	glVertex2f(bar_X + bar_width, bar_Y);
	glEnd();
}

void draw_bricks(void) {	// 벽돌 그리기
	int determination = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 16; x++) {
			if (collision_count[determination] == 0) {	// 충돌 판정이 없을 경우 그리기
				glBegin(GL_POLYGON);
				glColor3f(0.65, 0.0, 0.0);
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
void Collision_Detection_to_Walls(void) {	//공과 벽 충돌 함수
	if (bottom + height < (moving_ball.y + moving_ball_radius)) {	//상단 충돌
		//printf("벽 상단충돌\n");
		velocity.y *= -1;
	}
	else if (left > (moving_ball.x + moving_ball_radius)) {	//왼쪽 충돌
		//printf("벽 왼쪽충돌\n");
		velocity.x *= -1;
	}

	else if (left + width < (moving_ball.x + moving_ball_radius)) {	//오른쪽 충돌
		//printf("벽 오른쪽충돌\n");
		velocity.x *= -1;
	}

	else if (bottom > (moving_ball.y + moving_ball_radius)) { //하단 충돌 : 초기값으로 지정
		//printf("벽 하단충돌\n");
		moving_ball.x = width / 2;
		moving_ball.y = height / 4;
		velocity.x = 0;
		velocity.y = 0;
	}
}

void Collision_Detection_to_Bar(void) {		//공과 막대기 충돌 함수
	if (moving_ball.x >= bar_X && moving_ball.x <= bar_X + bar_width) {
		if ((bar_Y + bar_height) > (moving_ball.y - moving_ball_radius)) {	//상단 충돌
			//printf("바 상단충돌\n");
			if (moving_ball.x > bar_X + (bar_width / 2) && velocity.x < 0) {
			//	printf("바 오른쪽 상단충돌\n");
				velocity.x *= -1;
			}
			velocity.y *= -1;
		}
	}

	if (moving_ball.y > bar_Y && moving_ball.y <= bar_Y + bar_height) {
		if ((bar_X + bar_width) > (moving_ball.x + moving_ball_radius)) {	// 우측 충돌
		//	printf("바 오른쪽충돌\n");
			velocity.x *= -1;
		}
		if (bar_X > (moving_ball.x + moving_ball_radius)) {	// 좌측 충돌 
		//	printf("바 왼쪽충돌\n");
			velocity.x *= -1;
		}
	}
}

void Collision_Detection_to_Bricks(void) {	//공과 벽돌 충돌 함수
	int determination = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 16; x++) {
			if (collision_count[determination] == 1) {	// 충돌 판정이 났을 경우 다음을 실행하지 않는다.
				determination++;
				continue;
			}
			if (moving_ball.x > (brickWidth * x) && moving_ball.x < (brickWidth * (x + 1))) {
				if ((moving_ball.y - moving_ball_radius) > height - (brickHeight * y)) {	//위
					printf("벽돌 위쪽충돌\n");
					velocity.y *= -1;
					collision_count[determination] = 1;
				}
				if ((moving_ball.y + moving_ball_radius) > height - (brickHeight * (y + 1))) { //	아래
					printf("벽돌 아래쪽충돌\n");
					velocity.y *= -1;
					collision_count[determination] = 1;
				}
			}
			determination++;
		}
	}
}

void RenderScene(void) {
	glClearColor(0.0, 1.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// 충돌 처리 부분
	Collision_Detection_to_Walls();		// 공과 벽 충돌 함수
	Collision_Detection_to_Bar();		// 공과 막대기 충돌 함수
	Collision_Detection_to_Bricks();

	// 움직이는 공의 위치 변화 
	moving_ball.x += velocity.x;	// 움직이는 공의 x좌표
	moving_ball.y += velocity.y;	// 움직이는 공의 y좌표

	// 움직이는 공 그리기 
	glColor3f(0.0, 0.0, 1.0);
	Modeling_Circle(moving_ball_radius, moving_ball);

	// 움직이는 막대기 그리기
	glColor3f(1, 0.647059, 0);
	Modeling_Bar();

	//벽돌 그리기
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