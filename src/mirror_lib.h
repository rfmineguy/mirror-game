#ifndef MIRROR_LIB_H
#define MIRROR_LIB_H
#include <stdlib.h>
#include <raylib.h>

typedef struct ray_t ray_t;

typedef struct {
  Vector2 p1, p2;
  Vector2 normal;
} boundary_t;

struct ray_t {
  Vector2 origin, endp, direction;
  float magnitude;
  ray_t *next, *prev;
};

typedef struct {
  ray_t* header;
  ray_t* trailer;
} ray_ll_t;

typedef struct {
  ray_t       ray_source;
  boundary_t* boundaries;
  size_t      boundary_count;
} mirror_lib_setup_t;

ray_ll_t ml_ll_new();
void     ml_ll_free(ray_ll_t);
void     ml_ll_append(ray_ll_t, ray_t*);

void     ml_run(mirror_lib_setup_t*, ray_ll_t*);

ray_t*   ml_new_ray(float ox, float oy, Vector2 direction, float mag);
void     ml_ray_update_xy(ray_t*, int, int);

boundary_t    ml_new_boundary(float, float, float, float);
int           ml_ray_boundary_intersection(ray_t, boundary_t, Vector2*);

Vector2       ml_reflect(Vector2, Vector2);

#endif
