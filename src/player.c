#define JUMP_MAX_TIME 0.11
typedef enum
{
	PLAYER_IDLE_STATE = 0,
	PLAYER_WALK_STATE = 1,
	PLAYER_FALL_STATE = 2,
	PLAYER_JUMP_STATE = 3,
} PlayerStates;

unsigned int current_floor_index = 0;
unsigned int used_jumps = 0;

Vector2 ray_floor_collision_offsets[] = {
	{    0,		 .175},
	{    0, 		-.175},
	{  .175,		   0},
	{ -.175,		   0},
	{   .125,		  .125},
	{  -.125,		 -.125},
	{   .125,		 -.125},
	{  -.125,		  .125}
};	

float jump_time = 0;

unsigned int get_index_on_current_floor_collision()
{
	RayCollision tmp = {0};

	unsigned int index_floor = -99;
	float distance_min = 10000000.0f;
	for (int x = 0; x < 8; x++)
	{
		for(int i = 0; i < scene_floor_collisions.len; i++)
		{
			tmp = GetRayCollisionBox(
					(Ray){(Vector3){PLAYER.position.x + ray_floor_collision_offsets[x].x, 
							PLAYER.position.y, 
							PLAYER.position.z + ray_floor_collision_offsets[x].y 
							},
						DOWN_AXIS},
					scene_floor_collisions.items[i]
				); 
			if (tmp.hit)
			{
				if(tmp.distance < distance_min)
				{
					distance_min = tmp.distance;
					index_floor = i;
				}
			} 
		}
	}
	if(index_floor== -99) index_floor = 0;
	return index_floor;
}

const char* get_current_player_state()
{
	switch(PLAYER.state)
	{
		case PLAYER_IDLE_STATE:
			return "IDLE";
		case PLAYER_WALK_STATE:
			return "WALK";
		case PLAYER_FALL_STATE:
			return "FALL";
	}
	return "NULL";
}


void set_player_state(PlayerStates new_state)
{
	if(new_state == PLAYER.state) return;

	//PlayerStates last_state = PLAYER.state;
	PLAYER.state = new_state;

	switch(new_state)
	{
		case PLAYER_IDLE_STATE:	
			used_jumps = 0;
			PLAYER.character.anim_index = get_index_animation("Idle", PLAYER.character);
			break;
		case PLAYER_WALK_STATE:
			
			PLAYER.character.anim_index = get_index_animation("Walk", PLAYER.character);
			break;
		case PLAYER_FALL_STATE:
			PLAYER.character.anim_index = get_index_animation("Fall", PLAYER.character);
			break;
		case PLAYER_JUMP_STATE:
			used_jumps += 1;
			PLAYER.character.anim_current_frame = -1;
			PLAYER.character.anim_index = get_index_animation("Jump", PLAYER.character);
			break;
		
	}
	
}


void setup_player()
{
	PLAYER.speed = 8.0f;
	PLAYER.health = 10;
	PLAYER.velocity = V3ZERO;
	PLAYER.position = (Vector3){1.3, 5, 11};
	PLAYER.direction = Vector2Zero();
	PLAYER.angle = 0;

	PLAYER.state = 200;

	set_player_state(PLAYER_IDLE_STATE);
	current_floor_index = get_index_on_current_floor_collision();
}

void reset_player()
{
	jump_time = 0;
	PLAYER.position = (Vector3){1.3, 5, 11};
    	PLAYER.velocity = V3ZERO;
    	set_player_state(PLAYER_IDLE_STATE);
    	current_floor_index = get_index_on_current_floor_collision();
}

void reload_player()
{
	jump_time = 0;
	PLAYER.speed = 8.0f;
	PLAYER.state = 200;   
    	set_player_state(PLAYER_IDLE_STATE);
    	current_floor_index = get_index_on_current_floor_collision();
}

void process_player(float delta)
{
	


	// :player :input
	if (IsKeyDown(KEY_A)) PLAYER.direction.x = -1;
	else if (IsKeyDown(KEY_D)) PLAYER.direction.x = 1;
	else PLAYER.direction.x = 0;
	
	if (IsKeyDown(KEY_W)) PLAYER.direction.y = 1;
	else if (IsKeyDown(KEY_S)) PLAYER.direction.y = -1;
	else PLAYER.direction.y = 0;

	if(IsKeyPressed(KEY_SPACE))
	{
		if (used_jumps < 1)
			if(PLAYER.state != PLAYER_JUMP_STATE){
				PLAYER.velocity.y = 9;
				set_player_state(PLAYER_JUMP_STATE);
			}
	}
	
	if(IsKeyUp(KEY_SPACE))
		if(PLAYER.state == PLAYER_JUMP_STATE)
			if (jump_time > 0.01)
				jump_time = JUMP_MAX_TIME;
			

	// :player :directions
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

	if (Vector3Length(moveDir) > 0.01f) 
	{
		moveDir = Vector3Scale(Vector3Normalize(moveDir), PLAYER.speed);
		PLAYER.angle = float_move_toward_angle(PLAYER.angle, atan2f(moveDir.x, moveDir.z) * RAD2DEG, delta * 1000);
		PLAYER.last_move_direction = moveDir;
		if (PLAYER.state != PLAYER_FALL_STATE && PLAYER.state != PLAYER_JUMP_STATE) set_player_state(PLAYER_WALK_STATE);
	} 
	else 
	{
	    	moveDir = V3ZERO;
		if (PLAYER.state != PLAYER_FALL_STATE && PLAYER.state != PLAYER_JUMP_STATE) set_player_state(PLAYER_IDLE_STATE);
	}





	// :player :move
	PLAYER.velocity.x = float_move_toward(PLAYER.velocity.x, moveDir.x, delta * 50);
	PLAYER.velocity.z = float_move_toward(PLAYER.velocity.z, moveDir.z, delta * 50);

	Vector3 temp_position = PLAYER.position;

	PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.velocity, delta));

	
	if(PLAYER.state != PLAYER_JUMP_STATE)
	{
		unsigned char on_floor = 0;
		unsigned char on_air = 0;
		RayCollision ray_temp = {0};
		unsigned char ponderacion_floor = 0;
		check_floor:
		for (int x = 0; x < 8; x++)
		{
			ray_temp = GetRayCollisionBox(
									(Ray){(Vector3){PLAYER.position.x + ray_floor_collision_offsets[x].x, 
											PLAYER.position.y, 
											PLAYER.position.z + ray_floor_collision_offsets[x].y 
											},
										DOWN_AXIS},
									scene_floor_collisions.items[current_floor_index]
								); 
			if (ray_temp.hit)
			{
				ponderacion_floor = 1;
				if (ray_temp.distance <= 0.3f)
				{
					on_floor = 1;
					on_air = 0;
					break;
				}
				else
				{
					on_air = 1;
				}
			}
		}

		if (on_floor )
		{
			PLAYER.velocity.y = 0;
			PLAYER.position.y = float_move_toward(PLAYER.position.y, ray_temp.point.y, delta * 10);
			if (PLAYER.state != PLAYER_WALK_STATE && PLAYER.state != PLAYER_JUMP_STATE) set_player_state(PLAYER_IDLE_STATE);
		}
		else if(on_air)
		{

			PLAYER.velocity.y =  PLAYER.velocity.y - delta * 20;
			set_player_state(PLAYER_FALL_STATE);
			current_floor_index = get_index_on_current_floor_collision();
		}

		if (!ponderacion_floor){
			current_floor_index = get_index_on_current_floor_collision();
			goto check_floor;
		}
	}
	else
	{
		jump_time += dt;
		if (jump_time >= JUMP_MAX_TIME)
		{
			set_player_state(PLAYER_FALL_STATE);
			jump_time = 0;
		}
	}




	for(int i = 0; i < scene_wall_collisions.len; i++)
	{
		
		if
		(
			CheckCollisionBoxSphere(
				scene_wall_collisions.items[i], 
				(Vector3){PLAYER.position.x, PLAYER.position.y + .45, PLAYER.position.z}, .2f 
			)
		)
		{

			// 1
			PLAYER.position.x = temp_position.x;
			PLAYER.position.z = temp_position.z;
			// 2
			int collide_temp = 0;
			// 3
			Vector3 vel_temp = PLAYER.velocity;
			vel_temp.y = 0;
			vel_temp.z = 0;
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, delta));
			
			if (PLAYER.state == PLAYER_FALL_STATE)
		                if (PLAYER.direction.x == 0 && PLAYER.direction.y == 0)
		                {
		                        temp_position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.last_move_direction, delta));
		                       	
		                }

	


			// 4
			for(int j = 0; j < scene_wall_collisions.len; j++)
			{
				collide_temp = CheckCollisionBoxSphere(
									scene_wall_collisions.items[j], 
									(Vector3){temp_position.x, temp_position.y + .45, temp_position.z}, .2f 
								);
				if(collide_temp) {break;}
			}
			//5
			if(!collide_temp) {PLAYER.position = temp_position; continue;}
			//6
			vel_temp.z = PLAYER.velocity.z;
			vel_temp.x = 0;
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, delta));
			//7
			for(int j = 0; j < scene_wall_collisions.len; j++)
			{
				collide_temp = 	CheckCollisionBoxSphere(
									scene_wall_collisions.items[j], 
									(Vector3){temp_position.x, temp_position.y + .45, temp_position.z}, .2f 
								);
				if(collide_temp) {break;}
			}
			//8
			if(!collide_temp) {PLAYER.position = temp_position;}

		}
	}
}

void draw_player()
{
	DrawModelEx(
		PLAYER.character.model, 
		PLAYER.position,
		UP_AXIS,
		PLAYER.angle,
		V3ONE,
		WHITE
	);
	if(PLAYER.state == PLAYER_JUMP_STATE)
	{
		PLAYER.character.anim_current_frame = (PLAYER.character.anim_current_frame + 1);
		if(PLAYER.character.anim_current_frame >=  PLAYER.character.model_animations[PLAYER.character.anim_index].frameCount)
		{
			PLAYER.character.anim_current_frame =  PLAYER.character.model_animations[PLAYER.character.anim_index].frameCount;
		}
	}
	else
	{
		PLAYER.character.anim_current_frame = (PLAYER.character.anim_current_frame + 1)%PLAYER.character.model_animations[PLAYER.character.anim_index].frameCount;
	}
        UpdateModelAnimation(PLAYER.character.model, PLAYER.character.model_animations[PLAYER.character.anim_index], PLAYER.character.anim_current_frame );

       
}

