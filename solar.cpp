#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <functional>

static GLfloat angleMoon = 0.0, angleEarth = 0.0, angleAstroid = 0.0,
               angleMars = 0.0, angleMercury = 0.0, angleVenus = 0.0,
               angleJupiter = 0.0, angleSaturn = 0.0, angleUranus = 30.0,
               angleNeptune = 60.0;
static GLfloat sx = 0.2f, sy = 0.2f, sz = 0.2f;

static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};
// static const GLfloat white[] = {1.0f, 1.0f, 1.0f, 1.0f};
// static const GLfloat blue[] = {0.0f, 0.0f, 0.9f, 1.0f};
// static const GLfloat er[] = {0.0f, 5.0f, 0.9f, 1.0f};
static const GLfloat yellow[] = {0.7f, 0.2f, 0.0f, 1.0f};
static const GLfloat qAmb[] = {0.1, 0.1, 0.1, 1.0};
static const GLfloat qDif[] = {1.0, 1.0, 1.0, 1.0};
static const GLfloat qSpec[] = {.50, .50, .50, .10};
static const GLfloat qPos[] = {0, 0, 0, 0.1};

static GLfloat sc[8] = {0.295, 0.40, 0.50, 0.60, 0.80, 1.0, 1.05, 1.13};
static double ang = 2 * M_PI / 300;
static double angular = 2 * M_PI / 50;

static void initLighting() {
  // glMaterialfv(GL_FRONT,GL_AMBIENT,yellow);
  // glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT7);

  glLightfv(GL_LIGHT7, GL_AMBIENT, qAmb);
  glLightfv(GL_LIGHT7, GL_DIFFUSE, qDif);
  glLightfv(GL_LIGHT7, GL_SPECULAR, qSpec);
  // glMaterialfv(GL_FRONT,GL_DIFFUSE,yellow);
}

static void orbit() {
  glColor3f(0.5, 0.5, 0.5);

  int i = 0;
  for (i = 0; i < 8; i++) {
    glPushMatrix();
    if (i == 5) {
      glRotatef(45, 1.0, 0.0, 0.0);
    } else {
      glRotatef(63, 1.0, 0.0, 0.0);
    }
    glScalef(sc[i], sc[i], sc[i]);
    glBegin(GL_POINTS);
    double ang1 = 0.0;
    int j = 0;
    // creates points along the orbit
    for (j = 0; j < 60; j++) {
      glVertex2d(cos(ang1), sin(ang1));
      ang1 += 6 * ang;
    }
    glEnd();
    glPopMatrix();
  }
}

static void push_pop(std::function<void()> action) {
  glPushMatrix();
  action();
  glPopMatrix();
}

static void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  orbit();

  glPushMatrix();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);

  push_pop([](void) {
    glColor3f(0.7, 0.5, 0.0);
    glScalef(sx, sy, sz);
    glLightfv(GL_LIGHT7, GL_POSITION, qPos);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, yellow);
    glutSolidSphere(1, 50, 50);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
  });

  glScalef(0.2, 0.2, 0.2);

  push_pop([](void) {
    glRotatef(angleMercury, 0.0, 1.0, -0.5);
    glTranslatef(1.5, 0.0, 0.0);
    glColor3f(1.0, 0.9, 0.0);
    glScalef(0.08, 0.08, 0.08);
    glutSolidSphere(1, 50, 50);
  });

  push_pop([](void) {
    glRotatef(angleVenus, 0.0, 1.0, -0.5);
    glTranslatef(2.0, 0.0, 0.0);
    glColor3f(0.9, 0.1, 0.0);
    glScalef(0.1, 0.1, 0.1);
    glutSolidSphere(1, 50, 50);
  });

  push_pop([](void) {
    glRotatef(angleEarth, 0.0, 1.0, -0.5);
    glTranslatef(2.5, 0.0, 0.0);
    glColor3f(0.0, 0.1, 0.7);
    glScalef(0.23, 0.23, 0.23);
    glutSolidSphere(1, 50, 50);
    // create satelite
    push_pop([](void) {
      glRotatef(angleMoon, 0.0, 0.1, 0.05);
      glTranslatef(1.3, 0.0, 0.0);
      glColor3f(1.0, 1.0, 1.0);
      glScalef(0.5, 0.5, 0.5);
      glutSolidSphere(0.5, 50, 50);
    }); // moon made
  });   // earth made

  push_pop([](void) {
    glRotatef(angleMars, 0.0, 1.0, -0.5);
    glTranslatef(-3.0, 0.0, 0.0);
    glColor3f(0.05, 0.05, 0.01);
    glScalef(0.17, 0.17, 0.17);
    glutSolidSphere(1, 50, 50);
  });

  push_pop([](void) {
    glColor3f(3.30, 3.30, 3.30);
    glRotatef(63, 1.0, 0.0, 0.0);
    int div = 90;
    float siz = 2;
    float scl[4] = {3.3, 3.4, 3.35, 3.2};
    for (int j = 0; j < 4; j++) {
      push_pop([&siz, &scl, j, &div](void) {
        siz -= 0.3;
        glPointSize(siz);
        glScalef(scl[j], scl[j], scl[j]);
        glBegin(GL_POINTS);
        double ang1 = 0.0 - angleAstroid, a = (2 * M_PI) / div;
        for (int i = 0; i < div; i++) {
          glVertex2d(cos(ang1), sin(ang1));
          ang1 += a;
        }
        div += 10;
        glEnd();
      });
    }
  }); // astroid made

  push_pop([](void) {
    glRotatef(angleJupiter, 0.0, 1.0, -0.5);
    glTranslatef(-4.0, 0.0, 0.0);
    glColor3f(0.4, 0.2, 0.0);
    glScalef(0.5, 0.5, 0.5);
    glutSolidSphere(1, 50, 50);
    push_pop([](void) {
      glRotatef(angleMoon, 1.0, -0.5, 0.0);
      glTranslatef(0.0, 0, 1.1);
      glColor3f(1.0, 1.0, 1.0);
      glScalef(0.1, 0.1, 0.1);
      glutSolidSphere(0.5, 50, 50);
    }); // moon made
  });

  push_pop([](void) {
    glRotatef(angleSaturn, 0.0, 1.0, -1.0);
    glTranslatef(-5.0, 0.0, 0.0);
    glColor3f(0.9, 0.0, 0.0);
    glScalef(0.4, 0.4, 0.4);
    glutSolidSphere(1, 50, 50);
    push_pop([](void) {
      glRotatef(45, 1.0, 0.0, 0.0);
      glPointSize(3);
      glColor3f(5.0, 3.0, 1.0);
      glScalef(1.2, 1.2, 1.2);
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

  push_pop([](void) {
    glRotatef(angleUranus, 0.0, 1.0, -0.5);
    glTranslatef(5.2, 0.0, 0.0);
    glColor3f(0.0, 0.5, 0.9);
    glScalef(0.23, 0.23, 0.23);
    glutSolidSphere(1, 50, 50);
  });

  push_pop([](void) {
    glRotatef(angleNeptune, 0.0, 1.0, -0.5);
    glTranslatef(-5.7, 0.0, 0.0);
    glColor3f(0.0, 0.0, 0.9);
    glScalef(0.2, 0.2, 0.2);
    glutSolidSphere(1, 50, 50);
  });

  glPopMatrix();
  glFlush();
}

static void update_angle(float &angle, const float change) {
  angle += change;
  if (angle > 360.0f) {
    angle -= 360.0f;
  }
}

static void update(const int) {
  if ((angleMoon >= 0 && angleMoon < 180)) {
    sx -= 0.0003f;
    sy -= 0.0003f;
    sz -= 0.0003f;
  } else {
    sx += 0.0003f;
    sy += 0.0003f;
    sz += 0.0003f;
  }
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

// adapt geometry to window
static void reshape(int w, int h) {
  glLoadIdentity();
  glViewport(0, 0, w, h);
  gluPerspective(5.0, (GLdouble)w / (GLdouble)h, 3.0, 90.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(2.0, 0.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Solar System");
  initLighting();
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutTimerFunc(25, update, 0);
  glutMainLoop();
  return 0;
}
