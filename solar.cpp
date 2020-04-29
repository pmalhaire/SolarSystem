#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <FreeImage.h>
#include <functional>

static GLfloat angleMoon = 0.0, angleEarth = 0.0, angleAstroid = 0.0,
               angleMars = 0.0, angleMercury = 0.0, angleVenus = 0.0,
               angleJupiter = 0.0, angleSaturn = 0.0, angleUranus = 30.0,
               angleNeptune = 60.0;
static GLfloat sx = 0.2f, sy = 0.2f, sz = 0.2f;

static GLfloat sc[8] = {0.295, 0.40, 0.50, 0.60, 0.80, 1.0, 1.05, 1.13};
static double ang = 2 * M_PI / 300;
static double angular = 2 * M_PI / 50;

static GLdouble eyeX = 0.0, eyeY = 15.0, eyeZ = 15.0, centerX = 0.0,
                centerY = 0.0, centerZ = 0.0, upX = 0.0, upY = 1.0, upZ = 0.0;
static float angleX = 0.0, angleY = 0.0;
static int g_width = 700, g_height = 700;

static GLuint sun_tex, moon_tex, earth_tex, mercury_tex, venus_tex, mars_tex,
    jupiter_tex, saturn_tex, uranus_tex, neptune_tex;

static void initLighting() {
  // Lighting set up
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  // Set lighting intensity and color
  GLfloat qaAmbientLight[] = {0.1, 0.1, 0.1, 1.0};
  GLfloat qaDiffuseLight[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat qaSpecularLight[] = {1.0, 1.0, 1.0, 1.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);

  // Set the light position
  GLfloat qaLightPosition[] = {0.0, 0.0, 0.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
}

static void orbit() {
  glColor3f(0.3, 0.3, 0.3);
  glEnable(GL_LINE_SMOOTH);
  int i = 0;
  for (i = 0; i < 8; i++) {
    glPushMatrix();
    if (i == 5) {
      glRotatef(45, 1.0, 0.0, 0.0);
    } else {
      glRotatef(63, 1.0, 0.0, 0.0);
    }
    glScalef(sc[i], sc[i], sc[i]);
    glBegin(GL_LINE_LOOP);
    double ang1 = 0.0;
    int j = 0;
    // creates points along the orbit
    for (j = 0; j < 50; j++) {
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

static void create_sphere(GLdouble radius, GLint slice, GLint stacks,
                          GLuint texIdx) {
  glColor3f(1.0, 1.0, 1.0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texIdx);
  GLUquadricObj *quadric = gluNewQuadric();
  gluQuadricTexture(quadric, true);
  gluQuadricNormals(quadric, GLU_SMOOTH);
  gluSphere(quadric, radius, slice, stacks);
  glDisable(GL_TEXTURE_2D);
}

static void sun(void) {
  push_pop([](void) {
    glScalef(sx, sy, sz);
    create_sphere(1, 50, 50, sun_tex);
  });
}

static void mercury(void) {
  push_pop([](void) {
    glRotatef(angleMercury, 0.0, 1.0, -0.5);
    glTranslatef(0.3, 0.0, 0.0);
    create_sphere(0.016, 50, 50, mercury_tex);
  });
}

static void venus(void) {
  push_pop([](void) {
    glRotatef(angleVenus, 0.0, 1.0, -0.5);
    glTranslatef(0.4, 0.0, 0.0);
    create_sphere(0.02, 50, 50, venus_tex);
  });
}
static void earth(void) {
  push_pop([](void) {
    glRotatef(angleEarth, 0.0, 1.0, -0.5);
    glTranslatef(0.5, 0.0, 0.0);
    create_sphere(0.046, 50, 50, earth_tex);
    // create satelite
    push_pop([](void) {
      glRotatef(angleMoon, 0.0, 0.1, 0.05);
      glTranslatef(0.0598, 0.0, 0.0);
      create_sphere(0.023, 50, 50, moon_tex);
    }); // moon made
  });   // earth made
}
static void mars(void) {
  push_pop([](void) {
    glRotatef(angleMars, 0.0, 1.0, -0.5);
    glTranslatef(-0.6, 0.0, 0.0);
    create_sphere(0.034, 50, 50, mars_tex);
  });
}
static void asteroid(void) {
  push_pop([](void) {
    glColor3f(3.30, 3.30, 3.30);
    glRotatef(63, 1.0, 0.0, 0.0);
    int div = 90;
    float siz = 2;
    float scl[4] = {0.66, 0.68, 0.67, 0.64};
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
}
static void jupiter(void) {
  push_pop([](void) {
    glRotatef(angleJupiter, 0.0, 1.0, -0.5);
    glTranslatef(-0.8, 0.0, 0.0);
    create_sphere(0.1, 50, 50, jupiter_tex);
    push_pop([](void) {
      glRotatef(angleMoon, 1.0, -0.5, 0.0);
      glTranslatef(0.0, 0, 0.11);
      create_sphere(0.005, 50, 50, moon_tex);
    }); // moon made
  });
}
static void saturn(void) {
  push_pop([](void) {
    glRotatef(angleSaturn, 0.0, 1.0, -1.0);
    glTranslatef(-1.0, 0.0, 0.0);

    create_sphere(0.08, 50, 50, saturn_tex);
    push_pop([](void) {
      glRotatef(45, 1.0, 0.0, 0.0);
      glPointSize(3);
      glScalef(0.096, 0.096, 0.096);
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
static void uranus(void) {
  push_pop([](void) {
    glRotatef(angleUranus, 0.0, 1.0, -0.5);
    glTranslatef(1.04, 0.0, 0.0);
    create_sphere(0.046, 50, 50, uranus_tex);
  });
}
static void neptune(void) {
  push_pop([](void) {
    glRotatef(angleNeptune, 0.0, 1.0, -0.5);
    glTranslatef(-1.14, 0.0, 0.0);
    create_sphere(0.04, 50, 50, neptune_tex);
  });
}

static void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  orbit();

  glPushMatrix();
  // test depth ensure ze don't draw object behind eachother
  glEnable(GL_DEPTH_TEST);
  // don't know about this
  glEnable(GL_COLOR_MATERIAL);

  sun();
  mercury();
  venus();
  earth();
  mars();
  jupiter();
  uranus();
  neptune();
  asteroid();

  glPopMatrix();
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

static void specialKey(int key, int, int) {
  const double step = 0.1;
  switch (key) {
  case GLUT_KEY_UP:
    update_angle(angleX, step);
    eyeY = 15.0 * cos(angleX);
    eyeZ = 15.0 * sin(angleX);
    break;
  case GLUT_KEY_DOWN:
    update_angle(angleX, -step);
    eyeY = 15.0 * cos(angleX);
    eyeZ = 15.0 * sin(angleX);
    break;
  case GLUT_KEY_LEFT:
    update_angle(angleY, step);
    eyeX = 15.0 * cos(angleY);
    eyeZ = 15.0 * sin(angleY);
    break;
  case GLUT_KEY_RIGHT:
    update_angle(angleY, -step);
    eyeX = 15.0 * cos(angleY);
    eyeZ = 15.0 * sin(angleY);
  default:
    break;
  }
  glLoadIdentity();
  glViewport(0, 0, g_width, g_height);
  gluPerspective(5.0, (GLdouble)g_width / (GLdouble)g_height, 3.0, 90.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
  glutPostRedisplay();
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
  gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
  g_width = w;
  g_height = h;
}

FIBITMAP *loadImage(const char *filename) {
  FIBITMAP *dib1 = NULL;

  FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename);

  dib1 = FreeImage_Load(fif, filename, JPEG_DEFAULT);
  if (!dib1) {
    exit(1);
  }
  return dib1;
}

GLuint loadTexture(FIBITMAP *dib1) {
  GLuint tex_id = 0;
  int x, y;
  int height, width;

  RGBQUAD rgbquad;

  FREE_IMAGE_TYPE type;
  BITMAPINFOHEADER *header;

  type = FreeImage_GetImageType(dib1);

  height = FreeImage_GetHeight(dib1);
  width = FreeImage_GetWidth(dib1);

  header = FreeImage_GetInfoHeader(dib1);
  int scanLineWidh =
      ((3 * width) % 4 == 0) ? 3 * width : ((3 * width) / 4) * 4 + 4;
  unsigned char *texels =
      (GLubyte *)calloc(height * scanLineWidh, sizeof(GLubyte));
  for (x = 0; x < width; x++)
    for (y = 0; y < height; y++) {
      FreeImage_GetPixelColor(dib1, x, y, &rgbquad);
      texels[(y * scanLineWidh + 3 * x)] = ((GLubyte *)&rgbquad)[2];
      texels[(y * scanLineWidh + 3 * x) + 1] = ((GLubyte *)&rgbquad)[1];
      texels[(y * scanLineWidh + 3 * x) + 2] = ((GLubyte *)&rgbquad)[0];
    }

  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, texels);

  free(texels);

  return tex_id;
}
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Solar System");
  glewInit();
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
  FIBITMAP *sun = loadImage("images/sun.jpg");
  sun_tex = loadTexture(sun);
  FreeImage_Unload(sun);

  initLighting();
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutSpecialFunc(specialKey);
  glutKeyboardFunc(keyPressed);
  glutTimerFunc(25, update, 0);
  glutMainLoop();
  return 0;
}
