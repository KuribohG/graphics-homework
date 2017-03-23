#include <cmath>
#include <vector>
#include <list>
#include <gl/glut.h>

const int WIDTH = 400;
const int HEIGHT = 400;
const double eps = 1e-10;

typedef float Float;

struct Point {
    int x, y;
    Point() {}
    Point(int _x, int _y) : x(_x), y(_y) {}
};

struct Segment {
    Point p, q;
    Segment() {}
    Segment(Point x, Point y) : p(x), q(y) {
        if (p.y > q.y) {
            std::swap(p, q);
        }
    }
    Float get_inverse_slope() {
        return (q.x - p.x) * 1.0f / (q.y - p.y);
    }
};

struct Node {
    Float x_int, delta;
    int y_high;
    Node(Float x, Float y, int z)
            : x_int(x), delta(y), y_high(z) {}
};

bool operator < (const Node &p, const Node &q) {
    return p.x_int < q.x_int;
}

std::list<Node> ael;
std::list<Node> et[HEIGHT * 3];
__uint8_t buffer[WIDTH * HEIGHT * 3];
__uint8_t aug_buffer[WIDTH * HEIGHT * 27];
Float filter[3][3] = {{1.0f / 16, 2.0f / 16, 1.0f / 16},
                      {2.0f / 16, 4.0f / 16, 2.0f / 16},
                      {1.0f / 16, 2.0f / 16, 1.0f / 16}};
Float img[WIDTH * HEIGHT * 3];

void initialize(Segment edge[], int n, int h) {
    ael.clear();
    for (int i = 0; i < h; i++) {
        et[i].clear();
    }
    for (int i = 0; i < n; i++) {
        if (edge[i].p.y != edge[i].q.y) {
            Node node(edge[i].p.x, edge[i].get_inverse_slope(), edge[i].q.y);
            et[edge[i].p.y].push_back(node);
        }
    }
    for (int i = 0; i < h; i++) {
        et[i].sort();
    }
}

void scan_line_filling(Segment edge[], int n, int h, int w, __uint8_t buffer[]) {
    initialize(edge, n, h);
    for (int i = 0; i < h; i++) {
        std::vector<int> v;
        int sz = 0;
        for (std::list<Node>::iterator it = ael.begin(); it != ael.end(); ++it) {
            if (sz & 1) {
                double t = it->x_int;
                if (t - int(t) < eps) {
                    v.push_back(int(t) - 1);
                } else {
                    v.push_back(int(t));
                }
            } else {
                v.push_back(int(std::ceil(it->x_int)));
            }
            sz++;
        }
        for (int o = 0; o < v.size(); o += 2) {
            for (int k = v[o]; k <= v[o + 1]; k++) {
                for (int ch = 0; ch < 3; ch++) {
                    buffer[(i * w + k) * 3 + ch] = __uint8_t(128);
                }
            }
        }
        ael.merge(et[i]);
        std::list<Node>::iterator it = ael.begin();
        while (it != ael.end()) {
            if (i == it->y_high) {
                ael.erase(it++);
            } else {
                ++it;
            }
        }
        for (Node &x : ael) {
            x.x_int += x.delta;
        }
    }
}

int n;
Segment edge[1000];
Segment edge_aug[1000];
bool anti_aliasing;

void plot() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (anti_aliasing) {
        memset(aug_buffer, 0, sizeof(aug_buffer));
        memset(buffer, 0, sizeof(buffer));
        for (int i = 0; i < HEIGHT * WIDTH * 3; i++) {
            img[i] = 0.0f;
        }
        scan_line_filling(edge_aug, n, HEIGHT * 3, WIDTH * 3, aug_buffer);
        for (int i = 0; i < HEIGHT * 3; i++) {
            for (int j = 0; j < WIDTH * 3; j++) {
                for (int ch = 0; ch < 3; ch++) {
                    img[((i / 3) * WIDTH + (j / 3)) * 3 + ch]
                            += filter[i % 3][j % 3] * aug_buffer[(i * WIDTH * 3 + j) * 3 + ch];
                }
            }
        }
        for (int i = 0; i < HEIGHT * WIDTH * 3; i++) {
            buffer[i] = __uint8_t(std::roundf(img[i]));
        }
    } else {
        memset(buffer, 0, sizeof(buffer));
        scan_line_filling(edge, n, HEIGHT, WIDTH, buffer);
    }
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    anti_aliasing = true;

    Point point[20] = {
            Point(52 * 4, 30 * 4),
            Point(74 * 4, 43 * 4),
            Point(60 * 4, 60 * 4),
            Point(38 * 4, 60 * 4),
            Point(30 * 4, 50 * 4),
            Point(10 * 4, 50 * 4),
            Point(10 * 4, 28 * 4),
            Point(22 * 4, 41 * 4),
            Point(33 * 4, 10 * 4),
            Point(50 * 4, 10 * 4),
            Point(39 * 4, 30 * 4)
    };
    point[11] = point[0];
    for (int i = 0; i < 11; i++) {
        edge[n] = Segment(point[i], point[i + 1]);
        edge_aug[n] = edge[n];
        edge_aug[n].p.x *= 3;
        edge_aug[n].p.y *= 3;
        edge_aug[n].q.x *= 3;
        edge_aug[n].q.y *= 3;
        n++;
    }
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("hw1");
    glutDisplayFunc(plot);
    glutMainLoop();

    return 0;
}