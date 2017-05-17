#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <gl/glew.h>
#include <gl/glut.h>

const int WIDTH = 256;
const int HEIGHT = 256;

std::string readFile(const char *filePath) {
    std::string content = "";
    std::ifstream fileStream(filePath, std::ios::in);

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

GLuint LoadShaders(const char *vertFile, const char *fragFile) {
    glewInit();
    GLint result = GL_FALSE;
    GLchar log[512];

    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

    std::string svs = readFile(vertFile);
    const char *vs = svs.c_str();
    std::string sfs = readFile(fragFile);
    const char *fs = sfs.c_str();

    glShaderSource(v, 1, &vs, NULL);
    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &result);
    if (!result) {
        std::cout << "Vertex shader compile error:" << std::endl;
        glGetShaderInfoLog(v, 512, NULL, log);
        std::cout << log << std::endl;
    }

    glShaderSource(f, 1, &fs, NULL);
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &result);
    if (!result) {
        std::cout << "Fragment shader compile error:" << std::endl;
        glGetShaderInfoLog(f, 512, NULL, log);
        std::cout << log << std::endl;
    }

    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);

    glLinkProgram(p);
    glGetProgramiv(p, GL_LINK_STATUS, &result);
    if (!result) {
        std::cout << "Program link error:" << std::endl;
        glGetProgramInfoLog(p, 512, NULL, log);
        std::cout << log << std::endl;
    }

    return p;
}

GLuint LoadTexture(unsigned char *data, int width, int height) {
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    return texture;
}

GLfloat lightPosition[] = {-1.0, -1.0, 1.0};
GLfloat eyePosition[] = {0.0, 0.0, 2.0};

void plot() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint shader = LoadShaders("bump_mapping.vert", "bump_mapping.frag");
    glUseProgram(shader);

    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, WIDTH / HEIGHT, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyePosition[0], eyePosition[1], eyePosition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    int imageSize = 1024 * 1024 * 3;
    unsigned char *brick = new unsigned char[imageSize];
    FILE *f = fopen("brick.txt", "r");
    for (int i = 0; i < imageSize; i++) {
        int x;
        fscanf(f, "%d", &x);
        brick[i] = (unsigned char)x;
    }
    fclose(f);

    unsigned char *brick_texture = new unsigned char[imageSize];
    FILE *g = fopen("brick_texture.txt", "r");
    for (int i = 0; i < imageSize; i++) {
        int x;
        fscanf(g, "%d", &x);
        brick_texture[i] = (unsigned char)x;
    }
    fclose(g);

    GLuint texture = LoadTexture(brick, 1024, 1024);
    GLuint normal_texture = LoadTexture(brick_texture, 1024, 1024);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    GLint texture_location = glGetUniformLocation(shader, "texture");
    glUniform1i(texture_location, 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    GLint normal_location = glGetUniformLocation(shader, "normal_texture");
    glUniform1i(normal_location, 1);
    glBindTexture(GL_TEXTURE_2D, normal_texture);

    GLint lightLocation = glGetUniformLocation(shader, "light_pos");
    glUniform3f(lightLocation, lightPosition[0], lightPosition[1], lightPosition[2]);
    GLint eyeLocation = glGetUniformLocation(shader, "eye_pos");
    glUniform3f(eyeLocation, eyePosition[0], eyePosition[1], eyePosition[2]);

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(1.0, -1.0, -1.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(1.0, 1.0, -1.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, 1.0, -1.0);
    glEnd();

    delete [] brick;
    delete [] brick_texture;

    glutSwapBuffers();
}

void process_key(unsigned char key, int x, int y) {
    GLfloat step = 0.1;
    if (key == 'a') {
        lightPosition[0] -= step;
    } else if (key == 'd') {
        lightPosition[0] += step;
    } else if (key == 'w') {
        lightPosition[1] += step;
    } else if (key == 's') {
        lightPosition[2] += step;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("hw5");
    glutDisplayFunc(plot);
    glutKeyboardFunc(process_key);
    glutMainLoop();

    return 0;
}