#include "raylib.h"
#include <vector>

enum class BodyType { Static, Dynamic };

struct TransformComponent {
    Vector3 position;
    Vector3 size;
};

struct PhysicsComponent {
    Vector3 velocity;
    BodyType type;
};

struct Entity {
    TransformComponent transform;
    PhysicsComponent physics;
};

bool CheckCollision(const TransformComponent& a, const TransformComponent& b) {
    return (fabs(a.position.x - b.position.x) <= (a.size.x / 2 + b.size.x / 2) &&
        fabs(a.position.y - b.position.y) <= (a.size.y / 2 + b.size.y / 2) &&
        fabs(a.position.z - b.position.z) <= (a.size.z / 2 + b.size.z / 2));
}

void PhysicsSystem(std::vector<Entity>& entities, float dt) {
    for (auto& e : entities) {
        if (e.physics.type == BodyType::Dynamic) {
            e.transform.position.x += e.physics.velocity.x * dt;
            e.transform.position.y += e.physics.velocity.y * dt;
            e.transform.position.z += e.physics.velocity.z * dt;
        }
    }
}

void CollisionSystem(std::vector<Entity>& entities) {
    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            if (CheckCollision(entities[i].transform, entities[j].transform)) {
                if (entities[i].physics.type == BodyType::Dynamic)
                    entities[i].physics.velocity = { 0, 0, 0 };
                if (entities[j].physics.type == BodyType::Dynamic)
                    entities[j].physics.velocity = { 0, 0, 0 };
            }
        }
    }
}

int main() {
    InitWindow(800, 600, "Minimal ECS Physics");
    Camera3D camera = { 0 };
    camera.position = { 5.0f, 5.0f, 5.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    std::vector<Entity> entities;

    // Dynamic küp
    entities.push_back({ {{-2, 0, 0}, {1, 1, 1}}, {{1, 0, 0}, BodyType::Dynamic} });
    // Static küp
    entities.push_back({ {{2, 0, 0}, {1, 1, 1}}, {{0, 0, 0}, BodyType::Static} });

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        PhysicsSystem(entities, dt);
        CollisionSystem(entities);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        for (auto& e : entities) {
            DrawCube(e.transform.position, e.transform.size.x, e.transform.size.y, e.transform.size.z, RED);
            DrawCubeWires(e.transform.position, e.transform.size.x, e.transform.size.y, e.transform.size.z, BLACK);
        }

        EndMode3D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
