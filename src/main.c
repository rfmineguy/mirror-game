#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
  Vector2 p1, p2;
} boundary_t;

typedef struct ray_t {
  Vector2 origin, endp, direction;
  float angle;
  float magnitude;
  float closest_hit_distance;
  struct ray_t *next, *prev;
} ray_t;

typedef struct {
  ray_t* header;
  ray_t* trailer;
} ll_t;

#define new_boundary(x1, y1, x2, y2) (boundary_t) { .p1 = (Vector2){.x = x1, .y = y1 }, .p2 = (Vector2){.x = x2, .y = y2} }

ll_t ll_new() {
  ll_t ll = {0};
  ll.header = calloc(1, sizeof(ray_t));
  ll.trailer = calloc(1, sizeof(ray_t));
  ll.header->next = ll.trailer;
  ll.trailer->prev = ll.header;
  return ll;
}

void ll_free(ll_t ll) {
  ray_t* t = ll.header;
  while (t) {
    ray_t* temp = t->next;
    free(t);
    t = temp;
    }
}

void ll_append(ll_t ll, ray_t* ray) {
  ray_t* prev = ll.trailer->prev;
  prev->next = ray;
  ll.trailer->prev = ray;
}

int ray_boundary_intersection(ray_t ray, boundary_t b, Vector2* p_out) {
  float x1 = ray.origin.x, y1 = ray.origin.y;
  float x2 = ray.endp.x,   y2 = ray.endp.y;
  float x3 = b.p1.x,       y3 = b.p1.y;
  float x4 = b.p2.x,       y4 = b.p2.y;

  float t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
  float t_den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  float t = t_num / t_den;

  float u_num = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
  float u_den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  float u = u_num / u_den;

  float x = x1 + t * (x2 - x1);
  float y = y1 + t * (y2 - y1);
  if (p_out)
    *p_out = (Vector2) {.x=x,.y=y};
  return t >= 0 && t <= 1 && u >= 0 && u <= 1;
}

void update_ray(ray_t* ray, float angle) {
  ray->angle = angle;
  ray->endp.x = ray->origin.x + cos(angle*PI/180.0) * ray->magnitude;
  ray->endp.y = ray->origin.y + sin(angle*PI/180.0) * ray->magnitude;
}

void update_ray_xy(ray_t* ray, int x, int y) {
  ray->endp.x = x;
  ray->endp.y = y;
  ray->direction = (Vector2) {.x=ray->origin.x - x, .y=ray->origin.y - y};
  float mag = sqrt(ray->direction.x * ray->direction.x + ray->direction.y * ray->direction.y);
  ray->direction.x /= mag;
  ray->direction.y /= mag;
}

ray_t* new_ray(float ox, float oy, float angle, float mag) {
  ray_t* r = malloc(sizeof(ray_t));
  r->origin.x = ox;
  r->origin.y = oy;
  r->angle = angle;
  r->magnitude = mag;
  r->next = NULL;
  update_ray(r, angle);
  r->closest_hit_distance = INT_MAX;
  return r;
}

// https://en.wikipedia.org/wiki/Line–line_intersection
int main() {
  InitWindow(600, 600, "Mirror Game");
  SetTargetFPS(60);

  boundary_t boundaries[4];
  boundaries[0] = new_boundary(40,  100, 100, 40);
  boundaries[1] = new_boundary(40,  100, 100, 160);
  boundaries[2] = new_boundary(100, 100, 100, 180);
  boundaries[3] = new_boundary(100, 100, 130, 100);

  while (!WindowShouldClose()) {
    ll_t rays = ll_new();
    ray_t* r = new_ray(300, 300, 0.0f, 300);
    update_ray_xy(r, GetMousePosition().x, GetMousePosition().y);
    ll_append(rays, r);

    BeginDrawing();
    ClearBackground(GRAY);

    ray_t* temp = rays.header->next;
    while (temp) {
      int closest_boundary = -1;
      int closest_distance = INT_MAX;
      Vector2 closest_point;
      int hit = 0;
      for (int i = 0; i < 4; i++) {
        DrawLineEx(boundaries[i].p1, boundaries[i].p2, 5, BLUE);
        Vector2 point;
        if (ray_boundary_intersection(*temp, boundaries[i], &point)) {
          int dist_sq = (temp->origin.x - point.x) * (temp->origin.x - point.x) + (temp->origin.y - point.y) * (temp->origin.y - point.y);
          if (dist_sq < closest_distance) {
            closest_distance = dist_sq;
            closest_boundary = i;
            closest_point = point;
            hit = 1;
          }
        }
      }
      if (hit)
        DrawCircleV(closest_point, 5, RED);
      temp = temp->next;
    }
    temp = rays.header->next;
    while (temp && temp != rays.trailer) {
      DrawLineV(temp->origin, temp->endp, PINK);
      temp = temp->next;
    }
    EndDrawing();

    ll_free(rays);
  }
}
