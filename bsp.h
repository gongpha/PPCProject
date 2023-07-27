#ifndef _H_BSP_H_
#define _H_BSP_H_

#include "maindef.h"

typedef struct
{
	long  offset;
	long  size;
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
	scalar_t x;
	scalar_t y;
	scalar_t z;
} vec3_t;

typedef struct
{
	vec3_t   min;
	vec3_t   max;
} boundbox_t;

typedef struct
{
	short   min;
	short   max;
} bboxshort_t;

/////////////////////

typedef struct
{
	vec3_t normal;
	scalar_t dist;
	long    type;
} plane_t;

typedef struct
{
	char   name[16];
	u_long width;
	u_long height;
	u_long offset1;

	// unused mipmaps (opengl is my best friend <3)
	u_long offset2;
	u_long offset4;
	u_long offset8;
} miptex_t;

typedef struct
{
	// actually ints but idk why they preferred float numbers
	float x;
	float y;
	float z;
} vertex_t;

typedef struct
{
	long    plane_id;
	u_short front;
	u_short back;
	bboxshort_t box;
	u_short face_id;
	u_short face_num;
} bspnode_t;

typedef struct
{
	vec3_t   vectorS;
	scalar_t distS;
	vec3_t   vectorT;
	scalar_t distT;
	u_long   texture_id;
	u_long   animated;
} surface_t;

typedef struct
{
	u_short plane_id;
	u_short side;
	long ledge_id;
	u_short ledge_num;
	u_short texinfo_id;
	u_char typelight;
	u_char baselight;
	u_char light[2];
	long lightmap;
} face_t;

typedef struct
{
	u_long planenum;
	short front;
	short back;
} clipnode_t;

typedef struct
{
	long type;
	long vislist;
	bboxshort_t bound;
	u_short lface_id;
	u_short lface_num;
	u_char sndwater; // change ?
	u_char sndsky; // change ?
	u_char sndslime; // change ?
	u_char sndlava; // change ?
} dleaf_t;

typedef struct
{
	u_short vertex0;
	u_short vertex1;
} edge_t;

typedef struct
{
	boundbox_t bound;
	vec3_t origin;
	long node_id0;
	long node_id1;
	long node_id2;
	long node_id3;
	long numleafs;
	long face_id;
	long face_num;
} model_t;

//

typedef struct
{
	float x;
	float y;
	float z;
} vector_t;

#endif