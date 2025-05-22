#include "utils.c"


GameState* game_state = NULL;

#define PLAYER (game_state->player)
#define CAMERA (game_state->camera)

Model model_map_scene = {0};

Vector3 camera_positions[2] = {
    { 0.33, 2.94, -9.72 },
    { -1.99, 7.84, 6.19 },
};

Vector3 CratesPositions[16] = {
    { -2.00, 0.50, 11.00 },
    { -2.00, 2.50, 11.00 },
    { -2.00, 1.50, 11.00 },
    { 2.00, 2.50, 4.00 },
    { -1.00, 0.50, 27.00 },
    { 1.00, 1.50, 21.00 },
    { 1.00, 0.50, 21.00 },
    { -1.00, 0.50, 29.00 },
    { -0.50, 0.50, 38.50 },
    { 0.50, 0.50, 38.50 },
    { -0.50, 1.50, 38.50 },
    { 0.50, 1.50, 38.50 },
    { -0.50, 2.50, 38.50 },
    { 0.50, 2.50, 38.50 },
    { -0.50, 3.50, 38.50 },
    { 0.50, 3.50, 38.50 },
};

unsigned char index_camera_positions = 0;

Model model_scene_wall_collisions = {0};
CollisionBoxSlice scene_wall_collisions = {0};
Model model_scene_floor_collisions = {0};
CollisionBoxSlice scene_floor_collisions = {0};

float dt;

Model model_crate_collisions = {0};
Model model_crate_character = {0};
CrateSlice crates = {0};

Model model_player_attack = {0};

#include "crate.c"
#include "player.c"



void setup_raywindow()
{	
	
	InitWindow(WIDTH,HEIGHT, "WaRk");
	SetTargetFPS(60);
	SetExitKey(0);
}


void load_assets()
{
    printf("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
    printf("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
    printf("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
    printf("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
    printf("-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
	
	load_model(&model_crate_collisions, "assets/3dmodels/props/crate_collisions.glb", "model_crate_collisions");
	

	load_model(&model_map_scene, "assets/3dmodels/scene_0/scene_0.glb", "model_map_scene");
	load_model(&PLAYER.character.model, "assets/3dmodels/char_f.glb", "model_player");
	load_model_animations(&PLAYER.character, "assets/3dmodels/char_f.glb", "model_player");

	load_model(&model_player_attack, "assets/3dmodels/char_attack.glb", "model_player_attack");

	load_scene_collisions(
		&model_scene_wall_collisions, 
		&scene_wall_collisions, 
		"assets/3dmodels/scene_0/scene_0_wall_collisions.glb",
		"model_scene_wall_collisions"
		
	);
	load_scene_collisions(
		&model_scene_floor_collisions, 
		&scene_floor_collisions, 
		"assets/3dmodels/scene_0/scene_0_floor_collisions.glb",
		"model_scene_floor_collisions"
		
	);
}


void setup()
{
	
	SetTraceLogLevel(LOG_NONE);


	setup_raywindow();
	srand((unsigned long)time(NULL));

	load_assets();

	setup_crates(&crates, CratesPositions, 16, &scene_floor_collisions, &scene_wall_collisions);

	setup_player();

	CAMERA = (Camera3D){ 0 };
    CAMERA.position = camera_positions[index_camera_positions];  // CAMERA position
    CAMERA.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // CAMERA looking at point
    CAMERA.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // CAMERA up vector (rotation towards target)
    CAMERA.fovy = 30.0f;                                // CAMERA field-of-view Y
    CAMERA.projection = CAMERA_PERSPECTIVE;             // CAMERA mode type
}

void reset()
{

	reset_player();
	for(size_t i = 0; i < crates.len; i++)
		restore_crate(&crates.items[i]);

}

int process()
{
	dt = GetFrameTime();
	
	process_player(dt);
	
	if(IsKeyPressed(KEY_UP)) {index_camera_positions = (index_camera_positions+1)%2; CAMERA.position = camera_positions[index_camera_positions];}
	

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
                draw_crates();
                
              	//for(int i = 0; i < scene_wall_collisions.len; i++){DrawBoundingBox(scene_wall_collisions.items[i].box, ORANGE);}
                //DrawModel(model_scene_floor_collisions, Vector3Zero(), 1.0f, WHITE);
                //DrawModel(model_scene_wall_collisions, Vector3Zero(), 1.0f, WHITE);
                


                // PLAYER
                draw_player();

                //for(int i = 0; i < 8; i++)DrawRay((Ray){(Vector3){PLAYER.position.x + ray_floor_collision_offsets[i].x, PLAYER.position.y + 1, PLAYER.position.z + ray_floor_collision_offsets[i].y },DOWN_AXIS}, GREEN);
				//DrawSphere((Vector3){PLAYER.position.x, PLAYER.position.y + .45, PLAYER.position.z}, .2f  , RED );     

				       
                
        EndMode3D();	
       	//DrawFPS(10, 10);
       	//DrawText(TextFormat(" %d , %d ", model_map_scene.materialCount, model_map_scene.meshCount), 30,30,42,BLACK);
		//DrawText(TextFormat(" %f , %f , %f", PLAYER.position.x, PLAYER.position.y, PLAYER.position.z ), 30,30,42,BLACK);
		//DrawText(TextFormat(" %s ", get_current_player_state() ), 30,70,42,BLACK);



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
		setup_crates(&crates, CratesPositions, 16, &scene_floor_collisions, &scene_wall_collisions);
		reload_player();
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
