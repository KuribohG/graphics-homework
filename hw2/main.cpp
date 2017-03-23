#include <string>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <gl/glut.h>

typedef float Float;

const int HEIGHT = 400;
const int WIDTH = 400;
const Float WINDOW_X1 = -0.5f;
const Float WINDOW_Y1 = -0.5f;
const Float WINDOW_X2 = 0.5f;
const Float WINDOW_Y2 = 0.5f;
const Float eps = 1e-10;

struct Point {
    Float x, y;
    Point() {}
    Point(Float _x, Float _y) : x(_x), y(_y) {}
};

struct Segment {
    Point p, q;
    Segment() {}
    Segment(const Point &_p, const Point &_q)
            : p(_p), q(_q) {}
};

void draw_line(Segment s) {
    glBegin(GL_LINES);
    glVertex2f(s.p.x, s.p.y);
    glVertex2f(s.q.x, s.q.y);
    glEnd();
}

void draw_dash_line(Segment s) {
    glLineStipple(1, 0x0F0F);
    glBegin(GL_LINES);
    glVertex2f(s.p.x, s.p.y);
    glVertex2f(s.q.x, s.q.y);
    glEnd();
    glLineStipple(1, 0xFFFF);
}

int n;
std::string algo;
Segment seg[1000];

__uint8_t get_mask(const Point &p, Float x1, Float y1, Float x2, Float y2) {
    __uint8_t mask = 0;
    if (p.x < x1) {
        mask ^= (1 << 0);
    }
    if (p.x > x2) {
        mask ^= (1 << 1);
    }
    if (p.y < y1) {
        mask ^= (1 << 2);
    }
    if (p.y > y2) {
        mask ^= (1 << 3);
    }
    return mask;
}

int cohen_sutherland(Segment &s, Float x1, Float y1, Float x2, Float y2) {
    Point p1 = s.p, p2 = s.q;
    int t = 10;
    while (t--) {
        __uint8_t mask1 = get_mask(p1, x1, y1, x2, y2);
        __uint8_t mask2 = get_mask(p2, x1, y1, x2, y2);
        if (mask1 == 0 && mask2 == 0) {
            return 1;
        }
        if (mask1 & mask2) {
            return 0;
        }
        Float delx = p2.x - p1.x;
        Float dely = p2.y - p1.y;
        if (mask1) {
            if (mask1 & (1 << 0)) {
                p1.y += (x1 - p1.x) * dely / delx;
                p1.x = x1;
            } else if (mask1 & (1 << 1)) {
                p1.y += (x2 - p1.x) * dely / delx;
                p1.x = x2;
            } else if (mask1 & (1 << 2)) {
                p1.x += (y1 - p1.y) * delx / dely;
                p1.y = y1;
            } else if (mask1 & (1 << 3)) {
                p1.x += (y2 - p1.y) * delx / dely;
                p1.y = y2;
            }
        } else if (mask2) {
            if (mask2 & (1 << 0)) {
                p2.y += (x1 - p2.x) * dely / delx;
                p2.x = x1;
            } else if (mask2 & (1 << 1)) {
                p2.y += (x2 - p2.x) * dely / delx;
                p2.x = x2;
            } else if (mask2 & (1 << 2)) {
                p2.x += (y1 - p2.y) * delx / dely;
                p2.y = y1;
            } else if (mask2 & (1 << 3)) {
                p2.x += (y2 - p2.y) * delx / dely;
                p2.y = y2;
            }
        }
        s = Segment(p1, p2);
    }
    return 1;
}

int liang_barsky(Segment &s, Float x1, Float y1, Float x2, Float y2) {
    Point pp = s.p, qq = s.q;
    Float delx = qq.x - pp.x, dely = qq.y - pp.y;
    Float p[4] = {-delx, delx, -dely, dely};
    Float q[4] = {pp.x - x1, x2 - pp.x, pp.y - y1, y2 - pp.y};
    Float u[4], u1 = 0, u2 = 1;
    for (int i = 0; i < 4; i++) {
        if (std::fabs(p[i]) < eps) {
            if (q[i] < 0) {
                return 0;
            }
        } else {
            u[i] = q[i] / p[i];
            if (p[i] < 0) {
                u1 = std::max(u1, u[i]);
            } else {
                u2 = std::min(u2, u[i]);
            }
        }
    }
    if (u1 > u2) {
        return 0;
    } else {
        s = Segment(Point(pp.x + u1 * delx, pp.y + u1 * dely),
                    Point(pp.x + u2 * delx, pp.y + u2 * dely));
        return 1;
    }
}

void plot() {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_LINE_STIPPLE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int i = 0; i < n; i++) {
        draw_dash_line(seg[i]);
    }
    if (algo == "cs") {
        for (int i = 0; i < n; i++) {
            int res = cohen_sutherland(seg[i], WINDOW_X1, WINDOW_Y1, WINDOW_X2, WINDOW_Y2);
            if (res == 1) {
                draw_line(seg[i]);
            }
        }
    } else if (algo == "lb") {
        for(int i = 0; i < n; i++) {
            int res = liang_barsky(seg[i], WINDOW_X1, WINDOW_Y1, WINDOW_X2, WINDOW_Y2);
            if (res == 1) {
                draw_line(seg[i]);
            }
        }
    }
    glRectf(WINDOW_X1, WINDOW_Y1, WINDOW_X2, WINDOW_Y2);
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    algo = "cs";
    assert(algo == "cs" || algo == "lb");

    n = 4;
    seg[0] = Segment(Point(-0.25f, -0.25f), Point(0.25f, 0.25f));
    seg[1] = Segment(Point(-0.7f, -0.8f), Point(-0.7f, -0.2f));
    seg[2] = Segment(Point(-0.7f, 0.7f), Point(0.7f, -0.7f));
    seg[3] = Segment(Point(-0.3f, -0.3f), Point(0.75f, 0.0f));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("hw2");
    glutDisplayFunc(plot);
    glutMainLoop();

    return 0;
}