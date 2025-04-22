#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#include "utils.c"

typedef struct 
{
	int health;
	Vector2 direction;
	Vector2 velocity;
	Vector2 position;
} Player;

typedef struct 
{
	int last_state;
	Player player;
} GameState;



static GameState* game_state = NULL;
#define PLAYER (game_state->player)

float dt;

void setup()
{
	PLAYER.health = 10;
	PLAYER.velocity = Vector2Zero();
	PLAYER.position = (Vector2){MID_WIDTH, MID_HEIGHT};
	PLAYER.direction = Vector2Zero();
	InitWindow(WIDTH,HEIGHT, "WaRk");
	SetTargetFPS(60);
	SetExitKey(0);
	srand((unsigned long)time(NULL));
}


int process()
{
	dt = GetFrameTime();
	
	if (IsKeyDown(KEY_A)) PLAYER.velocity.x = -500;
	else if (IsKeyDown(KEY_D)) PLAYER.velocity.x = 500;
	else PLAYER.velocity.x = 0;
	
	if (IsKeyDown(KEY_W)) PLAYER.velocity.y = -500;
	else if (IsKeyDown(KEY_S)) PLAYER.velocity.y = 500;
	else PLAYER.velocity.y = 0;

	
	//float_move_toward(PLAYER.velocity.x, PLAYER.position.x + 100, , dt * 100);
	
	PLAYER.position = Vector2Add(PLAYER.position, Vector2Scale(PLAYER.velocity, dt));



	if (IsKeyPressed(KEY_ENTER)) return 1;
	return 0;
}

void draw()
{
	BeginDrawing();
		ClearBackground(WHITE);	
		DrawCircle(PLAYER.position.x, PLAYER.position.y, 10, BLUE);
	EndDrawing();
}

void* module_main(void* saved_state)
{
	game_state = saved_state;
	
	if(game_state == NULL)
	{
		game_state = calloc(1, sizeof(*game_state));
		printf("[INIT] Inicializando Programa.\n");			
		game_state->last_state = 0;
		setup();
	} 
	else 
	{
		printf("[RELOAD] Recarga en Caliente\n");
	}

	int recompile = 0;

	while(!WindowShouldClose())
	{
		recompile = process();
		draw();

		if(recompile) return game_state; 
	}
	
	CloseWindow();

	// Returning NULL tells the hot-load loop to just exit the application instead.
	return NULL;
}