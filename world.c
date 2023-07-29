#include "world.h"

// should work on Quake 1 and GoldSRC (halflife1) maps

char world_loaded_world[256]; // if the first character is not 0, then the world is loaded.

MEMSTATIC(world_entities_src);

MEMSTATIC(world_textures);
MEMSTATIC(world_models);

shader_t world_shader;

#include "glad.h"
#include <GLFW/glfw3.h>

#include "shaders.h"

mat4 world_projection;
mat4 world_view;
vec3 camera_pos;
vec3 camera_rot;

void World_init() {
	world_loaded_world[0] = 0;

	glm_vec3_zero(camera_pos);
	glm_vec3_zero(camera_rot);

	glm_mat4_identity(world_view);

	Shader_create(&world_shader, WORLD_VSHADER, WORLD_FSHADER);

	World_resize_viewport(Game_win_get_width(), Game_win_get_height());
}

void World_clear() {
	// delet
	Mem_release(&world_entities_src);

	for (uint32_t i = 0; i < world_models.size; i++) {
		world_model_t* model = MEMARRAYINDEXPTR(world_models, world_model_t, i);
		Mem_release(&model->meshes);
	}
	Mem_release(&world_models);
	Mem_release(&world_textures);
}

void World_draw()
{
	if (world_loaded_world[0] == 0) return;

	Shader_use(&world_shader);

	// draw
	for (int i = 0; i < world_models.size; i++) {
		world_model_t* model = MEMARRAYINDEXPTR(world_models, world_model_t, i);

		for (int j = 0; j < model->meshes.size; j++) {
			world_mesh_t* mesh = MEMARRAYINDEXPTR(model->meshes, world_mesh_t, j);
			if (mesh->vao == -1)
				continue;

			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

			//printf("%d\n", mesh->vi_cursor);
			glDrawArrays(GL_TRIANGLES, 0, mesh->vi_cursor);

			//break;
		}
	}

	Camera_translate((vec3) { 0.0f, 0.0f, -10.0f });
}

void World_resize_viewport(int width, int height)
{
	glm_perspective(glm_rad(45.0f), (float)width / (float)height, 0.1f, 100000.0f, world_projection);
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
	Shader_use(&world_shader);
	glUniformMatrix4fv(
		glGetUniformLocation(world_shader.program, "view"),
		1, GL_FALSE, world_view
	);
}

void Camera_set_position(vec3 v)
{
	glm_vec3_copy(v, camera_pos);
	camera_upload_view_matrix();
}

void Camera_translate(vec3 v)
{
	//glm_vec3_add(camera_pos, v, camera_pos);
	glm_translate(world_view, v);
	camera_upload_view_matrix();
}

void Worldutil_vec3_to_opengl(vec3 v, vec3 dest)
{
	dest[0] = -v[0];
	dest[1] = v[2];
	dest[2] = v[1];
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

	// entities
	fseek(f, header.entities.offset, SEEK_SET);
	MEMCREATE(world_entities_src, header.entities.size);
	MEMREADLUMP(world_entities_src, f);

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
		fseek(f, header.miptex.offset + offset, SEEK_SET);

		fread(&texture->name, sizeof(char), 16, f);
		uint32_t size[2];
		fread(size, sizeof(uint32_t), 2, f);

		texture->texture.width = size[0];
		texture->texture.height = size[1];

		uint32_t data_offset;
		fread(&data_offset, sizeof(uint32_t), 1, f);

		if (data_offset) {
			// embed into the file !!!
			fseek(f, data_offset, SEEK_SET);
			char* im_data = malloc((size[0] * size[1]) * sizeof(char));
			fread(&im_data, sizeof(char), size[0] * size[1], f);

			glGenTextures(1, &texture->texture.id);
			glBindTexture(GL_TEXTURE_2D, texture->texture.id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size[0], size[1], 0, GL_RED, GL_UNSIGNED_BYTE, im_data);
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
	MEMCREATEARRAYLUMP(vertices, sizeof(vec3), header.planes.size);
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

	// lightmaps
	fseek(f, header.lightmaps.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(lightmaps, sizeof(u_char), header.lightmaps.size);
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
	MEMCREATEARRAYLUMP(lface, sizeof(u_short), header.lface.size);
	MEMREADLUMP(lface, f);

	// edges
	fseek(f, header.edges.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(edges, sizeof(edge_t), header.edges.size);
	MEMREADLUMP(edges, f);

	// ledges
	uint32_t edgeidsize = (header.version == 29) ? sizeof(short) : sizeof(int);
	fseek(f, header.ledges.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(ledges, edgeidsize, header.ledges.size);
	MEMREADLUMP(ledges, f);

	// models
	fseek(f, header.models.offset, SEEK_SET);
	MEMCREATEARRAYLUMP(models, sizeof(model_t), sizeof(model_t));
	MEMREADLUMP(models, f);

	MEMCREATEARRAY(world_models, sizeof(world_model_t), models.size);

	/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

	for (uint32_t i = 0; i < models.size; i++) {
		model_t* model = MEMARRAYINDEXPTR(models, model_t, i);
		world_model_t* wmodel = MEMARRAYINDEXPTR(world_models, world_model_t, i);

		MEM(wmodel->meshes);
		MEMCREATEARRAY(wmodel->meshes, sizeof(world_mesh_t), world_textures.size);
		//MEMCREATEARRAY(wmodel->meshes, sizeof(world_mesh_t), 1);

		typedef struct {
			uint32_t triangle[3];
			uint32_t face;
		} vertex_item_con;

		for (uint32_t j = 0; j < wmodel->meshes.size; j++) {
			world_mesh_t* m = MEMARRAYINDEXPTR(wmodel->meshes, world_mesh_t, j);

			MEM(m->vertex_items);
			MEMCREATEDYNAMICARRAY(m->vertex_items, vertex_item_con);
			m->vi_cursor = 0;
			m->texture = MEMARRAYINDEXPTR(world_textures, world_texture_t, j);
		}

		for (uint32_t j = 0; j < model->face_num; j++) {
			face_t* f = MEMARRAYINDEXPTR(faces, face_t, model->face_id + j);
			surface_t* surf = MEMARRAYINDEXPTR(surface, surface_t, f->texinfo_id);
			world_mesh_t* mesh = MEMARRAYINDEXPTR(wmodel->meshes, world_mesh_t, surf->texture_id);

			void* ledge;
			if (header.version == 29) {
				ledge = MEMARRAYINDEXPTR(ledges, short, f->first_edge);
			}
			else {
				ledge = MEMARRAYINDEXPTR(ledges, int, f->first_edge);
			}

			// triangulate

			uint32_t tri_count = f->edge_count - 2;
			for (uint32_t k = 1; k <= tri_count; k++) {
				int edge;
				vertex_item_con vic;
				vic.face = model->face_id + j;
				
				uint32_t l = 0;
#define CHOOSEEDGE(iii, cast)\
				edge = ((cast*)ledge)[iii];\
				if (edge >= 0)\
					vic.triangle[l] = MEMARRAYINDEXPTR(edges, edge_t, edge)->vertex[0];\
				else\
					vic.triangle[l] = MEMARRAYINDEXPTR(edges, edge_t, -edge)->vertex[1];\
				l++

				if (header.version == 29) {
					CHOOSEEDGE(0, short);
					CHOOSEEDGE(k, short);
					CHOOSEEDGE(k + 1, short);
				}
				else {
					CHOOSEEDGE(0, int);
					CHOOSEEDGE(k, int);
					CHOOSEEDGE(k + 1, int);
				}

				MEMDYNAMICARRAYPUSH(mesh->vertex_items, vertex_item_con, mesh->vi_cursor, vic);
			}
		}

		for (uint32_t j = 0; j < wmodel->meshes.size; j++) {
			world_mesh_t* mesh = MEMARRAYINDEXPTR(wmodel->meshes, world_mesh_t, j);

			MEMSTATIC(items);
			MEMCREATEARRAY(items, sizeof(world_vertex_item_t), mesh->vi_cursor * 3);
			uint32_t add_c = 0;

			for (uint32_t k = 0; k < mesh->vi_cursor; k++) {
				vertex_item_con* vidx = MEMARRAYINDEXPTR(mesh->vertex_items, vertex_item_con, k);

				face_t* f = MEMARRAYINDEXPTR(faces, face_t, vidx->face);
				surface_t* surf = MEMARRAYINDEXPTR(surface, surface_t, f->texinfo_id);

				world_mesh_t* mesh = MEMARRAYINDEXPTR(wmodel->meshes, world_mesh_t, surf->texture_id);
				world_texture_t* tex = mesh->texture;

				vec2 tscale;
				tscale[0] = 1.0f / tex->texture.width;
				tscale[1] = 1.0f / tex->texture.height;

				for (uint32_t l = 0; l < 3; l++) {
					world_vertex_item_t vi;
					vec3* v0 = MEMARRAYINDEXPTR(vertices, vec3, vidx->triangle[l]);

					vec2 uv0 = {
						glm_dot(*v0, surf->vectorS) * tscale[0] +
						surf->distS / tex->texture.width
						,
						glm_dot(*v0, surf->vectorT) * tscale[1] +
						surf->distT / tex->texture.height
					};

					vec3* normal = MEMARRAYINDEXPTR(planes, plane_t, f->plane_id)->normal;

					Worldutil_vec3_to_opengl(*v0, vi.vertex);
					Worldutil_vec3_to_opengl(*normal, vi.normal);
					glm_vec2_copy(uv0, vi.uv_texture);
					*MEMARRAYINDEXPTR(items, world_vertex_item_t, add_c++) = vi;
				}
			}

			Mem_replace(&items, &mesh->vertex_items);

			if (add_c == 0) {
				// empty mesh
				mesh->vao = -1;
				mesh->vbo = -1;
				continue;
			}

			mesh->vi_cursor *= 3;

			glGenVertexArrays(1, &mesh->vao);
			glGenBuffers(1, &mesh->vbo);
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(world_vertex_item_t) * add_c, mesh->vertex_items.data, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(world_vertex_item_t), NULL);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(world_vertex_item_t), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(world_vertex_item_t), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(world_vertex_item_t), (void*)(8 * sizeof(float)));

			Mem_release(&mesh->vertex_items);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		///////////
	}

	/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

	Mem_release(&planes);
	Mem_release(&world_textures);
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

	return 0;
}
