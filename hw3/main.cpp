#include <cmath>
#include <algorithm>
#include <gl/glut.h>

typedef float Float;

const int WIDTH = 400;
const int HEIGHT = 400;
const int MAX_POINTS = 10000;

struct Point {
    Float x, y;
    Point() {}
    Point(Float _x, Float _y) : x(_x), y(_y) {}
};

typedef Point Vector;

Vector operator + (const Vector &p, const Vector &q) {
    return Vector(p.x + q.x, p.y + q.y);
}

Vector operator - (const Vector &p, const Vector &q) {
    return Vector(p.x - q.x, p.y - q.y);
}

Vector operator * (const Vector &p, Float x) {
    return Vector(p.x * x, p.y * x);
}

void draw_dot(const Point &p) {
    glBegin(GL_POINTS);
    glVertex2f(p.x, p.y);
    glEnd();
}

void draw_line(const Point &p, const Point &q) {
    glBegin(GL_LINES);
    glVertex2f(p.x, p.y);
    glVertex2f(q.x, q.y);
    glEnd();
}

void draw_dash_line(const Point &p, const Point &q) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x0F0F);
    glBegin(GL_LINES);
    glVertex2f(p.x, p.y);
    glVertex2f(q.x, q.y);
    glEnd();
    glLineStipple(1, 0xFFFF);
    glDisable(GL_LINE_STIPPLE);
}

int n;
Point p[1000], tmp[1000];

void process_mouse(int button, int state, int x, int y) {
    if (state == 0) {
        Float px = (x - WIDTH / 2) * 1.0f / (WIDTH / 2);
        Float py = ((HEIGHT - y) - HEIGHT / 2) * 1.0f / (HEIGHT / 2);
        p[n] = Point(px, py);
        n++;
    }
    glutPostRedisplay();
}

Point get_bezier(Point p[], int n, Float t) {
    if (n == 1) {
        return p[0];
    }
    for (int i = 0; i < n - 1; i++) {
        tmp[i] = p[i] + (p[i + 1] - p[i]) * t;
    }
    return get_bezier(tmp, n - 1, t);
}

void plot() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < n - 1; i++) {
        draw_dash_line(p[i], p[i + 1]);
    }
    if (n > 1) {
        for (int i = 0; i <= MAX_POINTS; i++) {
            Float t = i * 1.0f / MAX_POINTS;
            Point res = get_bezier(p, n, t);
            draw_dot(res);
        }
    }
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("hw3");
    glutMouseFunc(process_mouse);
    glutDisplayFunc(plot);
    glutMainLoop();

    return 0;
}