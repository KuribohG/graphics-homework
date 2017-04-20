#include <iostream>
#include <algorithm>
#include <gl/glut.h>

typedef float Float;

const int MAXV = 100000;
const int MAXM = 100000;
const int HEIGHT = 400;
const int WIDTH = 400;

int n;
struct Point {
    Float x, y, z;
    Point() {}
    Point(Float _x, Float _y, Float _z)
            : x(_x), y(_y), z(_z) {}
}P[MAXV];

typedef Point Vector;

Vector operator+(const Vector &p, const Vector &q) {
    return Vector(p.x + q.x, p.y + q.y, p.z + q.z);
}

Vector operator-(const Vector &p, const Vector &q) {
    return Vector(p.x - q.x, p.y - q.y, p.z - q.z);
}

Float Dot(const Vector &p, const Vector &q) {
    return p.x * q.x + p.y * q.y + p.z * q.z;
}

Vector Cross(const Vector &p, const Vector &q) {
    return Vector(p.y * q.z - p.z * q.y,
                  p.z * q.x - p.x * q.z,
                  p.x * q.y - p.y * q.x);
}

int m;
struct Mesh {
    int x, y, z;
    int color;
    Mesh() {}
    Mesh(int _x, int _y, int _z)
            : x(_x), y(_y), z(_z) {}
    Float f(const Point &p) {
        return Dot(Cross(P[y] - P[x], P[z] - P[x]), p - P[x]);
    }
}face[MAXM];

__uint8_t r, g, b;

void get_color(int k) {
    if (k == 0) r = 255, g = 255, b = 255;
    else if (k == 1) r = 255, g = 0, b = 255;
    else if (k == 2) r = 0, g = 255, b = 255;
    else if (k == 3) r = 255, g = 255, b = 0;
    else if (k == 4) r = 255, g = 0, b = 0;
    else if (k == 5) r = 0, g = 255, b = 0;
    else if (k == 6) r = 0, g = 0, b = 255;
    else if (k == 7) r = 128, g = 128, b = 128;
}

void init_model() {
    P[++n] = Point(1, 1, 1);
    P[++n] = Point(-1, 1, 1);
    P[++n] = Point(-1, -1, 1);
    P[++n] = Point(1, -1, 1);
    P[++n] = Point(3, 3, -1);
    P[++n] = Point(1, 3, -1);
    P[++n] = Point(1, 1, -1);
    P[++n] = Point(3, 1, -1);

    face[++m] = Mesh(1, 2, 3);
    face[++m] = Mesh(3, 4, 1);
    face[++m] = Mesh(6, 7, 8);
    face[++m] = Mesh(8, 5, 6);
    face[++m] = Mesh(1, 5, 6);
    face[++m] = Mesh(6, 2, 1);
    face[++m] = Mesh(3, 7, 8);
    face[++m] = Mesh(8, 4, 3);
    face[++m] = Mesh(2, 6, 7);
    face[++m] = Mesh(7, 3, 2);
    face[++m] = Mesh(4, 8, 5);
    face[++m] = Mesh(5, 1, 4);

    for (int i = 1; i <= m; i++) {
        face[i].color = i % 8;
    }
}

Float get_z(const Point &camera, const Point &look_at, const Mesh &p) {
    Point &x = P[p.x];
    Point &y = P[p.y];
    Point &z = P[p.z];
    Vector direction = look_at - camera;
    Vector E1 = y - x, E2 = z - x;
    Vector P = Cross(direction, E2);
    Float det = Dot(E1, P);
    Vector T;
    if (det > 0) {
        T = camera - x;
    } else {
        T = x - camera;
        det = -det;
    }
    if (det < 0.0001) {
        return 1e31;
    }
    Float u = Dot(T, P);
    if (u < 0.0 || u > det) {
        return 1e31;
    }
    Vector Q = Cross(T, E1);
    Float v = Dot(direction, Q);
    if (v < 0.0 || u + v > det) {
        return 1e31;
    }
    Float z_dep = Dot(E2, Q);
    z_dep /= det;
    return z_dep;
}

Float z_buf[HEIGHT * WIDTH];
__uint8_t img[HEIGHT * WIDTH * 3];

void z_buffer() {
    for (int i = 0; i < HEIGHT * WIDTH; i++) {
        z_buf[i] = 1e30;
    }
    for (int i = 0; i < HEIGHT * WIDTH * 3; i++) {
        img[i] = 0;
    }
    Point camera(0.5, 0.5, 5.0);
    Float min_x = -10, max_x = 10, min_y = -10, max_y = 10;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int z_base = i * WIDTH + j;
            int base = (i * WIDTH + j) * 3;
            Float x = j * (max_x - min_x) / (WIDTH - 1) + min_x;
            Float y = i * (max_y - min_y) / (HEIGHT - 1) + min_y;
            Point look_at = Point(x, y, 0.0);
            for (int k = 1; k <= m; k++) {
                Float z = get_z(camera, look_at, face[k]);
                if (z < z_buf[z_base]) {
                    get_color(face[k].color);
                    z_buf[z_base] = z;
                    img[base] = r;
                    img[base + 1] = g;
                    img[base + 2] = b;
                }
            }
        }
    }
}

void plot() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, img);
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    init_model();
    z_buffer();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("zbuffer");
    glutDisplayFunc(plot);
    glutMainLoop();

    return 0;
}