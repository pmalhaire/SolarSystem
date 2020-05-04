#include <stdio.h>
#include <stdlib.h>
#ifdef DEBUG
#include <GL/glew.h>
#endif
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <functional>
#include "textures.hpp"

static GLfloat angleMoon = 0.0, angleEarth = 0.0, angleAstroid = 0.0,
               angleMars = 0.0, angleMercury = 0.0, angleVenus = 0.0,
               angleJupiter = 0.0, angleSaturn = 0.0, angleUranus = 30.0,
               angleNeptune = 60.0;

// scale for sun
static GLdouble sizeSun = 0.2;

// orbit of each planet
static GLfloat distMercury = 0.295f, distVenus = 0.4f, distEarth = 0.5f,
               distMars = 0.6f, distJupiter = 0.8f, distSaturn = 1.0f,
               distUranus = 1.05f, distNeptune = 1.13f, distEarthMoon = 0.0598f,
               distJupiterMoon = 0.11f;
static GLdouble sizeMercury = 0.016, sizeVenus = 0.02, sizeEarth = 0.046,
                sizeMars = 0.034, sizeJupiter = 0.1, sizeSaturn = 0.08,
                sizeUranus = 0.046, sizeNeptune = 0.04, sizeEarthMoon = 0.023,
                sizeJupiterMoon = 0.005;

static double angular = 2 * M_PI / 50;

// view params kept for moving using keys
static GLdouble eyeX = 0.0, eyeY = 15.0, eyeZ = 15.0, centerX = 0.0,
                centerY = 0.0, centerZ = 0.0, upX = 0.0, upY = 1.0, upZ = 0.0;

static double angleX = 0.0, angleY = 0.0;
static int g_width = 700, g_height = 700;

static GLuint sun_tex, moon_tex, earth_tex, mercury_tex, venus_tex, mars_tex,
    jupiter_tex, saturn_tex, uranus_tex, neptune_tex;

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

static void push_pop(std::function<void()> action) {
  glPushMatrix();
  action();
  glPopMatrix();
}

static std::vector<double> circle(const int points) {
  std::vector<double> vertices;
  double ang = 0.0;
  static const double step = 2.0 * M_PI / static_cast<double>(points);
  // creates points along the orbit
  for (int j = 0; j < points; j++) {
    vertices.push_back(cos(ang));
    vertices.push_back(sin(ang));
    ang += step;
  }
  return vertices;
}

static void orbit(GLfloat scale) {
  push_pop([scale](void) {
    glColor3f(0.3f, 0.3f, 0.3f);
    glEnable(GL_LINE_SMOOTH);
    glRotatef(63, 1.0, 0.0, 0.0);
    glScalef(scale, scale, scale);
    glBegin(GL_LINE_LOOP);

    static const int points = 50;
    auto vertices = circle(points);
    for (unsigned long j = 0; j < 2 * points; j += 2) {
      glVertex2d(vertices[j], vertices[j + 1]);
    }
    glEnd();
    glDisable(GL_LINE_SMOOTH);
  });
}

static void create_sphere(GLdouble radius, GLint slice, GLint stacks,
                          GLuint texIdx) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texIdx);
  GLUquadricObj *quadric = gluNewQuadric();
  gluQuadricTexture(quadric, true);
  gluQuadricNormals(quadric, GLU_SMOOTH);
  gluSphere(quadric, radius, slice, stacks);
  glDisable(GL_TEXTURE_2D);
}

static void sun(void) {
  push_pop([](void) { create_sphere(sizeSun, 50, 50, sun_tex); });
}

static void planet(GLfloat dist, GLdouble size, GLfloat angle, GLuint tex) {
  orbit(dist);
  push_pop([&angle, &dist, &size, &tex](void) {
    glRotatef(angle, 0.0, 1.0, -0.5);
    glTranslatef(dist, 0.0, 0.0);
    create_sphere(size, 50, 50, tex);
  });
}

static void planet_with_moon(GLfloat dist, GLdouble size, GLfloat angle,
                             GLuint tex, GLfloat distMoon, GLdouble sizeMoon,
                             GLfloat planetMoon, GLuint texMoon) {
  planet(dist, size, angle, tex);
  push_pop([&angle, &dist, &distMoon, &sizeMoon, &planetMoon, &texMoon](void) {
    glRotatef(angle, 0.0, 1.0, -0.5);
    glTranslatef(dist, 0.0, 0.0);
    planet(distMoon, sizeMoon, planetMoon, texMoon);
  });
}

static void asteroid(void) {
  push_pop([](void) {
    glColor3f(3.30f, 3.30f, 3.30f);
    glRotatef(63.0f, 1.0f, 0.0f, 0.0f);
    int div = 90;
    float siz = 2;
    float scl[4] = {0.66f, 0.68f, 0.67f, 0.64f};
    for (int j = 0; j < 4; j++) {
      push_pop([&siz, &scl, &j, &div](void) {
        siz -= 0.3f;
        glPointSize(siz);
        glScalef(scl[j], scl[j], scl[j]);
        glBegin(GL_POINTS);
        double ang1 = 0.0 - static_cast<GLdouble>(angleAstroid),
               a = (2 * M_PI) / div;
        for (int i = 0; i < div; i++) {
          glVertex2d(cos(ang1), sin(ang1));
          ang1 += a;
        }
        div += 10;
        glEnd();
      });
    }
  }); // astroid made
}

static void saturn(void) {
  orbit(distSaturn);
  push_pop([](void) {
    glRotatef(angleSaturn, 0.0f, 1.0f, -1.0f);
    glTranslatef(-distSaturn, 0.0f, 0.0f);

    create_sphere(sizeSaturn, 50, 50, saturn_tex);
    push_pop([](void) {
      glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
      glPointSize(3);
      glScalef(0.096f, 0.096f, 0.096f);
      glBegin(GL_POINTS);
      double ang1 = 0.0;
      for (int i = 0; i < 50; i++) {
        glVertex2d(cos(ang1), sin(ang1));
        ang1 += angular;
      }
      glEnd();

      glPointSize(2);
    }); // ring made
  });
}

static void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // test depth ensure ze don't draw object behind eachother
  glEnable(GL_DEPTH_TEST);

  sun();

  planet(distMercury, sizeMercury, angleMercury, mercury_tex);
  planet(distVenus, sizeVenus, angleVenus, venus_tex);
  planet_with_moon(distEarth, sizeEarth, angleEarth, earth_tex, distEarthMoon,
                   sizeEarthMoon, angleMoon, moon_tex);
  planet(distMars, sizeMars, angleMars, mars_tex);
  planet_with_moon(distJupiter, sizeJupiter, angleJupiter, jupiter_tex,
                   distJupiterMoon, sizeJupiterMoon, angleMoon, moon_tex);
  saturn();
  planet(distUranus, sizeUranus, angleUranus, uranus_tex);
  planet(distNeptune, sizeNeptune, angleNeptune, neptune_tex);
  asteroid();

  glFlush();
}

static void update_angle(float &angle, const float change) {
  angle += change;
  if (angle > 360.0f) {
    angle -= 360.0f;
  }
  if (angle < 0.0f) {
    angle += 360.0f;
  }
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
    eyeY = 15.0 * cos(angleX);
    eyeZ = 15.0 * sin(angleX);
    break;
  case GLUT_KEY_DOWN:
    angleX -= step;
    if (angleX < 0.0) {
      angleX += 2 * M_PI;
    }
    eyeY = 15.0 * cos(angleX);
    eyeZ = 15.0 * sin(angleX);
    break;
  case GLUT_KEY_LEFT:
    angleY += step;
    if (angleY > 2 * M_PI) {
      angleY -= 2 * M_PI;
    }
    eyeX = 15.0 * cos(angleY);
    eyeZ = 15.0 * sin(angleY);
    break;
  case GLUT_KEY_RIGHT:
    angleY -= step;
    if (angleY < 0.0) {
      angleY += 2 * M_PI;
    }
    eyeX = 15.0 * cos(angleY);
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
  update_angle(angleMoon, 2.0f);
  update_angle(angleEarth, 0.7f);
  update_angle(angleMercury, 2.0f);
  update_angle(angleVenus, 0.9f);
  update_angle(angleMars, 0.5f);
  update_angle(angleJupiter, 0.2f);
  update_angle(angleSaturn, 0.1f);
  update_angle(angleUranus, 0.05f);
  update_angle(angleNeptune, 0.02f);
  update_angle(angleAstroid, 0.002f);

  glutPostRedisplay();
  glutTimerFunc(20, update, 0);
}

static void load_textures() {
  load_texture("images/sun.jpg", &sun_tex);
  load_texture("images/moon.jpg", &moon_tex);
  load_texture("images/earth.jpg", &earth_tex);
  load_texture("images/mercury.jpg", &mercury_tex);
  load_texture("images/venus.jpg", &venus_tex);
  load_texture("images/mars.jpg", &mars_tex);
  load_texture("images/jupiter.jpg", &jupiter_tex);
  load_texture("images/saturn.jpg", &saturn_tex);
  load_texture("images/uranus.jpg", &uranus_tex);
  load_texture("images/neptune.jpg", &neptune_tex);
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
  load_textures();
  initLighting();
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutSpecialFunc(specialKey);
  glutKeyboardFunc(keyPressed);
  glutTimerFunc(25, update, 0);
  glutMainLoop();
  return 0;
}
