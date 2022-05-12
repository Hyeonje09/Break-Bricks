#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>

#define	width 			800
#define	height			600
#define	PI				3.1415
#define	polygon_num		50

int	left = 0, bottom = 0;
int bar_width = 200, bar_height = 30;
int bar_X = 350, bar_Y = 25;
int bar_speed = 10;

int	collision_count = 0;

float radius1, moving_ball_radius;

typedef struct _Point {
	float	x;
	float	y;
} Point;

Point moving_ball, velocity;


void init(void) {
	radius1 = 20.0;

	moving_ball_radius = 10.0;
	moving_ball.x = width / 2;
	moving_ball.y = height / 4;

	velocity.x = 0.0;
	velocity.y = 0.1;

	collision_count = 1;
}


void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, left + width, bottom, bottom + height); // 왼쪽 오른쪽 아래 위 벽 위치
}

void Modeling_Circle(float radius, Point CC) {
	float	delta;

	delta = 2 * PI / polygon_num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i));
	glEnd();
}

void Modeling_Bar(void) {
	glBegin(GL_QUADS);
	glVertex2f(bar_X, bar_Y);
	glVertex2f(bar_X, bar_Y + bar_height);
	glVertex2f(bar_X + bar_width, bar_Y + bar_height);
	glVertex2f(bar_X + bar_width, bar_Y);
	glEnd();
}

void Collision_Detection_to_Walls(void) {
	if (bottom + height < (moving_ball.y + moving_ball_radius)) {
		velocity.y = -0.05;
	}
}

void Collision_Detection_to_Bar(void) {
	if (moving_ball.x >= bar_X && moving_ball.x <= bar_X + bar_width) {
		if ((bar_Y + bar_height) > (moving_ball.y - moving_ball_radius)) {
			velocity.y = +0.05;
		}
	}
}

void RenderScene(void) {

	glClearColor(1.0, 1.0, 0.0, 0.0); // Set display-window color to Yellow
	glClear(GL_COLOR_BUFFER_BIT);

	// 충돌 처리 부분
	Collision_Detection_to_Walls();			// 공과 벽의 충돌 함수
	Collision_Detection_to_Bar();			// 공과 막대기 충돌 함수

	// 움직이는 공의 위치 변화 
	moving_ball.x += velocity.x;	//움직이는 공의 x좌표
	moving_ball.y += velocity.y;	//움직이는 공의 y좌표

	// 움직이는 공 그리기 
	glColor3f(0.0, 0.0, 1.0);
	Modeling_Circle(moving_ball_radius, moving_ball);

	// 움직이는 막대기 그리기
	glColor3f(1.0, 0.0, 1.0);
	Modeling_Bar();

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