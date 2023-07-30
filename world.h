#ifndef _H_WORLD_H_
#define _H_WORLD_H_

#include "bsp.h"
#include "maindef.h"

int World_load_bsp(const char* map_name);
void World_clear();
void World_draw();
void World_resize_viewport(int width, int height);

void Camera_set_position(vec3 v);
void Camera_translate(vec3 v);
void Camera_forward(float step);
void Camera_right(float step);
void Camera_update_yawpitch(float yaw, float pitch);

void Worldutil_vec3_to_opengl(vec3 v, vec3 dest);

typedef struct {
	char name[16];
	texture_t texture;
} world_texture_t;

typedef struct {
	GLint vao, vbo;
	world_texture_t* texture;

	memory_t vertex_items;
	uint32_t vi_cursor;

} world_mesh_t; // per texture

typedef struct {
	memory_t meshes;
} world_model_t;

typedef struct {
	vec3 vertex;
	vec3 normal;
	vec2 uv_texture;
	vec2 uv_lightmap;
} world_vertex_item_t;

////////////

struct camera
{
	float fov;
	float aspect;
	float near;
	float far;

	vec3 position;
	vec3 rotation;
	mat4 projection;
};

#endif