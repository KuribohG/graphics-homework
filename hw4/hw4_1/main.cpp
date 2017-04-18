#include <iostream>
#include <gl/glut.h>

typedef float Float;

const int MAXV = 100000;
const int MAXM = 100000;
const int MAXT = 100000;

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
    Mesh() {}
    Mesh(int _x, int _y, int _z)
            : x(_x), y(_y), z(_z) {}
    Float f(const Point &p) {
        return Dot(Cross(P[y] - P[x], P[z] - P[x]), p - P[x]);
    }
}face[MAXM];

void get_color() {
    static int k = 0;
    if (k == 0) glColor3f(1.0, 1.0, 1.0);
    else if (k == 1) glColor3f(1.0, 0.0, 1.0);
    else if (k == 2) glColor3f(0.0, 1.0, 1.0);
    else if (k == 3) glColor3f(1.0, 1.0, 0.0);
    else if (k == 4) glColor3f(1.0, 0.0, 0.0);
    else if (k == 5) glColor3f(0.0, 1.0, 0.0);
    else if (k == 6) glColor3f(0.0, 0.0, 1.0);
    k = (k + 1) % 7;
}

void draw_mesh(const Mesh &f) {
    get_color();
    glBegin(GL_TRIANGLES);
    glVertex3f(P[f.x].x, P[f.x].y, P[f.x].z);
    glVertex3f(P[f.y].x, P[f.y].y, P[f.y].z);
    glVertex3f(P[f.z].x, P[f.z].y, P[f.z].z);
    glEnd();
}

struct Tree {
    int mesh_id;
    int lch;
    int rch;
}BSP[MAXT];

int tot = 1;

void add(int x, int id) {
    int cur = BSP[x].mesh_id;
    if (cur == 0) {
        BSP[x].mesh_id = id;
        return;
    }
    Float a = face[cur].f(P[face[id].x]);
    Float b = face[cur].f(P[face[id].y]);
    Float c = face[cur].f(P[face[id].z]);
    if (a <= 0 && b <= 0 && c <= 0) {
        if (BSP[x].rch == 0) {
            BSP[x].rch = ++tot;
        }
        add(BSP[x].rch, id);
    } else if (a >= 0 && b >= 0 && c >= 0) {
        if (BSP[x].lch == 0) {
            BSP[x].lch = ++tot;
        }
        add(BSP[x].lch, id);
    } else {
        printf("Painter algorithm cannot process face %d.\n", id);
    }
}

void visit(int x, const Point &camera) {
    if (BSP[x].mesh_id == 0) {
        return;
    }
    Float a = face[BSP[x].mesh_id].f(camera);
    if (a < 0) {
        visit(BSP[x].lch, camera);
        draw_mesh(face[BSP[x].mesh_id]);
        visit(BSP[x].rch, camera);
    } else {
        visit(BSP[x].rch, camera);
        draw_mesh(face[BSP[x].mesh_id]);
        visit(BSP[x].lch, camera);
    }
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
}

void plot() {
    glClear(GL_COLOR_BUFFER_BIT);
    Point camera(0.5, 0.5, 5.0);
    visit(1, camera);
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)h, (GLsizei)w);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.5, 0.5, 5.0, 0.5, 0.5, 0.0, 0.0, 1.0, 0.0);
}

int main(int argc, char **argv) {
    init_model();
    for (int i = 1; i <= m; i++) {
        add(1, i);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("painter");
    glutDisplayFunc(plot);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}