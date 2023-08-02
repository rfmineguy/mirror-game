#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mirror_lib.h"

// https://en.wikipedia.org/wiki/Line–line_intersection
int main() {
  InitWindow(600, 600, "Mirror Game");
  SetTargetFPS(60);

  boundary_t boundaries[7];
  boundaries[0] = ml_new_boundary(90,  40, 100, 100);
  boundaries[1] = ml_new_boundary(140,  100, 200, 160);
  boundaries[2] = ml_new_boundary(260, 100, 300, 180);
  boundaries[3] = ml_new_boundary(260, 120, 300, 200);
  boundaries[4] = ml_new_boundary(260, 140, 300, 220);
  boundaries[5] = ml_new_boundary(260, 160, 300, 240);
  boundaries[6] = ml_new_boundary(300, 100, 430, 100);
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
    ray_ll_t rays = ml_ll_new();
    struct ray_t* r = ml_new_ray(ray_origin.x, ray_origin.y, Vector2Zero(), 100);
    // update_ray_xy(r, GetMousePosition().x, GetMousePosition().y);
    ml_ray_update_xy(r, GetMousePosition().x, GetMousePosition().y);
    ml_ll_append(rays, r);

    BeginDrawing();
    ClearBackground(GRAY);

    ray_t* temp = rays.header->next;
    int ll_depth = 0;
#define MAX_DEPTH 1
    while (temp && temp != rays.trailer && ll_depth < MAX_DEPTH) {
      int closest_boundary = -1;
      int closest_distance = INT_MAX;
      Vector2 closest_point;
      int hit = 0;
      for (int i = 0; i < 7; i++) {
        DrawLineEx(boundaries[i].p1, boundaries[i].p2, 5, BLUE);
        DrawLineEx(boundaries[i].p1, boundaries[i].normal, 2, YELLOW);
        Vector2 point;
        if (ml_ray_boundary_intersection(*temp, boundaries[i], &point)) {
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
        // Vector2 boundary_normal_p = Vector2Add(closest_point, boundary_normal);
        // DrawCircleV(closest_point, 5, RED);
        // DrawLineEx(closest_point, boundary_normal_p, 5, GREEN);

        int dist = sqrt((temp->origin.x - closest_point.x) * (temp->origin.x - closest_point.x) + (temp->origin.y - closest_point.y) * (temp->origin.y - closest_point.y));
        printf("%d\n", dist);
        // Calculating the reflection
        //    I - 2 * (N.I) * N
        Vector2 I = temp->direction;
        Vector2 N = boundary_normal;
        Vector2 I_mid_N = ml_reflect(I, N);
        Vector2 reflect = Vector2Add(I_mid_N, closest_point);

        // Generate new ray
        ray_t* r = ml_new_ray(closest_point.x, closest_point.y, reflect, dist);
        ml_ray_update_xy(r, reflect.x, reflect.y);
        ml_ll_append(rays, r);
        ml_ray_update_xy(temp, closest_point.x, closest_point.y);
        temp = temp->next;
        continue;
      }
      temp = temp->next;
      ll_depth ++;
    }
    temp = rays.header->next;
    while (temp && temp != rays.trailer) {
      DrawLineEx(temp->origin, temp->endp, 2, PINK);
      temp = temp->next;
    }
    EndDrawing();

    ml_ll_free(rays);
  }
}
