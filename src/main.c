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
  setup.boundaries[0] = ml_new_boundary(1, 0, 1, 600, REFLECT_OFF, MOVABLE);
  setup.boundaries[1] = ml_new_boundary(1, 550, 550, 550, REFLECT_OFF, MOVABLE);
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
        ml_save_setup("mlsave.msave", &setup);
        printf("Saved boundary layout\n");
      }
      if (IsKeyPressed(KEY_L)) {
        if (!ml_load_setup("mlsave.msave", &setup)) {
          fprintf(stderr, "Failed to open mlsave.msave\n");
        }
        printf("Loaded boundary layout\n");
      }
      if (IsKeyPressed(KEY_N)) {
        if (setup.boundary_count + 1 < 7) {
          setup.boundary_count ++;
          setup.boundaries[setup.boundary_count - 1] = ml_new_boundary(300, 200, 300, 400, REFLECT_OFF, MOVABLE);
        }
        else {
          fprintf(stderr, "Failed to create boundary, max number reached: 7\n");
        }
      }
    }

    // Control intructions
    {
      int width = MeasureText("WASD: Move source", 20);
      DrawText("WASD: Move source", 600 - width, 40, 20, WHITE);

      width = MeasureText("Mouse: Change direction", 20);
      DrawText("Mouse: Change direction", 600 - width, 70, 20, WHITE);

      width = MeasureText("N: Spawn boundary", 20);
      DrawText("N: Spawn boundary", 600 - width, 100, 20, WHITE);

      width = MeasureText("M: Save boundary layout (mlsave.msave)", 20);
      DrawText("M: Save boundary layout (mlsave.msave)", 600 - width, 130, 20, WHITE);

      width = MeasureText(",: Load boundary layout (mlsave.msave)", 20);
      DrawText("L: Load boundary layout (mlsave.msave)", 600 - width, 160, 20, WHITE);
    }

    // Create the ray linked list
    //   This gets constructed and destroyed every frame (maybe not ideal?)
    rays = ml_ll_new();

    // Setup initial ray for the simulation
    {
      ray_t* r = ml_new_ray(ray_origin.x, ray_origin.y, Vector2Zero(), 100);
      ml_ray_update_xy(r, GetMousePosition().x, GetMousePosition().y);
      ml_ll_append(&rays, r);
    }

    // Run the simulation
    ml_run(&setup, &rays);

    // Draw the 'game'
    {
      BeginDrawing();
        ClearBackground(GRAY);
        ml_show(&setup, &rays);
        DrawText(TextFormat("# of rays: %zu", rays.size), 400, 10, 20, WHITE);
      EndDrawing();
    }

    // Free the ray linked list
    ml_ll_free(rays);

    // Run "win" logic
    {
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
  }

  rays = (ray_ll_t){0};
}
