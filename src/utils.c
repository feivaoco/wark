#ifndef WARK_UTILS
#define WARK_UTILS

#include "stdio.h"
#include <sys/stat.h>
#include "stdlib.h"
#include "time.h"	
#include <raylib.h>
#include <raymath.h>
#include "string.h"

#define UP_AXIS (Vector3){0,1,0}
#define DOWN_AXIS (Vector3){0,-1,0}
#define V3ONE (Vector3){1,1,1}
#define V3ZERO (Vector3){0,0,0}
#define V4ZERO (Vector4){0,0,0,0}
#define MATRIXZERO (Matrix){0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,0,1}



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





typedef struct 
{
	Model model;
	ModelAnimation *model_animations;
	int anims_count;
	unsigned int anim_index;
	unsigned int anim_current_frame;
}CharacterModel;

typedef struct 
{
	CharacterModel character;
	
	float speed;
	int health;
	float angle;
	Vector2 direction;
	Vector3 last_move_direction;
	Vector3 velocity;
	Vector3 position;
	unsigned char state;
} Player;

typedef struct 
{
	int last_state;
	Player player;
	Camera3D camera;
} GameState;




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

float float_move_toward_angle(float in, float to, float delta)
{
    float diff = fmodf(to - in + 540.0f, 360.0f) - 180.0f;
    if (fabsf(diff) <= delta) return to;
    return in + float_sign(diff) * delta;
}

int int_sign(int x){if (x>0) return +1; else if (x < 0) return -1; else return 0;}
int int_move_toward(int in, int to, float delta){ if (abs(to - in) <= delta) return to; else return in + int_sign(to - in) * delta;}




void load_model_animations(CharacterModel *character, const char *file_path, const char *text_debug)
{
	character -> anims_count = character -> anims_count > 0 ? character -> anims_count : 0;

	if (character -> anims_count > 0)
	{
		printf("[INFO] MODEL ANIMATIONS Revisando para hacer unload a %s\n", text_debug);
		if (IsModelAnimationValid(character -> model, character -> model_animations[0]))
		{
			UnloadModelAnimations(character -> model_animations, character -> anims_count);
			printf("[INFO] MODEL ANIMATIONS Unload a %s\n", text_debug); 
		}
	}
	
	printf("[INFO] MODEL ANIMATIONS Load a %s\n", text_debug);	
	
	character -> anim_index = 0;
	character -> anim_current_frame = 0;
	character -> model_animations = LoadModelAnimations(file_path, &character -> anims_count);

}


void load_model(Model *model, const char *file_path, const char *text_debug)
{
	Model local_model = *model;
	printf("[INFO] Revisando para hacer unload a %s\n", text_debug);
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
	for(int i = 1; i < local_model.materialCount; i++)
		if(IsMaterialValid(local_model.materials[i]))
		{
			unsigned int j = 0;
			while(IsTextureValid(local_model.materials[i].maps[j].texture))
			{
				if( local_model.materials[i].maps[j].texture.id >= 3) 
					SetTextureFilter(local_model.materials[i].maps[j].texture, TEXTURE_FILTER_BILINEAR);
				j += 1;
			}
		}	
	*model = local_model;
}

void load_scene_collisions
(
	Model *walls_model, BoundingBoxSlice *walls_collisions , const char *file_path_walls, const char *text_walls
)
{
	free_BoundingBoxSlice(walls_collisions);
	load_model(
			walls_model, 
			file_path_walls, 
			text_walls
		);
	for (int i = 0; i < walls_model->meshCount; i++)
	{
		append_BoundingBoxSlice(
			walls_collisions,
			GetMeshBoundingBox(walls_model->meshes[i])
		);
	}
}




int get_index_animation(const char *animation_name, CharacterModel character)
{
	for(unsigned int i = 0; i < character.anims_count; i++)
	{
		if(!strcmp(animation_name, character.model_animations[i].name))
		{
			return i;
		}
	}
	printf("[ERROR] No se encontro ninguna animacion con el nombre de \"%s\"\n", animation_name);
	return 0;
}



#include "crate.c"


#endif