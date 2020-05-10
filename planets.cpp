#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <map>
#include <functional>
#include "textures.hpp"

struct planet {
  GLfloat dist;
  GLdouble size;
  GLfloat angle;
  GLfloat angleStep;
  GLuint tex;
};

static std::map<const std::string, struct planet> planets{
    {"sun", {0.0f, 0.2, 0.0f, 0}},
    {"mercury", {0.295f, 0.016, 0.0, 2.0f, 0}},
    {"venus", {0.4f, 0.02, 0.0f, 0.9f, 0}},
    {"earth", {0.5f, 0.046, 0.0f, 0.7f, 0}},
    {"earthMoon", {0.0598f, 0.023, 0.0f, 2.0f, 0}},
    {"mars", {0.6f, 0.034, 0.0f, 0.5f, 0}},
    {"jupiter", {0.8f, 0.1, 0.0f, 0.2f, 0}},
    {"jupiterMoon", {0.11f, 0.005, 0.0f, 0.2f, 0}},
    {"saturn", {1.0f, 0.08, 0.0f, 0.1f, 0}},
    {"uranus", {1.05f, 0.046, 0.0f, 0.05f, 0}},
    {"neptune", {1.13f, 0.04, 0.0f, 0.02f, 0}}};

static GLfloat sizeSaturnRing = 0.16f;
static GLfloat angleAsteroid = 0.0f;

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

static void planet(const struct planet &p) {
  orbit(p.dist);
  push_pop([&p](void) {
    glRotatef(p.angle, 0.0, 1.0, -0.5);
    glTranslatef(p.dist, 0.0, 0.0);
    create_sphere(p.size, 50, 50, p.tex);
  });
}

static void planet_moon(const struct planet &p, const struct planet &moon) {
  push_pop([&p, &moon](void) {
    glRotatef(p.angle, 0.0, 1.0, -0.5);
    glTranslatef(p.dist, 0.0, 0.0);
    planet(moon);
  });
}

static void planet_ring(const struct planet &p, GLfloat &sizeRing) {
  push_pop([&p, &sizeRing](void) {
    glRotatef(p.angle, 0.0, 1.0, -0.5);
    glTranslatef(p.dist, 0.0, 0.0);
    push_pop([&p, &sizeRing](void) {
      glEnable(GL_BLEND);
      glEnable(GL_COLOR_MATERIAL);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
      glScalef(sizeRing, sizeRing, sizeRing);
      glBegin(GL_TRIANGLE_FAN);
      const float alpha = 0.2f;
      // todo make a better way to set alpha
      glColor4f(0.5f, 0.5f, 0.5f, alpha);
      double ang1 = 0.0;
      static double step = 2 * M_PI / 50;
      for (int i = 0; i < 50; i++) {
        glVertex2d(cos(ang1), sin(ang1));
        ang1 += step;
      }
      glEnd();
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_BLEND);
    });
  });
}

static void update_angle(std::pair<const std::string, struct planet> &p) {
  p.second.angle += p.second.angleStep;
  if (p.second.angle > 360.0f) {
    p.second.angle -= 360.0f;
  }
  if (p.second.angle < 0.0f) {
    p.second.angle += 360.0f;
  }
}

static void update_planets() {
  std::for_each(planets.begin(), planets.end(), update_angle);

  angleAsteroid += 0.002f;
  if (angleAsteroid > 360.0f) {
    angleAsteroid -= 360.0f;
  }
  if (angleAsteroid < 0.0f) {
    angleAsteroid += 360.0f;
  }
}

static void load_textures() {
  load_texture("images/sun.jpg", &planets["sun"].tex);

  load_texture("images/mercury.jpg", &planets["mercury"].tex);
  load_texture("images/venus.jpg", &planets["venus"].tex);
  load_texture("images/earth.jpg", &planets["earth"].tex);
  load_texture("images/moon.jpg", &planets["earthMoon"].tex);
  load_texture("images/mars.jpg", &planets["mars"].tex);
  load_texture("images/jupiter.jpg", &planets["jupiter"].tex);
  load_texture("images/moon.jpg", &planets["jupiterMoon"].tex);
  load_texture("images/saturn.jpg", &planets["saturn"].tex);
  load_texture("images/uranus.jpg", &planets["uranus"].tex);
  load_texture("images/neptune.jpg", &planets["neptune"].tex);
}

void draw_solar() {

  planet(planets["sun"]);

  planet(planets["mercury"]);
  planet(planets["venus"]);

  planet(planets["earth"]);
  planet_moon(planets["earth"], planets["earthMoon"]);

  planet(planets["mars"]);

  planet(planets["jupiter"]);
  planet_moon(planets["jupiter"], planets["jupiterMoon"]);

  planet(planets["saturn"]);
  planet_ring(planets["saturn"], sizeSaturnRing);

  planet(planets["uranus"]);
  planet(planets["neptune"]);
}

void init_solar() { load_textures(); }
void update_solar() { update_planets(); }