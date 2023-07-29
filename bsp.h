#ifndef _H_BSP_H_
#define _H_BSP_H_

#include "maindef.h"

typedef struct
{
	int offset;
	int size;
} bsp_entry_t;

typedef struct {
	long  version;
	bsp_entry_t entities;
	bsp_entry_t planes;

	bsp_entry_t miptex;
	bsp_entry_t vertices;

	bsp_entry_t visilist;
	bsp_entry_t nodes;

	bsp_entry_t texinfo;

	bsp_entry_t faces;

	bsp_entry_t lightmaps;
	bsp_entry_t clipnodes;

	bsp_entry_t leaves;

	bsp_entry_t lface;
	bsp_entry_t edges;

	bsp_entry_t ledges;
	bsp_entry_t models;
} bsp_header_t;

typedef struct
{
	vec3   min;
	vec3   max;
} boundbox_t;

typedef struct
{
	short   min;
	short   max;
} bboxshort_t;

/////////////////////

typedef struct
{
	vec3 normal;
	float dist;
	int32_t type;
} plane_t;

typedef struct
{
	uint32_t    plane_id; // quake : long
	u_short front;
	u_short back;
	bboxshort_t box;
	u_short face_id;
	u_short face_num;
} bspnode_t;

typedef struct
{
	vec3   vectorS;
	scalar_t distS;
	vec3   vectorT;
	scalar_t distT;
	uint32_t   texture_id; // miptex id
	uint32_t   animated; // 0, all the time
} surface_t; // tex info

typedef struct
{
	u_short plane_id;
	u_short side;
	uint32_t first_edge;
	u_short edge_count;
	u_short texinfo_id;
	u_char light[4];
	uint32_t lightmap;
} face_t;

typedef struct
{
	int32_t plane_id;
	int16_t front;
	int16_t back;
} clipnode_t;

typedef struct
{
	int32_t type;
	int32_t vislist;
	bboxshort_t bound;
	u_short lface_id;
	u_short lface_num;
	uint8_t ambients[4];
} dleaf_t;

typedef struct
{
	uint16_t vertex[2];
} edge_t;

typedef struct
{
	boundbox_t bound;
	vec3 origin;
	int32_t node_hulls[4];
	int32_t numleafs;
	int32_t face_id;
	int32_t face_num;
} model_t;

#endif