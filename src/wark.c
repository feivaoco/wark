#include "utils.c"

typedef struct 
{
	float speed;
	int health;
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
BoundingBox bounding_box_mesh_floor = {0};

float dt;

void setup_raywindow()
{
	InitWindow(WIDTH,HEIGHT, "WaRk");
	SetTargetFPS(60);
	SetExitKey(0);
}


Model load_model(Model model, const char *file_path, const char *text_debug)
{
	if(IsModelValid(model))
	{
		printf("[INFO] Unload a %s\n", text_debug); 
		for(int i = 1; i < model.materialCount; i++)
			if(IsMaterialValid(model.materials[i]))
			{
				unsigned int j = 0;
				while(IsTextureValid(model.materials[i].maps[j].texture))
				{
					if( model.materials[i].maps[j].texture.id >= 3) 
						UnloadTexture(model.materials[i].maps[j].texture);
					j += 1;
				}
			}
		UnloadModel(model);
	}
	printf("[INFO] Load a %s\n", text_debug);
	return LoadModel(file_path);
}


void load_assets()
{
	model_map_scene = load_model(model_map_scene, "assets/3dmodels/scene_0.glb", "model_map_scene");
	model_player = load_model(model_player, "assets/3dmodels/char_f.glb", "model_player");
	bounding_box_mesh_floor = GetMeshBoundingBox(model_map_scene.meshes[0]);
}

void setup()
{
	setup_raywindow();
	srand((unsigned long)time(NULL));

	load_assets();

	PLAYER.speed = 10.0f;
	PLAYER.health = 10;
	PLAYER.velocity = Vector3Zero();
	PLAYER.position = (Vector3){0, 1.0f, 0};
	PLAYER.direction = Vector2Zero();

	CAMERA = (Camera3D){ 0 };
    CAMERA.position = (Vector3){ 15, 10.0f, 15.0f };  // CAMERA position
    CAMERA.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // CAMERA looking at point
    CAMERA.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // CAMERA up vector (rotation towards target)
    CAMERA.fovy = 45.0f;                                // CAMERA field-of-view Y
    CAMERA.projection = CAMERA_PERSPECTIVE;             // CAMERA mode type
}

void reset()
{
	//PLAYER.speed = 10.0f;
	PLAYER.position = (Vector3){0, 1.0f, 0};
    CAMERA.position = (Vector3){ 15, 10.0f, 15.0f };
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
	} else {
	    moveDir = Vector3Zero();
	}

	PLAYER.velocity.x = float_move_toward(PLAYER.velocity.x, moveDir.x, dt * 500);
	PLAYER.velocity.z = float_move_toward(PLAYER.velocity.z, moveDir.z, dt * 500);
	

	Vector3 last_position = PLAYER.position;
	PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.velocity, dt));

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
                
				//PLAYER
				DrawModel(model_player, PLAYER.position, 1.0f, WHITE);            
                DrawBoundingBox(bounding_box_mesh_floor, GREEN);
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
			{CloseWindow(); return game_state;}
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
