#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
  Vector2 p1, p2;
  Vector2 normal;
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

// #define new_boundary(x1, y1, x2, y2) (boundary_t) { .p1 = (Vector2){.x = x1, .y = y1 }, .p2 = (Vector2){.x = x2, .y = y2} }

boundary_t new_boundary(float x1, float y1, float x2, float y2) {
  boundary_t b = {0};
  b.p1.x = x1;
  b.p1.y = y1;
  b.p2.x = x2;
  b.p2.y = y2;
  float dx = b.p2.x - b.p1.x;
  float dy = b.p2.y - b.p1.y;
  b.normal = (Vector2) {.x=x1 + dy / 4.f, .y=y1 - dx / 4.f};
  return b;
}

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

void update_ray_direction(ray_t* ray, Vector2 dir) {
  ray->direction = dir;
  float mag = sqrt(ray->direction.x * ray->direction.x + ray->direction.y * ray->direction.y);
  ray->direction.x /= mag;
  ray->direction.y /= mag;
}

void update_ray_xy(ray_t* ray, int x, int y) {
  ray->endp.x = x;
  ray->endp.y = y;
  ray->direction = (Vector2) {.x=ray->origin.x - x, .y=ray->origin.y - y};
  float mag = sqrt(ray->direction.x * ray->direction.x + ray->direction.y * ray->direction.y);
  ray->direction.x /= mag;
  ray->direction.y /= mag;
}

ray_t* new_ray(float ox, float oy, Vector2 direction, float mag) {
  ray_t* r = malloc(sizeof(ray_t));
  r->origin.x = ox;
  r->origin.y = oy;
  r->magnitude = mag;
  r->next = NULL;
  //update_ray(r, angle);
  r->closest_hit_distance = INT_MAX;
  return r;
}

// https://en.wikipedia.org/wiki/Line–line_intersection
int main() {
  InitWindow(600, 600, "Mirror Game");
  SetTargetFPS(60);

  boundary_t boundaries[4];
  boundaries[0] = new_boundary(90,  40, 100, 100);
  boundaries[1] = new_boundary(140,  100, 200, 160);
  boundaries[2] = new_boundary(260, 100, 300, 180);
  boundaries[3] = new_boundary(300, 100, 430, 100);
  Vector2 ray_origin = {0};

  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_S)) {
      ray_origin.y ++;
    }
    if (IsKeyDown(KEY_W)) {
      ray_origin.y --;
    }
    if (IsKeyDown(KEY_D)) {
      ray_origin.x ++;
    }
    if (IsKeyDown(KEY_A)) {
      ray_origin.x --;
    }
    ll_t rays = ll_new();
    ray_t* r = new_ray(ray_origin.x, ray_origin.y, Vector2Zero(), 300);
    // update_ray_xy(r, GetMousePosition().x, GetMousePosition().y);
    update_ray_xy(r, GetMousePosition().x, GetMousePosition().y);
    ll_append(rays, r);

    BeginDrawing();
    ClearBackground(GRAY);

    ray_t* temp = rays.header->next;
    int ll_depth = 0;
    while (temp && temp != rays.trailer) {
      int closest_boundary = -1;
      int closest_distance = INT_MAX;
      Vector2 closest_point;
      int hit = 0;
      for (int i = 0; i < 4; i++) {
        DrawLineEx(boundaries[i].p1, boundaries[i].p2, 5, BLUE);
        DrawLineEx(boundaries[i].p1, boundaries[i].normal, 2, YELLOW);
        Vector2 point;
        if (ray_boundary_intersection(*temp, boundaries[i], &point)) {
          int dist_sq = (temp->origin.x - point.x) * (temp->origin.x - point.x) + (temp->origin.y - point.y) * (temp->origin.y - point.y);
          if (dist_sq > 100 && dist_sq < closest_distance) {
            closest_distance = dist_sq;
            closest_boundary = i;
            closest_point = point;
            hit = 1;
          }
        }
      }
      if (hit) {
        boundary_t boundary = boundaries[closest_boundary];
        float dx = boundary.p2.x - boundary.p1.x;
        float dy = boundary.p2.y - boundary.p1.y;
        float m = dy / dx;
        float b = boundary.p1.y - m * boundary.p1.x;
        // y = m*x + b
        Vector2 boundary_normal;// = (Vector2) {.x = dy, .y = -dx};
        if (temp->origin.y < m * temp->origin.x + b) {
          boundary_normal = (Vector2) {.x = dy, .y = -dx};
        }
        else {
          boundary_normal = (Vector2) {.x = -dy, .y = dx};
        }

        // Drawing the normal
        Vector2 boundary_normal_p = Vector2Add(closest_point, boundary_normal);
        DrawCircleV(closest_point, 5, RED);
        DrawLineEx(closest_point, boundary_normal_p, 5, GREEN);

        // Calculating the reflection
        //  I - 2 * (N.I) * N
        Vector2 I = temp->direction;
        Vector2 N = boundary_normal;
        N = Vector2Normalize(N);
        float IdotN     = Vector2DotProduct(I, N);
        float mid       = (IdotN / 1) * 2;
        Vector2 mid_N   = Vector2Scale(N, mid);
        Vector2 I_mid_N = Vector2Subtract(I, mid_N);
        I_mid_N         = Vector2Normalize(I_mid_N);
        I_mid_N         = Vector2Scale(I_mid_N, -300);
        Vector2 reflect = Vector2Add(I_mid_N, closest_point);

        // Use reflection to generate new ray
        DrawLineEx(closest_point, reflect, 2, MAGENTA);
        ray_t* r = new_ray(closest_point.x, closest_point.y, reflect, 300);
        update_ray_xy(r, reflect.x, reflect.y);
        ll_append(rays, r);
        temp = temp->next;
        continue;
      }
      temp = temp->next;
      // printf("%p\n", temp);
      ll_depth ++;
    }
    temp = rays.header->next;
    while (temp && temp != rays.trailer) {
      DrawLineEx(temp->origin, temp->endp, 2, PINK);
      temp = temp->next;
    }
    EndDrawing();

    ll_free(rays);
  }
}
