#ifndef _H_WORLD_H_
#define _H_WORLD_H_

#include "bsp.h"
#include "entity_src.h"
#include "maindef.h"

int World_load_bsp(const char* map_name);
void World_clear();
void World_draw();
void World_physic_step();
void World_resize_viewport(int width, int height);

typedef struct {
	bool_t hit; // when inside solid, it will be true
	vec3 normal;
	vec3 point;

	bool_t split_claimed; // when hit some plane, it will be true (UNLIKE hit !)
} raycast_result_t;

typedef struct {
	int content_type;
} pointcheck_result_t;

void World_raycast(vec3 from, vec3 to, raycast_result_t* result);
void World_pointcheck(vec3 point, pointcheck_result_t* result);

void Camera_set_position(vec3 v);
void Camera_translate(vec3 v);
void Camera_forward(float step);
void Camera_right(float step);
void Camera_update_yawpitch(float yaw, float pitch);

void Player_forward(float step);
void Player_right(float step);
void Player_jump();

void Worldutil_vec3_to_opengl(vec3 v, vec3 dest);

typedef vec4 worldplane_t;

// as same as quake engine
// leaf contents
#define	CONTENTS_EMPTY -1
#define	CONTENTS_SOLID -2

typedef struct {
	char name[16];

	texture_t texture;
	texture_t texture2; // available when is_sky is true
	
	// preparsed
	bool is_sky;
} world_texture_t;

typedef struct {
	GLint vao, vbo;
	world_texture_t* texture;

	memory_t face_items;
	uint32_t fi_cursor;

} world_mesh_t; // per texture

typedef struct {
	vec3 vertex;
	vec3 normal;
	vec2 uv_texture;
	vec2 uv_lightmap;
} world_vertex_item_t;

// similar to clipnode_t
typedef struct {
	worldplane_t plane;
	int32_t front; // index of clipnodes
	int32_t back; // index of clipnodes
} world_clipnode_t;

typedef struct {
	memory_t meshes;
	int32_t first_node; // index of clipnodes
} world_model_t;

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