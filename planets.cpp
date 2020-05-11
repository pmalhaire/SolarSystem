#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <map>
#include <functional>
#include "textures.hpp"
#include "sphere.h"

struct position {
  GLdouble x, y, z;
};

struct planet {
  GLdouble dist;
  GLdouble size;
  GLdouble angleStep;
  GLuint tex;
  struct position pos;
};

static int step = 0;

// set orbital speed using https://www.sjsu.edu/faculty/watkins/orbital.htm
const double baseSpeed = 2.0 * M_PI / 360.0;

static std::map<const std::string, struct planet> planets{
    {"sun", {0.0, 0.2, 0.0, 0}},
    {"mercury", {0.295, 0.016, baseSpeed * 1.607, 0}},
    {"venus", {0.4, 0.02, baseSpeed * 1.174, 0}},
    {"earth", {0.5, 0.046, baseSpeed, 0}},
    {"earthMoon", {0.0598, 0.023, 0.20, 0}},
    {"mars", {0.6, 0.034, baseSpeed * 0.802, 0}},
    {"jupiter", {0.8, 0.1, baseSpeed * 0.434, 0}},
    {"jupiterMoon", {0.11, 0.005, 0.2, 0}},
    {"saturn", {1.0, 0.08, baseSpeed * 0.323, 0}},
    {"uranus", {1.05, 0.046, baseSpeed * 0.228, 0}},
    {"neptune", {1.13, 0.04, baseSpeed * 0.182, 0}}};

static GLfloat sizeSaturnRing = 0.14f;

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
    glScalef(scale, scale, scale);
    glBegin(GL_LINE_LOOP);

    static const int points = 100;
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
  Sphere s(radius, slice, stacks, true);
  s.draw();
  glDisable(GL_TEXTURE_2D);
}

static void planet(const struct planet &p) {
  orbit(p.dist);
  push_pop([&p](void) {
    glTranslated(p.pos.x, p.pos.y, p.pos.z);
    create_sphere(p.size, 50, 50, p.tex);
  });
}

static void planet_moon(const struct planet &p, const struct planet &moon) {
  push_pop([&p, &moon](void) {
    glTranslated(p.pos.x, p.pos.y, p.pos.z);
    planet(moon);
  });
}

static void planet_ring(const struct planet &p, GLfloat &sizeRing) {
  push_pop([&p, &sizeRing](void) {
    glTranslated(p.pos.x, p.pos.y, p.pos.z);
    push_pop([&p, &sizeRing](void) {
      glEnable(GL_BLEND);
      glEnable(GL_COLOR_MATERIAL);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

static void update_pos(std::pair<const std::string, struct planet> &p) {
  p.second.pos = {p.second.dist * cos(step * p.second.angleStep),
                  p.second.dist * sin(step * p.second.angleStep), 0.0};
}

static void update_planets() {
  ++step;
  if (step > INT32_MAX) {
    step = 0;
  }
  std::for_each(planets.begin(), planets.end(), update_pos);
}

static void
load_planet_texture(std::pair<const std::string, struct planet> &p) {
  load_texture("images/" + p.first + ".jpg", &p.second.tex);
}

static void load_textures() {

  std::for_each(planets.begin(), planets.end(), load_planet_texture);
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