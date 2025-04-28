#include "utils.c"

typedef struct 
{
	float speed;
	int health;
	float angle;
	Vector2 direction;
	Vector3 velocity;
	Vector3 position;
} Player;

typedef struct 
{
	int last_state;
	Player player;
	Camera3D camera;
} GameState;

static GameState* game_state = NULL;

#define PLAYER (game_state->player)
#define CAMERA (game_state->camera)

Model model_map_scene = {0};
Model model_player = {0};

Model model_collisions_scene_walls = {0};
BoundingBoxSlice collisions_scene_walls = {0};
Model model_collisions_scene_floors = {0};
Triangle3DSlice collisions_scene_floors = {0};

float dt;

void setup_raywindow()
{	
	
	InitWindow(WIDTH,HEIGHT, "WaRk");
	SetTargetFPS(60);
	SetExitKey(0);
}


void load_assets()
{
	load_model(&model_map_scene, "assets/3dmodels/scene_0.glb", "model_map_scene");
	load_model(&model_player, "assets/3dmodels/char_f.glb", "model_player");
	
	load_scene_collisions(
			&model_collisions_scene_walls, 
			&collisions_scene_walls, 
			"assets/3dmodels/collisions/scene_0_walls_collisions.glb",
			"model_collisions_scene_walls",
			&model_collisions_scene_floors, 
			&collisions_scene_floors, 
			"assets/3dmodels/collisions/scene_0_floors_collisions.glb",
			"model_collisions_scene_floors"
		);
}


void setup()
{
	
	setup_raywindow();
	srand((unsigned long)time(NULL));

	load_assets();

	PLAYER.speed = 10.0f;
	PLAYER.health = 10;
	PLAYER.velocity = Vector3Zero();
	PLAYER.position = (Vector3){0, 0.0f, 0};
	PLAYER.direction = Vector2Zero();
	PLAYER.angle = 0;

	CAMERA = (Camera3D){ 0 };
    CAMERA.position = (Vector3){ -15, 20.0f, 15.0f };  // CAMERA position
    CAMERA.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // CAMERA looking at point
    CAMERA.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // CAMERA up vector (rotation towards target)
    CAMERA.fovy = 20.0f;                                // CAMERA field-of-view Y
    CAMERA.projection = CAMERA_PERSPECTIVE;             // CAMERA mode type
}

void reset()
{
	//PLAYER.speed = 10.0f;
	PLAYER.angle = 0; 
	PLAYER.position = (Vector3){0, 0.0f, 0};
    CAMERA.position = (Vector3){ -15, 20.0f, 15.0f };
}

void process_collide_walls(float delta, Vector3 temp_position)
{
	// Sphere (Vector3){PLAYER.position.x, PLAYER.position.y + .5, PLAYER.position.z}, 0.3f
	for(int i = 0; i < collisions_scene_walls.len; i++)
		if
		(
			CheckCollisionBoxSphere(
				collisions_scene_walls.items[i], 
				(Vector3){PLAYER.position.x, PLAYER.position.y + .5, PLAYER.position.z}, 0.3f 
			)
		)
		{
			PLAYER.position = temp_position;
			int collide_temp = 0;
			Vector3 vel_temp = PLAYER.velocity;
			
			vel_temp.z = 0; 
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, dt));
			for(int j = 0; j < collisions_scene_walls.len; j++)
			{
				collide_temp = CheckCollisionBoxSphere(
									collisions_scene_walls.items[j], 
									(Vector3){temp_position.x, temp_position.y + .5, temp_position.z}, 0.3f 
								);
				if(collide_temp) {break;}
			}
			if(!collide_temp) {PLAYER.position = temp_position; return;}

			

			vel_temp.z = PLAYER.velocity.z;
			vel_temp.x = 0;
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, dt));
			for(int j = 0; j < collisions_scene_walls.len; j++)
			{
				collide_temp = CheckCollisionBoxSphere(
									collisions_scene_walls.items[j], 
									(Vector3){temp_position.x, temp_position.y + .5, temp_position.z}, 0.3f 
								);
				if(collide_temp) {return;}
			}
			if(!collide_temp) {PLAYER.position = temp_position; return;}
		}
}

int process()
{
	dt = GetFrameTime();
	
	if (IsKeyDown(KEY_A)) PLAYER.direction.x = -1;
	else if (IsKeyDown(KEY_D)) PLAYER.direction.x = 1;
	else PLAYER.direction.x = 0;
	
	if (IsKeyDown(KEY_W)) PLAYER.direction.y = 1;
	else if (IsKeyDown(KEY_S)) PLAYER.direction.y = -1;
	else PLAYER.direction.y = 0;

	Vector3 forward = Vector3Normalize((Vector3){
	    CAMERA.target.x - CAMERA.position.x,
	    0.0f,
	    CAMERA.target.z - CAMERA.position.z
	});

	Vector3 right = (Vector3){ -forward.z, 0.0f, forward.x };

	Vector3 moveDir = Vector3Add(
	    Vector3Scale(right, PLAYER.direction.x),
	    Vector3Scale(forward, PLAYER.direction.y)
	);
	
	if (Vector3Length(moveDir) > 0.01f) {
	    moveDir = Vector3Scale(Vector3Normalize(moveDir), PLAYER.speed);
	    PLAYER.angle = float_move_toward_angle(PLAYER.angle, atan2f(moveDir.x, moveDir.z) * RAD2DEG, dt * 500);
	} else {
	    moveDir = Vector3Zero();
	}

	PLAYER.velocity.x = float_move_toward(PLAYER.velocity.x, moveDir.x, dt * 50);
	PLAYER.velocity.z = float_move_toward(PLAYER.velocity.z, moveDir.z, dt * 50);
	

	Vector3 temp_position = PLAYER.position;
	PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.velocity, dt));
	process_collide_walls(dt, temp_position);
	

	CAMERA.target = PLAYER.position;
	UpdateCamera(&CAMERA, CAMERA_PERSPECTIVE);

	if (IsKeyPressed(KEY_F2)) load_assets(); // Recarga los modelos 3d
	if (IsKeyPressed(KEY_F11)) reset(); // Resetear valores
	if (IsKeyPressed(KEY_F1)) return 1; // Hotreload sin resetar valores
	return 0;
}

void draw()
{
	BeginDrawing();
		ClearBackground(WHITE);	
		BeginMode3D(CAMERA);        
				//MAP
                
                DrawModel(model_map_scene, Vector3Zero(), 1.0f, WHITE);
                
                for(int i = 0; i < collisions_scene_walls.len; i++){DrawBoundingBox(collisions_scene_walls.items[i], ORANGE);}
                for(int i = 0; i < collisions_scene_floors.len; i++){ DrawTriangle3DLines(collisions_scene_floors.items[i], GREEN);}

               

                //DrawModel(model_collisions_scene_walls, Vector3Zero(), 1.0f, WHITE);
                
				//PLAYER
				//DrawModel(model_player, PLAYER.position, 1.0f, WHITE);
				DrawModelEx(
					model_player, 
					PLAYER.position,
					UP_AXIS,
					PLAYER.angle,
					V3ONE,
					WHITE
				);            
                
        EndMode3D();	
        DrawFPS(10, 10);
        //DrawText(TextFormat(" %d , %d ", model_map_scene.materialCount, model_map_scene.meshCount), 30,30,42,BLACK);

    EndDrawing();
}

void* wark_main(void* state)
{
	game_state = state;

    if (game_state == NULL) {
        printf("[INIT] Inicializando Programa.\n");			
        game_state = calloc(1, sizeof(*game_state));
        game_state->last_state = 0;
        setup();
    } 
    else {
    	//( ￣ー￣)φ__
    	setup_raywindow();
		load_assets();
        printf("[RELOAD] Recarga en Caliente exitosa\n");
    }

    int recompile = 0;

    while (!WindowShouldClose())
    {
        recompile = process();
        draw();
        if (recompile) 
			{
				CloseWindow(); return game_state;
			}
    }

    CloseWindow();
    printf("[EXIT] ADIOS\n");
    return NULL;
}

#if defined(_WINDOWS)
    #define DLL_EXPORT __declspec(dllexport)
	extern DLL_EXPORT void* module_main(void* state){return wark_main(state);}    
#elif defined(_LINUX)
	void* module_main(void* state){return wark_main(state);}    
#endif
