#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <functional>
#ifndef EMSCRIPTEN
#include "textures.hpp"
#endif
#include "sphere.h"

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

static GLfloat sizeSaturnRing = 0.16f;

static GLuint sun_tex, moon_tex, earth_tex, mercury_tex, venus_tex, mars_tex,
    jupiter_tex, saturn_tex, uranus_tex, neptune_tex;

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
#ifndef EMSCRIPTEN
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
#endif
}

static void create_sphere(GLdouble radius, GLint slice, GLint stacks,
                          GLuint texIdx) {
#ifndef EMSCRIPTEN
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texIdx);
#endif
  Sphere s(radius, slice, stacks, true);
  s.draw();
#ifndef EMSCRIPTEN
  glDisable(GL_TEXTURE_2D);
#endif
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

static void planet_moon(GLfloat dist, GLfloat angle, GLfloat distMoon,
                        GLdouble sizeMoon, GLfloat planetMoon, GLuint texMoon) {
  push_pop([&angle, &dist, &distMoon, &sizeMoon, &planetMoon, &texMoon](void) {
    glRotatef(angle, 0.0, 1.0, -0.5);
    glTranslatef(dist, 0.0, 0.0);
    planet(distMoon, sizeMoon, planetMoon, texMoon);
  });
}

static void planet_ring(const GLfloat &dist, const GLfloat &angle,
                        const GLfloat &size) {
#ifndef EMSCRIPTEN
  push_pop([&angle, &dist, &size](void) {
    glRotatef(angle, 0.0, 1.0, -0.5);
    glTranslatef(dist, 0.0, 0.0);
    push_pop([&size](void) {
      glEnable(GL_BLEND);
      glEnable(GL_COLOR_MATERIAL);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
      glScalef(size, size, size);
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
#endif
}

static void update_angle(float &angle, const float &change) {
  angle += change;
  if (angle > 360.0f) {
    angle -= 360.0f;
  }
  if (angle < 0.0f) {
    angle += 360.0f;
  }
}

void update_planets() {
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
}
#ifndef EMSCRIPTEN
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
#endif

void draw_solar() {
  sun();

  planet(distMercury, sizeMercury, angleMercury, mercury_tex);
  planet(distVenus, sizeVenus, angleVenus, venus_tex);

  planet(distEarth, sizeEarth, angleEarth, earth_tex);
  planet_moon(distEarth, angleEarth, distEarthMoon, sizeEarthMoon, angleMoon,
              moon_tex);
  planet(distMars, sizeMars, angleMars, mars_tex);

  planet(distJupiter, sizeJupiter, angleJupiter, jupiter_tex);
  planet_moon(distJupiter, angleJupiter, distJupiterMoon, sizeJupiterMoon,
              angleMoon, moon_tex);

  planet(distSaturn, sizeSaturn, angleSaturn, saturn_tex);
  planet_ring(distSaturn, angleSaturn, sizeSaturnRing);

  planet(distUranus, sizeUranus, angleUranus, uranus_tex);
  planet(distNeptune, sizeNeptune, angleNeptune, neptune_tex);
}

void init_solar() {
#ifndef EMSCRIPTEN
  load_textures();
#endif
}