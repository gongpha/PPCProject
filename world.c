#include "world.h"

// should work on Quake 1 and GoldSRC (halflife1) maps

char world_loaded_world[256]; // if the first character is not 0, then the world is loaded.

MEMSTATIC(world_entities_src);

MEMSTATIC(world_textures);
MEMSTATIC(world_models);

MEMSTATIC(world_clipnodes);

shader_t world_shader;
texture_t lightmap_texture;

#include "glad.h"
#include <GLFW/glfw3.h>

#include "shaders.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "ext/stb_rect_pack.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ext/stb_image_write.h"

mat4 world_projection;
mat4 world_view;
vec3 camera_pos;
vec3 camera_front;
vec3 camera_up;
vec3 player_velocity;
vec2 player_plane_moveaxis;
vec3 player_wishdir;
bool_t player_is_on_floor;
bool_t player_queue_jump;

#define MAX_CHECK 8
const vec3 quake_pal[] = {
	{0.000000,0.000000,0.000000},{0.058824,0.058824,0.058824},{0.121569,0.121569,0.121569},{0.184314,0.184314,0.184314},{0.247059,0.247059,0.247059},{0.294118,0.294118,0.294118},{0.356863,0.356863,0.356863},{0.419608,0.419608,0.419608},{0.482353,0.482353,0.482353},{0.545098,0.545098,0.545098},{0.607843,0.607843,0.607843},{0.670588,0.670588,0.670588},{0.733333,0.733333,0.733333},{0.796078,0.796078,0.796078},{0.858824,0.858824,0.858824},{0.921569,0.921569,0.921569},{0.058824,0.043137,0.027451},{0.090196,0.058824,0.043137},{0.121569,0.090196,0.043137},{0.152941,0.105882,0.058824},{0.184314,0.137255,0.074510},{0.215686,0.168627,0.090196},{0.247059,0.184314,0.090196},{0.294118,0.215686,0.105882},{0.325490,0.231373,0.105882},{0.356863,0.262745,0.121569},{0.388235,0.294118,0.121569},{0.419608,0.325490,0.121569},{0.450980,0.341176,0.121569},{0.482353,0.372549,0.137255},{0.513726,0.403922,0.137255},{0.560784,0.435294,0.137255},{0.043137,0.043137,0.058824},{0.074510,0.074510,0.105882},{0.105882,0.105882,0.152941},{0.152941,0.152941,0.200000},{0.184314,0.184314,0.247059},{0.215686,0.215686,0.294118},{0.247059,0.247059,0.341176},{0.278431,0.278431,0.403922},{0.309804,0.309804,0.450980},{0.356863,0.356863,0.498039},{0.388235,0.388235,0.545098},{0.419608,0.419608,0.592157},{0.450980,0.450980,0.639216},{0.482353,0.482353,0.686275},{0.513726,0.513726,0.733333},{0.545098,0.545098,0.796078},{0.000000,0.000000,0.000000},{0.027451,0.027451,0.000000},{0.043137,0.043137,0.000000},{0.074510,0.074510,0.000000},{0.105882,0.105882,0.000000},{0.137255,0.137255,0.000000},{0.168627,0.168627,0.027451},{0.184314,0.184314,0.027451},{0.215686,0.215686,0.027451},{0.247059,0.247059,0.027451},{0.278431,0.278431,0.027451},{0.294118,0.294118,0.043137},{0.325490,0.325490,0.043137},{0.356863,0.356863,0.043137},{0.388235,0.388235,0.043137},{0.419608,0.419608,0.058824},{0.027451,0.000000,0.000000},{0.058824,0.000000,0.000000},{0.090196,0.000000,0.000000},{0.121569,0.000000,0.000000},{0.152941,0.000000,0.000000},{0.184314,0.000000,0.000000},{0.215686,0.000000,0.000000},{0.247059,0.000000,0.000000},{0.278431,0.000000,0.000000},{0.309804,0.000000,0.000000},{0.341176,0.000000,0.000000},{0.372549,0.000000,0.000000},{0.403922,0.000000,0.000000},{0.435294,0.000000,0.000000},{0.466667,0.000000,0.000000},{0.498039,0.000000,0.000000},{0.074510,0.074510,0.000000},{0.105882,0.105882,0.000000},{0.137255,0.137255,0.000000},{0.184314,0.168627,0.000000},{0.215686,0.184314,0.000000},{0.262745,0.215686,0.000000},{0.294118,0.231373,0.027451},{0.341176,0.262745,0.027451},{0.372549,0.278431,0.027451},{0.419608,0.294118,0.043137},{0.466667,0.325490,0.058824},{0.513726,0.341176,0.074510},{0.545098,0.356863,0.074510},{0.592157,0.372549,0.105882},{0.639216,0.388235,0.121569},{0.686275,0.403922,0.137255},{0.137255,0.074510,0.027451},{0.184314,0.090196,0.043137},{0.231373,0.121569,0.058824},{0.294118,0.137255,0.074510},{0.341176,0.168627,0.090196},{0.388235,0.184314,0.121569},{0.450980,0.215686,0.137255},{0.498039,0.231373,0.168627},{0.560784,0.262745,0.200000},{0.623529,0.309804,0.200000},{0.686275,0.388235,0.184314},{0.749020,0.466667,0.184314},{0.811765,0.560784,0.168627},{0.874510,0.670588,0.152941},{0.937255,0.796078,0.121569},{1.000000,0.952941,0.105882},{0.043137,0.027451,0.000000},{0.105882,0.074510,0.000000},{0.168627,0.137255,0.058824},{0.215686,0.168627,0.074510},{0.278431,0.200000,0.105882},{0.325490,0.215686,0.137255},{0.388235,0.247059,0.168627},{0.435294,0.278431,0.200000},{0.498039,0.325490,0.247059},{0.545098,0.372549,0.278431},{0.607843,0.419608,0.325490},{0.654902,0.482353,0.372549},{0.717647,0.529412,0.419608},{0.764706,0.576471,0.482353},{0.827451,0.639216,0.545098},{0.890196,0.701961,0.592157},{0.670588,0.545098,0.639216},{0.623529,0.498039,0.592157},{0.576471,0.450980,0.529412},{0.545098,0.403922,0.482353},{0.498039,0.356863,0.435294},{0.466667,0.325490,0.388235},{0.419608,0.294118,0.341176},{0.372549,0.247059,0.294118},{0.341176,0.215686,0.262745},{0.294118,0.184314,0.215686},{0.262745,0.152941,0.184314},{0.215686,0.121569,0.137255},{0.168627,0.090196,0.105882},{0.137255,0.074510,0.074510},{0.090196,0.043137,0.043137},{0.058824,0.027451,0.027451},{0.733333,0.450980,0.623529},{0.686275,0.419608,0.560784},{0.639216,0.372549,0.513726},{0.592157,0.341176,0.466667},{0.545098,0.309804,0.419608},{0.498039,0.294118,0.372549},{0.450980,0.262745,0.325490},{0.419608,0.231373,0.294118},{0.372549,0.200000,0.247059},{0.325490,0.168627,0.215686},{0.278431,0.137255,0.168627},{0.231373,0.121569,0.137255},{0.184314,0.090196,0.105882},{0.137255,0.074510,0.074510},{0.090196,0.043137,0.043137},{0.058824,0.027451,0.027451},{0.858824,0.764706,0.733333},{0.796078,0.701961,0.654902},{0.749020,0.639216,0.607843},{0.686275,0.592157,0.545098},{0.639216,0.529412,0.482353},{0.592157,0.482353,0.435294},{0.529412,0.435294,0.372549},{0.482353,0.388235,0.325490},{0.419608,0.341176,0.278431},{0.372549,0.294118,0.231373},{0.325490,0.247059,0.200000},{0.262745,0.200000,0.152941},{0.215686,0.168627,0.121569},{0.152941,0.121569,0.090196},{0.105882,0.074510,0.058824},{0.058824,0.043137,0.027451},{0.435294,0.513726,0.482353},{0.403922,0.482353,0.435294},{0.372549,0.450980,0.403922},{0.341176,0.419608,0.372549},{0.309804,0.388235,0.341176},{0.278431,0.356863,0.309804},{0.247059,0.325490,0.278431},{0.215686,0.294118,0.247059},{0.184314,0.262745,0.215686},{0.168627,0.231373,0.184314},{0.137255,0.200000,0.152941},{0.121569,0.168627,0.121569},{0.090196,0.137255,0.090196},{0.058824,0.105882,0.074510},{0.043137,0.074510,0.043137},{0.027451,0.043137,0.027451},{1.000000,0.952941,0.105882},{0.937255,0.874510,0.090196},{0.858824,0.796078,0.074510},{0.796078,0.717647,0.058824},{0.733333,0.654902,0.058824},{0.670588,0.592157,0.043137},{0.607843,0.513726,0.027451},{0.545098,0.450980,0.027451},{0.482353,0.388235,0.027451},{0.419608,0.325490,0.000000},{0.356863,0.278431,0.000000},{0.294118,0.215686,0.000000},{0.231373,0.168627,0.000000},{0.168627,0.121569,0.000000},{0.105882,0.058824,0.000000},{0.043137,0.027451,0.000000},{0.000000,0.000000,1.000000},{0.043137,0.043137,0.937255},{0.074510,0.074510,0.874510},{0.105882,0.105882,0.811765},{0.137255,0.137255,0.749020},{0.168627,0.168627,0.686275},{0.184314,0.184314,0.623529},{0.184314,0.184314,0.560784},{0.184314,0.184314,0.498039},{0.184314,0.184314,0.435294},{0.184314,0.184314,0.372549},{0.168627,0.168627,0.309804},{0.137255,0.137255,0.247059},{0.105882,0.105882,0.184314},{0.074510,0.074510,0.121569},{0.043137,0.043137,0.058824},{0.168627,0.000000,0.000000},{0.231373,0.000000,0.000000},{0.294118,0.027451,0.000000},{0.372549,0.027451,0.000000},{0.435294,0.058824,0.000000},{0.498039,0.090196,0.027451},{0.576471,0.121569,0.027451},{0.639216,0.152941,0.043137},{0.717647,0.200000,0.058824},{0.764706,0.294118,0.105882},{0.811765,0.388235,0.168627},{0.858824,0.498039,0.231373},{0.890196,0.592157,0.309804},{0.905882,0.670588,0.372549},{0.937255,0.749020,0.466667},{0.968627,0.827451,0.545098},{0.654902,0.482353,0.231373},{0.717647,0.607843,0.215686},{0.780392,0.764706,0.215686},{0.905882,0.890196,0.341176},{0.498039,0.749020,1.000000},{0.670588,0.905882,1.000000},{0.843137,1.000000,1.000000},{0.403922,0.000000,0.000000},{0.545098,0.000000,0.000000},{0.701961,0.000000,0.000000},{0.843137,0.000000,0.000000},{1.000000,0.000000,0.000000},{1.000000,0.952941,0.576471},{1.000000,0.968627,0.780392},{1.000000,1.000000,1.000000},{0.623529,0.356863,0.325490},
};

void World_init() {
	world_loaded_world[0] = 0;

	player_is_on_floor = false;
	player_queue_jump = false;

	//vec3 testspawnpoint = { 5.64962 ,-1418.14612 ,-928.71838 };
	//vec3 testspawnpoint = { 0 ,0 ,128 };
	//vec3 testspawnpoint = { 0 ,0 ,0 };

	//glm_vec3_zero(camera_pos);
	glm_vec3_zero(player_velocity);
	glm_vec2_zero(player_plane_moveaxis);

	glm_vec3_copy((vec3) { 0.0f, 0.0f, -1.0f }, camera_front);
	glm_vec3_copy((vec3) { 0.0f, 1.0f, 0.0f }, camera_up);

	glm_mat4_identity(world_view);

	Shader_create(&world_shader, WORLD_VSHADER, WORLD_FSHADER);

	World_resize_viewport(Game_win_get_width(), Game_win_get_height());
}

void World_clear() {
	// delet
	entitysrc_clear();
	Mem_release(&world_entities_src);

	for (uint32_t i = 0; i < world_models.size; i++) {
		world_model_t* model = MEMARRAYINDEXPTR(world_models, world_model_t, i);
		Mem_release(&model->meshes);
	}
	Mem_release(&world_models);
	Mem_release(&world_textures);

	Texture_delete(&lightmap_texture);
}

void World_draw()
{
	if (world_loaded_world[0] == 0) return;

	Shader_use(&world_shader);

	GLint u_tex = glGetUniformLocation(world_shader.program, "tex");
	GLint u_lightmap = glGetUniformLocation(world_shader.program, "lightmap");

	glUniform1i(u_tex, 0);
	glUniform1i(u_lightmap, 1);

	// draw
	for (int i = 0; i < world_models.size; i++) {
		world_model_t* model = MEMARRAYINDEXPTR(world_models, world_model_t, i);

		for (int j = 0; j < model->meshes.size; j++) {
			world_mesh_t* mesh = MEMARRAYINDEXPTR(model->meshes, world_mesh_t, j);
			if (mesh->vao == -1)
				continue;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->texture->texture.id);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, lightmap_texture.id);

			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

			//printf("%d\n", mesh->vi_cursor);
			glDrawArrays(GL_TRIANGLES, 0, mesh->fi_cursor);

			//break;
		}
	}

	//Camera_translate((vec3) { 0.0f, 0.0f, -10.0f });
}
#define PRINTV(v) printf("(%f %f %f)\n", v[0], v[1], v[2])
void camera_upload_view_matrix();
void World_physic_step() {
	vec3 oldpos;
	glm_vec3_copy(camera_pos, oldpos);

	vec3 dirf = { camera_front[0], 0.0f, camera_front[2] };
	vec3 dirl;
	glm_vec3_copy(dirf, dirl);
	glm_vec3_rotate(dirl, -GLM_PIf * 0.5 /* -90 deg */, (vec3) { 0.0f, 1.0f, 0.0f }); // rotate FORWARD vector to LEFT vector
	//glm_vec3_normalize(dirf);
	//glm_vec3_normalize(dirl);

	glm_vec3_copy((vec3) {
		player_plane_moveaxis[0] * dirf[0] + player_plane_moveaxis[1] * dirl[0],
			player_plane_moveaxis[0] * dirf[1] + player_plane_moveaxis[1] * dirl[1],
			player_plane_moveaxis[0] * dirf[2] + player_plane_moveaxis[1] * dirl[2]
	}, player_wishdir);

	glm_vec3_normalize(player_wishdir);

	//printf("(%f %f)\n", player_plane_moveaxis[0], player_plane_moveaxis[1]);
	//PRINTV(dirf);
	//PRINTV(dirl);
	//printf("(%f %f %f)\n", player_velocity[0], player_velocity[1], player_velocity[2]);

	// friction
	if (player_is_on_floor) {
		const float friction = 8.0f;
		float lspeed = glm_vec3_distance(GLM_VEC3_ZERO, player_velocity);
		if (lspeed > 0.0f) {
			float s = max(lspeed - (friction * lspeed * Game_delta()), 0.0f);
			glm_vec3_scale(player_velocity, s, player_velocity);
			glm_vec3_scale(player_velocity, 1.0f / lspeed, player_velocity);

		}
		else if (lspeed < 0.1) {
			glm_vec3_zero(player_velocity);
		}

	}

	// accelerate
	const float accel = 3200.0f;
	const float speed = 3200.0f;
	vec3 accelv;
	glm_vec3_scale(player_wishdir, (
		glm_clamp(
			(player_is_on_floor ? speed : 0.5f) -glm_vec3_dot(player_velocity, player_wishdir),
			0.0f, accel * Game_delta()
		)
		), accelv);
	glm_vec3_add(player_velocity, accelv, player_velocity);

	raycast_result_t res;
	vec3 realvel;
	vec3 added;
	vec3 added2;

	// stair

	glm_vec3_copy(player_velocity, realvel);
	realvel[1] = 0.0f;

	glm_vec3_scale(realvel, Game_delta(), realvel);
	glm_vec3_add(camera_pos, realvel, added);
	glm_vec3_copy(added, added2);
	added[1] += 16.0f;

	World_raycast(added, added2, &res);
	//printf("(%f %f %f) %d %d\n", res.normal[0], res.normal[1], res.normal[2], res.hit, res.split_claimed);
	if (res.split_claimed) {
		if (res.normal[1] > 0.7f) {
			// step up
			camera_pos[1] = res.point[1] + (8.0f / 32.0f); // << bonus margin
		}
	}

	// is on floor
	glm_vec3_copy(camera_pos, added);
	glm_vec3_copy(added, added2);
	added2[1] -= 1.0f;

	World_raycast(added, added2, &res);
	player_is_on_floor = false;
	if (res.split_claimed) {
		if (res.normal[1] > 0.7f) {
			player_is_on_floor = true;
		}
	}

	if (!player_is_on_floor)
		player_velocity[1] -= 800 * Game_delta(); // gravity
	else
		player_velocity[1] = 0.0f;

	if (player_queue_jump && player_is_on_floor) {
		player_velocity[1] += 244;
	}
	player_queue_jump = false;

	

	for (int i = 0; i < MAX_CHECK; i++) {

		// move
		glm_vec3_scale(player_velocity, Game_delta(), realvel);
		glm_vec3_add(camera_pos, realvel, added);
		World_raycast(camera_pos, added, &res);

		//printf("%d (%f %f %f) (%f %f %f)\n", res.hit, res.normal[0], res.normal[1], res.normal[2], res.point[0], res.point[1], res.point[2]);



		if (res.hit) {
			// nah stop
			vec3 cut;
			vec3 remaining;


			glm_vec3_sub(res.point, camera_pos, cut);
			glm_vec3_sub(player_velocity, cut, remaining);

			//PRINTV(cut);
			//PRINTV(remaining);

			vec3 slide;

			// slide
			glm_vec3_scale(res.normal, glm_vec3_dot(res.normal, remaining), slide);
			glm_vec3_sub(remaining, slide, slide);

			glm_vec3_copy(slide, player_velocity);

			//glm_vec3_print(res.normal, stdout);
			glm_vec3_scale(slide, Game_delta(), realvel);

			//player_velocity[1] += 800 * res.normal[1];
			//glm_vec3_print(res.normal, stdout);
		}
	}

	//printf("%d\n", res.hit);
	glm_vec3_add(camera_pos, realvel, camera_pos);
	//glm_vec3_print(realvel, stdout);

	if (!glm_vec3_eqv(camera_pos, oldpos))
		// pos changed
		camera_upload_view_matrix();
}

void World_resize_viewport(int width, int height)
{
	glm_perspective(glm_rad(75.0f), (float)width / (float)height, 0.1f, 100000.0f, world_projection);
	Shader_use(&world_shader);
	glUniformMatrix4fv(
		glGetUniformLocation(world_shader.program, "projection"),
		1, GL_FALSE, world_projection
	);

	mat4 model;
	glm_mat4_identity(model);
	glUniformMatrix4fv(
		glGetUniformLocation(world_shader.program, "model"),
		1, GL_FALSE, model
	);
}

void camera_upload_view_matrix() {
	vec3 posfront;
	vec3 realpos;
	glm_vec3_add(camera_pos, (vec3) { 0, 24, 0 }, realpos);

	glm_vec3_add(realpos, camera_front, posfront);
	glm_lookat(realpos, posfront, camera_up, world_view);

	Shader_use(&world_shader);
	glUniformMatrix4fv(
		glGetUniformLocation(world_shader.program, "view"),
		1, GL_FALSE, world_view
	);
}

// watch https://www.youtube.com/watch?v=wLHXn8IlAiA before understanding these
bool_t _recursive_raycast(int32_t nidx, vec3 from, vec3 to, raycast_result_t* res) {
	if (nidx == CONTENTS_SOLID)
		return true; // hit something
	if (nidx < 0)
		return false; // hit air, water, lava, etc

	world_clipnode_t* node = MEMARRAYINDEXPTR(world_clipnodes, world_clipnode_t, nidx);
	worldplane_t* plane = &node->plane;

	// distance from plane to from and to
	float d1 = glm_dot(*plane, from) - (*plane)[3];
	float d2 = glm_dot(*plane, to) - (*plane)[3];

	// copy vec4.xyz to result normal
	if (d1 < 0)
		// other side ? flip normal
		glm_vec3_sub(GLM_VEC3_ZERO, *(vec3*)plane, res->normal);
	else
		glm_vec3_copy(*(vec3*)plane, res->normal);

	if (d1 >= 0 && d2 >= 0) {
		// hmm both are in front of plane
		return _recursive_raycast(node->front, from, to, res);
	}
	else if (d1 < 0 && d2 < 0) {
		// hmm both are behind plane
		return _recursive_raycast(node->back, from, to, res);
	}
	else {
		// one is in front, one is behind . . . we have to split the ray
		float frac = d1 / (d1 - d2); // simply an inverse lerp

		glm_vec3_lerp(from, to, frac, res->point); // haha im middle

		// try front
		bool_t hit = _recursive_raycast(d2 < d1 ? node->front : node->back, from, res->point, res);
		if (hit) {
			//printf("WOW\n");
			return true;
		}

		// no ? try back
		bool_t ress = _recursive_raycast(d1 < d2 ? node->front : node->back, res->point, to, res);

		if (ress && !res->split_claimed) {
			// yes recopy
			if (d1 < 0)
				// other side ? flip normal
				glm_vec3_sub(GLM_VEC3_ZERO, *(vec3*)plane, res->normal);
			else
				glm_vec3_copy(*(vec3*)plane, res->normal);
			res->split_claimed = true;
		}
		return ress;
	}
}

void World_raycast(vec3 from, vec3 to, raycast_result_t* result)
{
	result->hit = true;
	result->split_claimed = false;
	glm_vec3_copy(to, result->point);

	// check on all models
	for (int i = 0; i < world_models.size; i++) {
		world_model_t* model = MEMARRAYINDEXPTR(world_models, world_model_t, i);

		if (_recursive_raycast(model->first_node, from, to, result)) {
			// yee hit something
			return;
		}
	}

	// no ?
	result->hit = false;
}

void World_pointcheck(vec3 point, pointcheck_result_t* result) {
	// check on all models
	for (int i = 0; i < world_models.size; i++) {
		world_model_t* model = MEMARRAYINDEXPTR(world_models, world_model_t, i);
		int32_t node = model->first_node;
		world_clipnode_t* clipnode = NULL;

		while (node >= 0) {
			clipnode = MEMARRAYINDEXPTR(world_clipnodes, world_clipnode_t, node);
			worldplane_t* plane = &clipnode->plane;

			float d = glm_dot(*plane, point) - (*plane)[3];
			if (d >= 0)
				node = clipnode->front;
			else
				node = clipnode->back;
		}

		result->content_type = node;
		if (node != CONTENTS_EMPTY)
			return; // found solid
	}
}

void Camera_set_position(vec3 v)
{
	//glm_vec3_copy(v, camera_pos);
	//camera_upload_view_matrix();
}

void Camera_translate(vec3 v)
{
	//glm_vec3_add(camera_pos, v, camera_pos);
	//glm_translate(world_view, v);
	//camera_upload_view_matrix();
}

void Camera_forward(float step)
{
	vec3 v;
	glm_vec3_scale(camera_front, step, v);
	glm_vec3_add(player_velocity, v, player_velocity);
	camera_upload_view_matrix();
}

void Camera_right(float step)
{
	vec3 v;
	glm_cross(camera_front, camera_up, v);
	glm_normalize(v);
	glm_vec3_scale(v, step, v);
	glm_vec3_add(player_velocity, v, player_velocity);
	camera_upload_view_matrix();
}

void Camera_update_yawpitch(float yaw, float pitch) {
	vec3 f = {
		cos(glm_rad(yaw)) * cos(glm_rad(pitch)),
		sin(glm_rad(pitch)),
		sin(glm_rad(yaw)) * cos(glm_rad(pitch)),
	};
	glm_normalize(f);
	glm_vec3_copy(f, camera_front);

	camera_upload_view_matrix();
}

void Player_forward(float step)
{
	player_plane_moveaxis[0] = step;
}

void Player_right(float step)
{
	player_plane_moveaxis[1] = step;
}

void Player_jump()
{
	player_queue_jump = true;
}

void Worldutil_vec3_to_opengl(vec3 v, vec3 dest)
{
	dest[0] = -v[0];
	dest[1] = v[2];
	dest[2] = v[1];
}

float invlerp(float a, float b, float v)
{
	return (v - a) / (b - a);
}


int World_load_bsp(const char* map_name)
{
	char fullpath[256];

	sprintf(fullpath, "assets/maps/%s.bsp", map_name);

	FILE* f = fopen(fullpath, "rb");
	if (!f) return -1;

	bsp_header_t header;
	fread(&header, sizeof(header), 1, f);

	if (header.version != 29 && header.version != 30) {
		Con_printf("bro opens the unsupported BSP (neither 29 nor 30)");
		return ERR_CANNOT_OPEN;
	}

	typedef struct {
		uint32_t* vertices;
		vec2* uv;
		vec2* uv2;
		uint32_t face;
	} face_uv;

	MEMSTATIC(planes);
	MEMSTATIC(vertices);
	MEMSTATIC(vislist);
	MEMSTATIC(nodes); // bsp nodes
	MEMSTATIC(surface);
	MEMSTATIC(faces);
	MEMSTATIC(lightmaps);
	MEMSTATIC(clipnodes);
	MEMSTATIC(leaves);
	MEMSTATIC(lface);
	MEMSTATIC(edges);
	MEMSTATIC(ledges);
	MEMSTATIC(models);

	MEMSTATIC(face_uvs);

	// entities
	fseek(f, header.entities.offset, SEEK_SET);
	MEMCREATE(world_entities_src, header.entities.size + 1);
	MEMREADLUMP(world_entities_src, f);
	((char*)world_entities_src.data)[header.entities.size] = '\0';

	if (entitysrc_parse(world_entities_src.data) == 555)
		return 555;

	// planes
	fseek(f, header.planes.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(planes, sizeof(plane_t), header.planes.size);
	MEMREADLUMP(planes, f);

	// miptextures
	fseek(f, header.miptex.offset, SEEK_SET);

	// read texture count
	uint32_t numtex;
	fread(&numtex, sizeof(uint32_t), 1, f);

	// load and generate textures
	MEMCREATEARRAY(world_textures, sizeof(world_texture_t), numtex);

	int32_t offset;

	for (int i = 0; i < numtex; i++) {
		world_texture_t* texture = MEMARRAYINDEXPTR(world_textures, world_texture_t, i);

		fseek(f, header.miptex.offset + sizeof(int32_t) * i + sizeof(uint32_t), SEEK_SET);

		fread(&offset, sizeof(int32_t), 1, f);
		if (offset < 0) {
			// invalid texture. SKip
			// assign some funni info
			strncpy(texture->name, "INVALIDTEXTURELMAO", 16);

			texture->texture.width = -1;
			texture->texture.height = -1;
			texture->texture.id = -1;
			continue;
		}
		uint32_t entry_offset = header.miptex.offset + offset;
		fseek(f, entry_offset, SEEK_SET);

		fread(&texture->name, sizeof(char), 16, f);
		uint32_t size[2];
		fread(size, sizeof(uint32_t), 2, f);

		texture->texture.width = size[0];
		texture->texture.height = size[1];

		uint32_t data_offset;
		fread(&data_offset, sizeof(uint32_t), 1, f);

		if (data_offset) {
			// embed into the file !!!
			fseek(f, entry_offset + data_offset, SEEK_SET);
			//char* id_data = malloc((size[0] * size[1]) * sizeof(char));

			size_t tnumsize = size[0] * size[1];
			struct rgb {
				uint8_t r, g, b;
			};
			struct rgb* im_data = malloc(tnumsize * sizeof(struct rgb));

			for (int j = 0; j < tnumsize; j++) {
				uint8_t index;
				fread(&index, sizeof(uint8_t), 1, f);


				im_data[j].r = (uint8_t)(quake_pal[index][0] * 255.0f);
				im_data[j].g = (uint8_t)(quake_pal[index][1] * 255.0f);
				im_data[j].b = (uint8_t)(quake_pal[index][2] * 255.0f);
			}

			glGenTextures(1, &texture->texture.id);
			glBindTexture(GL_TEXTURE_2D, texture->texture.id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size[0], size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, im_data);
			glGenerateMipmap(GL_TEXTURE_2D);
			free(im_data);
		}
		else {
			// load external (goldsrc)
			char path[256];
			sprintf(path, ASSET("textures/%s.png"), texture->name);
			Texture_load_from_file(&texture->texture, path);
		}
	}

	// vertices
	fseek(f, header.vertices.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(vertices, sizeof(vec3), header.vertices.size);
	MEMREADLUMP(vertices, f);

	// visilist
	fseek(f, header.visilist.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(vislist, sizeof(char), header.visilist.size);
	MEMREADLUMP(vislist, f);

	// bsp nodes
	fseek(f, header.nodes.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(nodes, sizeof(bspnode_t), header.nodes.size);
	MEMREADLUMP(nodes, f);

	// surfaces
	fseek(f, header.texinfo.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(surface, sizeof(surface_t), header.texinfo.size);
	MEMREADLUMP(surface, f);

	// faces
	fseek(f, header.faces.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(faces, sizeof(face_t), header.faces.size);
	MEMREADLUMP(faces, f);
	MEMCREATEARRAY(face_uvs, sizeof(face_uv), faces.size);
	memset(face_uvs.data, 0, face_uvs.size * sizeof(face_uv)); // set all to NULL

	// lightmaps
	fseek(f, header.lightmaps.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(lightmaps, sizeof(uint8_t), header.lightmaps.size);
	MEMREADLUMP(lightmaps, f);

	// clip nodes
	fseek(f, header.clipnodes.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(clipnodes, sizeof(clipnode_t), header.clipnodes.size);
	MEMREADLUMP(clipnodes, f);

	// bsp node leaves
	fseek(f, header.leaves.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(leaves, sizeof(dleaf_t), header.leaves.size);
	MEMREADLUMP(leaves, f);

	// face list
	fseek(f, header.lface.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(lface, sizeof(uint16_t), header.lface.size);
	MEMREADLUMP(lface, f);

	// edges
	fseek(f, header.edges.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(edges, sizeof(edge_t), header.edges.size);
	MEMREADLUMP(edges, f);

	// ledges
	fseek(f, header.ledges.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(ledges, sizeof(int), header.ledges.size);
	MEMREADLUMP(ledges, f);

	// models
	fseek(f, header.models.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(models, sizeof(model_t), sizeof(model_t));
	MEMREADLUMP(models, f);

	MEMCREATEARRAY(world_models, sizeof(world_model_t), models.size);

	MEMSTATIC(lightmap_rects);
	MEMCREATEDYNAMICARRAY(lightmap_rects, stbrp_rect);
	uint32_t lightmap_rects_size = 0;

	/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

	for (uint32_t i = 0; i < models.size; i++) {
		model_t* model = MEMARRAYINDEXPTR(models, model_t, i);
		world_model_t* wmodel = MEMARRAYINDEXPTR(world_models, world_model_t, i);

		wmodel->first_node = model->node_hulls[0];
		//wmodel->first_node = model->node_hulls[0];

		MEM(wmodel->meshes);
		MEMCREATEARRAY(wmodel->meshes, sizeof(world_mesh_t), world_textures.size);
		//MEMCREATEARRAY(wmodel->meshes, sizeof(world_mesh_t), 1);

		for (uint32_t j = 0; j < wmodel->meshes.size; j++) {
			world_mesh_t* m = MEMARRAYINDEXPTR(wmodel->meshes, world_mesh_t, j);

			MEM(m->face_items);
			MEMCREATEDYNAMICARRAY(m->face_items, uint32_t);
			m->fi_cursor = 0;
			m->texture = MEMARRAYINDEXPTR(world_textures, world_texture_t, j);
		}

		for (uint32_t j = 0; j < model->face_num; j++) {
			face_t* f = MEMARRAYINDEXPTR(faces, face_t, model->face_id + j);
			surface_t* surf = MEMARRAYINDEXPTR(surface, surface_t, f->texinfo_id);
			world_mesh_t* mesh = MEMARRAYINDEXPTR(wmodel->meshes, world_mesh_t, surf->texture_id);
			world_texture_t* tex = mesh->texture;

			void* ledge;
			ledge = MEMARRAYINDEXPTR(ledges, int, f->first_edge);

#define VECS pos
#define CHOOSEEDGE(iii, cast) {\
			edge = ((cast*)ledge)[iii];\
			if (edge >= 0)\
				VECS = MEMARRAYINDEXPTR(edges, edge_t, edge)->vertex[0];\
			else\
				VECS = MEMARRAYINDEXPTR(edges, edge_t, -edge)->vertex[1];\
			}

			// find lightmap uv

			vec2 uv_min = { FLT_MAX, FLT_MAX };
			vec2 uv_max = { -FLT_MAX, -FLT_MAX };

			face_uv facei;
			facei.uv = malloc(sizeof(vec2) * f->edge_count);
			facei.uv2 = malloc(sizeof(vec2) * f->edge_count);
			facei.vertices = malloc(sizeof(uint32_t) * f->edge_count);
			facei.face = model->face_id + j;

			vec2 tscale;
			tscale[0] = 1.0f / tex->texture.width;
			tscale[1] = 1.0f / tex->texture.height;

			for (uint32_t k = 0; k < f->edge_count; k++) {
				int edge;
				uint32_t pos;

				CHOOSEEDGE(k, int);

				vec3* vpos = MEMARRAYINDEXPTR(vertices, vec3, pos);

				vec2 uv0 = {
					glm_dot(*vpos, surf->vectorS) * tscale[0] +
					surf->distS / tex->texture.width
					,
					glm_dot(*vpos, surf->vectorT) * tscale[1] +
					surf->distT / tex->texture.height
				};

				vec2 uv2 = {
					glm_vec3_dot(*vpos, surf->vectorS) + surf->distS,
					glm_vec3_dot(*vpos, surf->vectorT) + surf->distT,
				};

				glm_vec2_copy(uv0, facei.uv[k]);
				glm_vec2_copy(uv2, facei.uv2[k]);
				facei.vertices[k] = pos;

				if (uv2[0] < uv_min[0]) uv_min[0] = uv2[0];
				if (uv2[1] < uv_min[1]) uv_min[1] = uv2[1];
				if (uv2[0] > uv_max[0]) uv_max[0] = uv2[0];
				if (uv2[1] > uv_max[1]) uv_max[1] = uv2[1];
			}

			// snap uv (16 unit)
			ivec2 snap_min, snap_max;
			snap_min[0] = floor(uv_min[0] / 16.0f);
			snap_min[1] = floor(uv_min[1] / 16.0f);
			snap_max[0] = ceil(uv_max[0] / 16.0f);
			snap_max[1] = ceil(uv_max[1] / 16.0f);

			for (uint32_t k = 0; k < f->edge_count; k++) {
				vec2* uv = &facei.uv2[k];
				(*uv)[0] = invlerp(snap_min[0] << 4, snap_max[0] << 4, (*uv)[0]);
				(*uv)[1] = invlerp(snap_min[1] << 4, snap_max[1] << 4, (*uv)[1]);
			}

			if (f->lightmap != -1) {

				// how many lightmap are there ?
				uint32_t lm = 0;
				for (uint32_t k = 0; k < 4; k++) {
					if (f->light[k] == 255) continue;
					lm++;
				}

				stbrp_rect rect;
				rect.id = model->face_id + j;
				rect.w = ((snap_max[0] - snap_min[0]) + 1) * lm;
				rect.h = (snap_max[1] - snap_min[1]) + 1;
				//printf("%d %d*%d %d\n", rect.id, rect.w, lm, rect.h);

				MEMDYNAMICARRAYPUSH(lightmap_rects, stbrp_rect, lightmap_rects_size,
					rect
				);
			}

			*MEMARRAYINDEXPTR(face_uvs, face_uv, model->face_id + j) = facei;
			MEMDYNAMICARRAYPUSH(mesh->face_items, uint32_t, mesh->fi_cursor, model->face_id + j);

#undef CHOOSEEDGE
#undef VECS
		}
	}

	// create lightmap textures

	if (lightmap_rects_size > 0) {
		// pack them first
		stbrp_context ctx;
		stbrp_node* packnodes = (stbrp_node*)malloc(sizeof(stbrp_node) * 1024 * 2);
		stbrp_init_target(&ctx, 1024, 1024, packnodes, 1024 * 2);
		stbrp_pack_rects(&ctx, lightmap_rects.data, lightmap_rects_size);
		free(packnodes);

		void* lightmap_bitmap;

		if (header.version == 29)
			lightmap_bitmap = malloc(1024 * 1024 * sizeof(uint8_t));
		else
			lightmap_bitmap = malloc(1024 * 1024 * sizeof(rgb_t));

		for (uint32_t i = 0; i < lightmap_rects_size; i++) {
			stbrp_rect* rect = MEMARRAYINDEXPTR(lightmap_rects, stbrp_rect, i);
			face_t* face = MEMARRAYINDEXPTR(faces, face_t, rect->id);
			const uint8_t* lightmapdata = ((uint8_t*)lightmaps.data + face->lightmap);

			face_uv* fuv = MEMARRAYINDEXPTR(face_uvs, face_uv, rect->id);
			vec2* uv2s = fuv->uv2;
			for (uint32_t k = 0; k < face->edge_count; k++) {
				vec2* uv = &uv2s[k];

				vec2 lpos = {
					((float)rect->x + 0.5f) / 1024.0f,
					((float)rect->y + 0.5f) / 1024.0f
				};

				vec2 fsize = {
					(rect->w - 1) / 1024.0f,
					(rect->h - 1) / 1024.0f
				};

				(*uv)[0] = ((*uv)[0] * fsize[0]) + lpos[0];
				(*uv)[1] = ((*uv)[1] * fsize[1]) + lpos[1];
			}

			//if (face->lightmap == -1) continue;

			// count lightmaps
			uint32_t lm = 0;
			for (uint32_t k = 0; k < 4; k++) {
				if (face->light[k] == 255) continue;
				lm++;
			}

			if (lm == 0)
				continue; // <- impossible

			const uint32_t real_width = (rect->w / lm);
			uint32_t cc = 0;

			// paint lightmaps
			for (uint32_t k = 0; k < 4; k++) {
				if (face->light[k] == 255) continue;
#define INDEXWITHSTRIDE (real_width * k + rect->x + x) + (y + rect->y) * 1024
				for (uint32_t y = 0; y < rect->h; y++) {
					for (uint32_t x = 0; x < real_width; x++) {
						if (header.version == 29) {
							((uint8_t*)lightmap_bitmap)[INDEXWITHSTRIDE] = lightmapdata[cc++];
						}
						else {
							rgb_t C = (rgb_t){ lightmapdata[cc++] ,lightmapdata[cc++] ,lightmapdata[cc++] };
							memcpy(&((rgb_t*)lightmap_bitmap)[INDEXWITHSTRIDE], &C, sizeof(rgb_t));
#undef INDEXWITHSTRIDE
						}
						//char name[1024];
						//sprintf(name, "lightmap%d.png", cc);
						//stbi_write_png(name, 1024, 1024, 3, lightmap_bitmap, 1024 * 3);
					}
				}
			}
			//break;
		}

		Texture_new(&lightmap_texture);
		if (header.version == 29) {
			Texture_from_data(&lightmap_texture, lightmap_bitmap, 1024, 1024, 1);
		}
		else {
			Texture_from_data(&lightmap_texture, lightmap_bitmap, 1024, 1024, 3);
		}
		//stbi_write_png("lightmap.png", 1024, 1024, 3, lightmap_bitmap, 1024 * 3);
		free(lightmap_bitmap);
	}
	else {
		// no lightmaps ?
		// create a dummy one
		uint8_t lightmap_bitmap_29[1] = { 255 }; // quake
		uint8_t lightmap_bitmap_30[3] = { 255, 255, 255 }; // goldsrc

		Texture_new(&lightmap_texture);
		if (header.version == 29)
			Texture_from_data(&lightmap_texture, lightmap_bitmap_29, 1, 1, 1);
		else
			Texture_from_data(&lightmap_texture, lightmap_bitmap_30, 1, 1, 3);
	}


	for (uint32_t i = 0; i < models.size; i++) {
		model_t* model = MEMARRAYINDEXPTR(models, model_t, i);
		world_model_t* wmodel = MEMARRAYINDEXPTR(world_models, world_model_t, i);

		for (uint32_t j = 0; j < wmodel->meshes.size; j++) {
			world_mesh_t* mesh = MEMARRAYINDEXPTR(wmodel->meshes, world_mesh_t, j);

			if (mesh->fi_cursor == 0) continue;

			MEMSTATIC(items);
			MEMCREATEDYNAMICARRAY(items, world_vertex_item_t);
			//MEMCREATEARRAY(items, sizeof(world_vertex_item_t), (mesh->fi_cursor - 2) * 3);
			uint32_t add_c = 0;

			for (uint32_t k = 0; k < mesh->fi_cursor; k++) {
				uint32_t fidx = *MEMARRAYINDEXPTR(mesh->face_items, uint32_t, k);
				face_uv* vidx = MEMARRAYINDEXPTR(face_uvs, face_uv, fidx);

				face_t* f = MEMARRAYINDEXPTR(faces, face_t, vidx->face);
				surface_t* surf = MEMARRAYINDEXPTR(surface, surface_t, f->texinfo_id);
				vec3* normal = MEMARRAYINDEXPTR(planes, plane_t, f->plane_id)->normal;

				world_texture_t* tex = mesh->texture;

				world_vertex_item_t vi;
				uint32_t vi0;
				vec3* v0;

				for (uint32_t l = 0; l < f->edge_count - 2; l++) {
#define ADDV(i)\
					vi0 = vidx->vertices[i];\
					v0 = MEMARRAYINDEXPTR(vertices, vec3, vi0);\
					Worldutil_vec3_to_opengl(*v0, vi.vertex);\
					Worldutil_vec3_to_opengl(*normal, vi.normal);\
					glm_vec2_copy(vidx->uv[i], vi.uv_texture);\
					glm_vec2_copy(vidx->uv2[i], vi.uv_lightmap);\
					MEMDYNAMICARRAYPUSH(items, world_vertex_item_t, add_c, vi)

					ADDV(0);
					ADDV(l + 1);
					ADDV(l + 2);
				}
				free(vidx->uv);
				free(vidx->uv2);
				free(vidx->vertices);
			}

			MEMDYNAMICARRAYFIXSIZE(items, world_vertex_item_t, add_c);
			Mem_replace(&items, &mesh->face_items);

			if (add_c == 0) {
				// empty mesh
				mesh->vao = -1;
				mesh->vbo = -1;
				continue;
			}

			mesh->fi_cursor = add_c;

			glGenVertexArrays(1, &mesh->vao);
			glGenBuffers(1, &mesh->vbo);
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(world_vertex_item_t) * add_c, mesh->face_items.data, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(world_vertex_item_t), NULL);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(world_vertex_item_t), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(world_vertex_item_t), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(world_vertex_item_t), (void*)(8 * sizeof(float)));

			Mem_release(&mesh->face_items);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		///////////
	}
	Mem_release(&face_uvs);

	/* convert nodes for collisions */

	MEMCREATEARRAY(world_clipnodes, sizeof(world_clipnode_t), clipnodes.size);
	for (uint32_t i = 0; i < clipnodes.size; i++) {
		clipnode_t* node = MEMARRAYINDEXPTR(clipnodes, clipnode_t, i);
		world_clipnode_t* item = MEMARRAYINDEXPTR(world_clipnodes, world_clipnode_t, i);
		plane_t* plane = MEMARRAYINDEXPTR(planes, plane_t, node->plane_id);

		item->plane[0] = -plane->normal[0];
		item->plane[1] = plane->normal[2];
		item->plane[2] = plane->normal[1];
		item->plane[3] = plane->dist;
		item->front = node->front;
		item->back = node->back;
	}
	Mem_release(&clipnodes);

	/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

	Mem_release(&planes);
	//Mem_release(&world_textures);
	Mem_release(&vertices);
	Mem_release(&vislist);
	Mem_release(&nodes);
	Mem_release(&surface);
	Mem_release(&faces);
	Mem_release(&lightmaps);
	Mem_release(&clipnodes);
	Mem_release(&leaves);
	Mem_release(&lface);
	Mem_release(&edges);
	Mem_release(&ledges);
	Mem_release(&models);

	strcpy(world_loaded_world, map_name);

	entitysrc_get_player_start_position(camera_pos);
	Worldutil_vec3_to_opengl(camera_pos, camera_pos);

	return 0;
}