#pragma warning(disable:4996)
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define gameWidth		1000
#define gameHeight		600

#define	width 			800
#define	height			600
#define PI				3.1415
#define	polygon_num		50

int	left = 0, bottom = 0;

// bar 설정 변수
int bar_width = 200, bar_height = 30;
int bar_speed = 20;

// brick 설정 변수
int brickWidth = 50, brickHeight = 40;
int percent, chk;

// 충돌 판정 배열
int	collision_count[100];

// ball 반지름
float moving_ball_radius;

// 점수
int score = 0;

typedef struct _Point {
	float	x;
	float	y;
} Point;

Point moving_ball, velocity, bar;

void init(void) {
	moving_ball_radius = 10.0;
	moving_ball.x = width / 2;
	moving_ball.y = height / 4;

	velocity.x = 0;
	velocity.y = 0;

	bar.x = 350;
	bar.y = 25;
}

void drawBitmapText(char* str, float x, float y) {
	int len = strlen(str);
	glColor3f(1, 1, 1);
	glRasterPos2f(x, y);
	for(int i=0;i<len;i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
}

void scoreboard(void) {
	glColor3d(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(width + 3, 0);
	glVertex2f(width + gameWidth, 0);
	glVertex2f(width + gameWidth, height);
	glVertex2f(width, height);
	glEnd();
	
	char str[] = "Score";
	drawBitmapText(str, width + 70, height-50);
	drawBitmapText(itoa(score, str, 10), width + 82, height - 72);
}

void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, left + gameWidth, bottom, bottom + gameHeight); // 왼쪽 오른쪽 아래 위 벽 위치
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
	glVertex2f(bar.x, bar.y);
	glVertex2f(bar.x, bar.y + bar_height);
	glVertex2f(bar.x + bar_width, bar.y + bar_height);
	glVertex2f(bar.x + bar_width, bar.y);
	glEnd();
}

void buff_Timer(int value) {	// 버프 시간
	bar_width -= 100;
	chk = 0;
}

void draw_bricks(void) {	// 벽돌 그리기
	srand(time(NULL));
	int determination = 0;
	for (int y = 0; y < 3; y++) {
		for (int x = 0; x < 16; x++) {
			if (collision_count[determination] == 0) {	// 충돌 판정이 없을 경우 그리기
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
	if (moving_ball.x >= bar.x && moving_ball.x <= bar.x + bar_width) {
		if ((bar.y + bar_height) > (moving_ball.y - moving_ball_radius)) {	//상단 충돌
			//printf("바 상단충돌\n");
			if (moving_ball.x > bar.x + (bar_width / 2) && velocity.x < 0) {
				//	printf("바 오른쪽 상단충돌\n");
				velocity.x *= -1;
			}
			velocity.y *= -1;
		}
	}

	if (moving_ball.y > bar.y  && moving_ball.y <= bar.y + bar_height) {
		if ((bar.x + bar_width) > (moving_ball.x + moving_ball_radius)) {	// 우측 충돌
		//	printf("바 오른쪽충돌\n");
			velocity.x *= -1;
		}
		if (bar.x > (moving_ball.x + moving_ball_radius)) {	// 좌측 충돌 
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
				/*if ((moving_ball.y - moving_ball_radius) > height - (brickHeight * y)) {	//위
					printf("벽돌 위쪽충돌\n");
					velocity.y *= -1;
					collision_count[determination] = 1;
				}*/
				if ((moving_ball.y + moving_ball_radius) > height - (brickHeight * (y + 1))) { //	아래
					score++;
					printf("벽돌 아래쪽충돌\n");
					velocity.y *= -1;
					collision_count[determination] = 1;
					if (chk == 1 && bar_width == 200) {
						printf("하얀 벽돌 충돌\n");
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

	scoreboard();	//스코어보드

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
		if ((width - bar_width) >= (bar.x + bar_speed)) {
			bar.x += bar_speed;
		}
	}

	else if (key == GLUT_KEY_LEFT) {
		if (0 <= (bar.x - bar_speed)) {
			bar.x -= bar_speed;
		}
	}

	else if (key == GLUT_KEY_F1) {
		int random = rand() % 100;
		if (random % 2 == 0) {
			velocity.x = 0.07;
			velocity.y = 0.07;
		}
		else {
			velocity.x = -0.07;
			velocity.y = 0.07;
		}
	}
	glutPostRedisplay();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(gameWidth, gameHeight);
	glutCreateWindow("Break Bricks");
	init();
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	glutSpecialFunc(myKey);
	glutMainLoop();
}