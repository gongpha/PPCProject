#include "world.h"

char world_loaded_world[256]; // if the first character is not 0, then the world is loaded.

MEMSTATIC(world_entities_src);

MEMSTATIC(world_planes);
MEMSTATIC(world_textures);
MEMSTATIC(world_vertices);
MEMSTATIC(world_vislist);
MEMSTATIC(world_nodes); // bsp nodes
MEMSTATIC(world_surface);
MEMSTATIC(world_faces);
MEMSTATIC(world_lightmaps);
MEMSTATIC(world_clipnodes);
MEMSTATIC(world_leaves);
MEMSTATIC(world_lface);
MEMSTATIC(world_edges);
MEMSTATIC(world_ledges);
MEMSTATIC(world_models);

#include "memory.h"

#include "glad.h"
#include <GLFW/glfw3.h>

void world__init() {
	world_loaded_world[0] = 0;
}

void world__clear() {
	// delet
	Mem_release(&world_entities_src);
	Mem_release(&world_planes);
	Mem_release(&world_textures);
}

int world__load_bsp(const char* map_name)
{
	char fullpath[256];

	sprintf(fullpath, "maps/%s.bsp", map_name);

	FILE* f = fopen(fullpath, "r");
	if (!f) return -1;

	bsp_header_t header;
	fread(&header, sizeof(header), 1, f);

	// entities
	fseek(f, header.entities.offset, SEEK_SET);
	MEMCREATE(world_entities_src, header.entities.size + 1);
	MEMREADLUMP(world_entities_src, f);
	((char*)world_entities_src.data)[header.entities.size] = 0; // terminate

	// planes
	fseek(f, header.planes.offset, SEEK_SET);
	MEMCREATEARRAY(world_planes, sizeof(plane_t), header.planes.size);
	MEMREADLUMP(world_planes, f);

	// miptextures
	fseek(f, header.miptex.offset, SEEK_SET);

	// read texture count
	long numtex;
	fread(&numtex, sizeof(long), 1, f);

	// load and generate textures
	MEMCREATEARRAY(world_textures, sizeof(world_texture_t), numtex);

	long offset;

	for (int i = 0; i < numtex; i++) {
		world_texture_t* texture = MEMARRAYINDEXPTR(world_textures, world_texture_t, i);

		fseek(f, header.miptex.offset + sizeof(u_long) * i + sizeof(long), SEEK_SET);

		fread(&offset, sizeof(long), 1, f);
		if (offset < 0) {
			// invalid texture. SKip
			// assign some funni info
			strncpy(texture->name, "INVALIDTEXTURELMAO", 16);
			texture->size[0] = -1;
			texture->size[1] = -1;
			texture->texture = -1;
			continue;
		}
		fseek(f, offset, SEEK_SET);

		fread(&texture->name, sizeof(char), 16, f);
		fread(&texture->size[0], sizeof(u_long), 2, f);

		u_long data_offset;
		fread(&data_offset, sizeof(u_long), 1, f);
		fseek(f, data_offset, SEEK_SET);
		char* im_data = malloc((texture->size[0] * texture->size[1]) * sizeof(char));
		fread(&im_data, sizeof(char), texture->size[0] * texture->size[1], f);

		glGenTextures(1, &texture->texture);
		glBindTexture(GL_TEXTURE_2D, texture->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture->size[0], texture->size[1], 0, GL_RED, GL_UNSIGNED_BYTE, im_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(im_data);
	}

	// vertices
	fseek(f, header.vertices.offset, SEEK_SET);
	MEMCREATEARRAY(world_vertices, sizeof(vertex_t), header.planes.size);
	MEMREADLUMP(world_vertices, f);

	// visilist
	fseek(f, header.visilist.offset, SEEK_SET);
	MEMCREATEARRAY(world_vislist, sizeof(char), header.visilist.size);
	MEMREADLUMP(world_vislist, f);

	// bsp nodes
	fseek(f, header.visilist.offset, SEEK_SET);
	MEMCREATEARRAY(world_nodes, sizeof(bspnode_t), header.nodes.size);
	MEMREADLUMP(world_nodes, f);

	// surfaces
	fseek(f, header.visilist.offset, SEEK_SET);
	MEMCREATEARRAY(world_surface, sizeof(bspnode_t), header.texinfo.size);
	MEMREADLUMP(world_surface, f);

	// faces
	fseek(f, header.visilist.offset, SEEK_SET);
	MEMCREATEARRAY(world_faces, sizeof(face_t), header.faces.size);
	MEMREADLUMP(world_faces, f);

	// lightmaps
	fseek(f, header.lightmaps.offset, SEEK_SET);
	MEMCREATEARRAY(world_lightmaps, sizeof(u_char), header.lightmaps.size);
	MEMREADLUMP(world_lightmaps, f);

	// clip nodes
	fseek(f, header.clipnodes.offset, SEEK_SET);
	MEMCREATEARRAY(world_clipnodes, sizeof(clipnode_t), header.clipnodes.size);
	MEMREADLUMP(world_clipnodes, f);

	// bsp node leaves
	fseek(f, header.leaves.offset, SEEK_SET);
	MEMCREATEARRAY(world_leaves, sizeof(dleaf_t), header.leaves.size);
	MEMREADLUMP(world_leaves, f);

	// face list
	fseek(f, header.lface.offset, SEEK_SET);
	MEMCREATEARRAY(world_lface, sizeof(u_short), header.lface.size);
	MEMREADLUMP(world_lface, f);

	// edges
	fseek(f, header.edges.offset, SEEK_SET);
	MEMCREATEARRAY(world_edges, sizeof(edge_t), header.edges.size);
	MEMREADLUMP(world_edges, f);

	// ledges
	fseek(f, header.ledges.offset, SEEK_SET);
	MEMCREATEARRAY(world_ledges, sizeof(short), header.ledges.size);
	MEMREADLUMP(world_ledges, f);

	// models
	fseek(f, header.models.offset, SEEK_SET);
	MEMCREATEARRAY(world_models, sizeof(model_t), header.models.size);
	MEMREADLUMP(world_models, f);

	return 0;
}
