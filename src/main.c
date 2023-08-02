#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mirror_lib.h"

// https://en.wikipedia.org/wiki/Line–line_intersection
int main() {
  Vector2 ray_origin = {0};
  mirror_lib_setup_t setup = {0};
  setup.boundaries = malloc(7 * sizeof(boundary_t));
  setup.boundary_count = 4;
  setup.boundaries[0] = ml_new_boundary(1, 0, 1, 600, REFLECT, MOVABLE);
  setup.boundaries[1] = ml_new_boundary(1, 550, 550, 550, REFLECT, MOVABLE);
  setup.boundaries[2] = ml_new_boundary(200, 200, 400, 400, ABSORB, STATIC);
  setup.boundaries[3] = ml_new_boundary(100, 200, 200, 200, ABSORB, STATIC);

  // ray_ll_t rays_2 = ml_ll_new();
  // ml_ll_append(rays_2, ml_new_ray(0, 0, Vector2Zero(), 100));
  // ml_ll_free(rays_2);

  InitWindow(600, 600, "Mirror Game");
  SetTargetFPS(60);

  ray_ll_t rays;
  while (!WindowShouldClose()) {
    ml_boundary_edit(&setup);

    // Keyboard input
    {
      if (IsKeyDown(KEY_S)) ray_origin.y ++;
      if (IsKeyDown(KEY_W)) ray_origin.y --;
      if (IsKeyDown(KEY_D)) ray_origin.x ++;
      if (IsKeyDown(KEY_A)) ray_origin.x --;
      if (IsKeyPressed(KEY_M)) {
        printf("Saving boundary layout\n");
      }
    }

    // Setup initial ray for the simulation
    rays = ml_ll_new();

    ray_t* r = ml_new_ray(ray_origin.x, ray_origin.y, Vector2Zero(), 100);
    ml_ray_update_xy(r, GetMousePosition().x, GetMousePosition().y);
    ml_ll_append(&rays, r);

    ml_run(&setup, &rays);

    BeginDrawing();
    ClearBackground(GRAY);

    // Draw the resulting rays
    ray_t* temp = rays.header->next;
    while (temp && temp != rays.trailer) {
      DrawLineEx(temp->origin, temp->endp, 2, PINK);
      temp = temp->next;
    }
    printf("%zu\n", rays.size);
    DrawText(TextFormat("# of rays: %zu", rays.size), 400, 10, 20, WHITE);

    EndDrawing();

    ml_ll_free(rays);

    if (setup.boundaries[1].was_hit) {
      int width = MeasureText("You win!", 30);
      DrawText("You win!", (600 / 2) - (width / 2), 20, 30, BLUE);
      setup.boundaries[1].was_hit = 0;
    }
    else {
      int width = MeasureText("You lose!", 30);
      DrawText("You lose!", (600 / 2) - (width / 2), 20, 30, BLUE);
    }
  }
  ml_ll_free(rays);
}
