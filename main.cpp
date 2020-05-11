#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
#include <GL/glew.h>
#endif
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include "planets.hpp"

// view params kept for moving using keys
static GLdouble eyeX = 0.0, eyeY = 15.0, eyeZ = 15.0, centerX = 0.0,
                centerY = 0.0, centerZ = 0.0, upX = 0.0, upY = 1.0, upZ = 0.0;

static double angleX = 0.0, angleY = 0.0;
static int g_width = 700, g_height = 700;

static void initLighting() {
  // Lighting set up
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  // Set lighting intensity and color
  GLfloat qaAmbientLight[] = {0.1f, 0.1f, 0.1f, 1.0f};
  GLfloat qaDiffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat qaSpecularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

  // Set the light position
  GLfloat qaLightPosition[] = {0.0, 0.0, 0.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
}

static void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // test depth ensure ze don't draw object behind eachother
  glEnable(GL_DEPTH_TEST);
  draw_solar();

  glFlush();
}

// adapt geometry to window
static void reshape(int w, int h) {
  glLoadIdentity();
  glViewport(0, 0, w, h);
  gluPerspective(5.0, static_cast<GLdouble>(w) / static_cast<GLdouble>(h), 3.0,
                 90.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
  g_width = w;
  g_height = h;
}

static void specialKey(int key, int, int) {
  const double step = 0.1;
  switch (key) {
  case GLUT_KEY_UP:
    angleX += step;
    if (angleX > 2 * M_PI) {
      angleX -= 2 * M_PI;
    }
    eyeX = 0.0;
    eyeY = 15.0 * cos(angleX);
    eyeZ = 15.0 * sin(angleX);
    break;
  case GLUT_KEY_DOWN:
    angleX -= step;
    if (angleX < 0.0) {
      angleX += 2 * M_PI;
    }
    eyeX = 0.0;
    eyeY = 15.0 * cos(angleX);
    eyeZ = 15.0 * sin(angleX);
    break;
  case GLUT_KEY_LEFT:
    angleY += step;
    if (angleY > 2 * M_PI) {
      angleY -= 2 * M_PI;
    }
    eyeX = 15.0 * cos(angleY);
    eyeY = 0.0;
    eyeZ = 15.0 * sin(angleY);
    break;
  case GLUT_KEY_RIGHT:
    angleY -= step;
    if (angleY < 0.0) {
      angleY += 2 * M_PI;
    }
    eyeX = 15.0 * cos(angleY);
    eyeY = 0.0;
    eyeZ = 15.0 * sin(angleY);
    break;
  default:
    break;
  }
  // fix axis based on rotation
  if (angleX < M_PI) {
    upY = 1.0;
  } else {
    upY = -1.0;
  }
#ifdef DEBUG
  printf("specialKey %d eye(%f,%f,%f) angle(%f,%f) center(%f,%f,%f) "
         "up(%f,%f,%f)\n",
         key, eyeX, eyeY, eyeZ, angleX, angleY, centerX, centerY, centerZ, upX,
         upY, upZ);
#endif
  reshape(g_width, g_height);
}

static void keyPressed(unsigned char key, int, int) {
  switch (key) {
  case 'q':
  case 'Q':
  // esc
  case 27:
    exit(0);
  default:
    break;
  }
}

static void update(const int) {
  update_solar();

  glutPostRedisplay();
  glutTimerFunc(20, update, 0);
}

#ifdef DEBUG
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}
#endif

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Solar System");
#ifdef DEBUG
  printf("Debug enabled\n");
  glewInit();
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
#endif
  init_solar();
  initLighting();
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutSpecialFunc(specialKey);
  glutKeyboardFunc(keyPressed);
  glutTimerFunc(25, update, 0);
  glutMainLoop();
  return 0;
}
