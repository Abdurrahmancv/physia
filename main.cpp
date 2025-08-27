struct Vec3 {
	float x{}, y{}, z{};

	Vec3() = default;
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vec3 operator +(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
	Vec3 operator +=(const Vec3& o) { x = x + o.x; y = y + o.y; z = z + o.z; return{}; }
	Vec3 operator -(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
	Vec3 operator -=(const Vec3& o) { x = x - o.x; y = y - o.y; z = z - o.z; return{}; }
	Vec3 operator *(float s) const { return { x * s, y * s, z * s }; }
};

struct Entity{
	Vec3 transform;
	Vec3 velocity;
	Vec3 box;
	bool isStatic;
};



#include <vector>
#include <cmath>

// Daha basit bir yolu olmalı?
bool AABB_overlap(const Vec3& A, const Vec3& a, const Vec3& B, const Vec3& b) {
	if(std::abs(A.x - B.x) > (a.x + b.x) / 2) return false; // abs alınan: pos | ikiye bölünen: half
	if(std::abs(A.y - B.y) > (a.y + b.y) / 2) return false;
	if(std::abs(A.z - B.z) > (a.z + b.z) / 2) return false;
	return true;
}

Vec3 compute_mtv(const Vec3& A, const Vec3& a, const Vec3& B, const Vec3& b) {
	float dx = (a.x + b.x) - std::abs(A.x - B.x); // ab half, AB pos
	float dy = (a.y + b.y) - std::abs(A.y - B.y);
	float dz = (a.z + b.z) - std::abs(A.z - B.z);

	if (dx <= dy && dx <= dz) {
		float sign = (A.x < B.x) ? -1.f : 1.f; return { sign * dx, 0, 0 };
	}
	else if (dy <= dx && dy <= dz) {
		float sign = (A.y < B.y) ? -1.f : 1.f; return { 0, sign * dy, 0 };
	}
	else {
		float sign = (A.z < B.z) ? -1.f : 1.f; return { 0, 0, sign * dz };
	}
}

void physics_step(std::vector<Entity>& entities, float dT) {
	int n = (int)entities.size();

	std::vector<Vec3> tentative(n); // Position olarak.

	for (int i = 0; i < n; i++) {
		tentative[i] = entities[i].transform;

		if (entities[i].isStatic) continue;

		tentative[i] += entities[i].velocity * dT;
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (AABB_overlap(tentative[i], entities[i].box, tentative[j], entities[j].box)) {
				Vec3 mtv = compute_mtv(tentative[i], entities[i].box, tentative[j], entities[j].box);

				bool si = entities[i].isStatic;
				bool sj = entities[j].isStatic;

				if (si && sj) {
					tentative[i] = entities[i].transform; tentative[j] = entities[j].transform;
				}
				else if (si && !sj) {
					tentative[j] += mtv * -1.f; // Burada niye -1.f ile çarpıyoruz?

					if (mtv.x != 0) entities[j].velocity.x = 0;
					if (mtv.y != 0) entities[j].velocity.y = 0;
					if (mtv.z != 0) entities[j].velocity.z = 0;
				}
				else if (!si && si) {
					tentative[i] += mtv; // Ve burada çarpmıyoruz?

					if (mtv.x != 0) entities[i].velocity.x = 0;
					if (mtv.y != 0) entities[i].velocity.y = 0;
					if (mtv.z != 0) entities[i].velocity.z = 0;
				}
				else {
					Vec3 half_mtv = mtv * 0.5f;

					tentative[i] += half_mtv;
					tentative[j] -= half_mtv;

					if (mtv.x != 0) std::swap(entities[i].velocity.x, entities[j].velocity.x);
					if (mtv.y != 0) std::swap(entities[i].velocity.y, entities[j].velocity.y);
					if (mtv.z != 0) std::swap(entities[i].velocity.z, entities[j].velocity.z);
				}
			}
		}
	}

	for (int i = 0; i < n; i++) entities[i].transform = tentative[i];
}



#include "raylib.h" // Buradan itibaren raylib kütüphanesi kullanılıyor.
#include "systems.hpp"

int main() {
	InitWindow(800, 450, "Minimal ECS Physics");
	SetTargetFPS(120);

	Camera cam = {};
	cam.position = {0.f, 10.f, 15.f};
	cam.target = {0.f, 0.f, 0.f};
	cam.up = {0.f, 1.f, 0.f};
	cam.fovy = 45.f;
	cam.projection = CAMERA_PERSPECTIVE;

	std::vector<Entity> entities;

	// Static Cubes
	Entity cube1 = { {0,0,0}, {0,0,0}, {1,1,1}, true };
	entities.push_back(cube1);
	// Dynamic Cubes
	entities.push_back({ {-5,0,0}, {3,0,0}, {1,1,1}, false });
	entities.push_back({ {5,0,2}, {-2.5f,0,-0.5f}, {1,1,1}, false});

	// Raylib Loop
	while (!WindowShouldClose()) {
		float dT = GetFrameTime(); // deltaTime'ı her dönüşte frame defasına eşitliyor.
		physics_step(entities, dT);

		BeginDrawing();
		ClearBackground(RAYWHITE);

		BeginMode3D(cam);
		DrawGrid(20, 1.f);

		for (auto& e : entities) { // Manası ne?
			Color c = e.isStatic ? BLUE : RED;

			DrawCube(
				{ e.transform.x, e.transform.y, e.transform.z },
				e.box.x, e.box.y, e.box.z, c);
		}

		EndMode3D();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
