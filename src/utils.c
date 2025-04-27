#ifndef WARK_UTILS
#define WARK_UTILS

#include "stdio.h"
#include <sys/stat.h>
#include "stdlib.h"
#include "time.h"	
#include <raylib.h>
#include <raymath.h>

typedef struct 
{
	Vector3 points[3];
} Triangle3D;

typedef struct 
{
	size_t cap;
	size_t len;
	Triangle3D *items;	
} Triangle3DSlice;

typedef struct
{
	size_t cap;
	size_t len;
	BoundingBox *items;
} BoundingBoxSlice;

void append_Triangle3DSlice(Triangle3DSlice *xs, Triangle3D x)
{
	if(xs->len >= xs->cap)
	{
		if(xs->cap == 0) xs->cap = 256;
		else xs->cap *= 2;
		xs->items = realloc(xs->items, xs->cap * sizeof(*xs->items)); 
	}
	xs->items[xs->len++] = x;	
}

void free_Triangle3DSlice(Triangle3DSlice *xs){
	if(xs->items){
		free(xs->items);
		xs->items = NULL;
	}
	xs->len = 0;
	xs->cap = 0;
}

void append_BoundingBoxSlice(BoundingBoxSlice *xs, BoundingBox x)
{
	if(xs->len >= xs->cap)
	{
		if(xs->cap == 0) xs->cap = 256;
		else xs->cap *= 2;
		xs->items = realloc(xs->items, xs->cap * sizeof(*xs->items)); 
	}
	xs->items[xs->len++] = x;	
}

void free_BoundingBoxSlice(BoundingBoxSlice *xs){
	if(xs->items){
		free(xs->items);
		xs->items = NULL;
	}
	xs->len = 0;
	xs->cap = 0;
}

void DrawTriangle3DLines(Triangle3D triangle, Color color)
{
	DrawLine3D(triangle.points[0], triangle.points[1],color);
	DrawLine3D(triangle.points[1], triangle.points[2],color);
	DrawLine3D(triangle.points[2], triangle.points[0],color);
}

int WIDTH  = 840;
int HEIGHT = 720;
int MID_WIDTH = 840/2;
int MID_HEIGHT = 720/2;


float float_rand (float min, float max){ return min + (rand() / (float) RAND_MAX) * ( max - min );}
float float_sign(float x){if (x>0) return +1.0f; else if (x < 0) return -1.0f; else return 0.0f;}
float float_move_toward(float in, float to, float delta){ if (fabsf(to - in) <= delta) return to; else return in + float_sign(to - in) * delta;}

int int_sign(int x){if (x>0) return +1; else if (x < 0) return -1; else return 0;}
int int_move_toward(int in, int to, float delta){ if (abs(to - in) <= delta) return to; else return in + int_sign(to - in) * delta;}







void load_model(Model *model, const char *file_path, const char *text_debug)
{
	Model local_model = *model;
	if(IsModelValid(local_model))
	{
		printf("[INFO] Unload a %s\n", text_debug); 
		for(int i = 1; i < local_model.materialCount; i++)
			if(IsMaterialValid(local_model.materials[i]))
			{
				unsigned int j = 0;
				while(IsTextureValid(local_model.materials[i].maps[j].texture))
				{
					if( local_model.materials[i].maps[j].texture.id >= 3) 
						UnloadTexture(local_model.materials[i].maps[j].texture);
					j += 1;
				}
			}
		UnloadModel(local_model);
	}
	printf("[INFO] Load a %s\n", text_debug);
	local_model = LoadModel(file_path);
	*model = local_model;
}

void load_scene_collisions
(
	Model *walls_model, BoundingBoxSlice *walls_collsions , const char *file_path_walls, const char *text_walls,
	Model *floors_model, Triangle3DSlice *floors_collisions , const char *file_path_floors, const char *text_floors
)
{
	free_BoundingBoxSlice(walls_collsions);
	load_model(
			walls_model, 
			file_path_walls, 
			text_walls
		);
	for (int i = 0; i < walls_model->meshCount; i++)
	{
		append_BoundingBoxSlice(
			walls_collsions,
			GetMeshBoundingBox(walls_model->meshes[i])
		);
	}

	free_Triangle3DSlice(floors_collisions);
	load_model(
			floors_model, 
			file_path_floors, 
			text_floors
		);
	for (int i = 0; i < floors_model->meshCount; i++)
	{
		if(floors_model->meshes[i].vertexCount > 3) {printf("[ERROR] Un Mesh de Floor tiene mas de 3 vertices\n"); return;}
		

		append_Triangle3DSlice(
			floors_collisions,
			(Triangle3D)
			{
				(Vector3){floors_model->meshes[i].vertices[0], floors_model->meshes[i].vertices[1], floors_model->meshes[i].vertices[2]},
				(Vector3){floors_model->meshes[i].vertices[3], floors_model->meshes[i].vertices[4], floors_model->meshes[i].vertices[5]},
				(Vector3){floors_model->meshes[i].vertices[6], floors_model->meshes[i].vertices[7], floors_model->meshes[i].vertices[8]},
			}
		);
	}
}







#endif