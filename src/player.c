typedef enum
{
	PLAYER_IDLE_STATE = 0,
	PLAYER_WALK_STATE = 1,
	PLAYER_FALL_STATE = 2,
} PlayerStates;

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

void setup_player()
{
	PLAYER.speed = 10.0f;
	PLAYER.health = 10;
	PLAYER.velocity = V3ZERO;
	PLAYER.position = (Vector3){2, 3, 11};
	PLAYER.direction = Vector2Zero();
	PLAYER.angle = 0;
	PLAYER.state = PLAYER_IDLE_STATE;
}


unsigned char process_player_is_on_floor(Vector3 position)
{
	RayCollision t_rc = {0};
	
	for(int i = 0; i < collisions_scene_floors.len; i++)
	{
		t_rc = GetRayCollisionTriangle(
								(Ray){position, DOWN_AXIS},
								collisions_scene_floors.items[i].points[0],
								collisions_scene_floors.items[i].points[1],
								collisions_scene_floors.items[i].points[2]
							);
		if(t_rc.hit)
		{
			if(t_rc.distance <= .2)
			{
				
				return 1;
			}
			else if(t_rc.distance > .5)
			{	
				return 0;
			}
		}		
	}
	
	return 0;
}

void process_player_collide_walls(float delta, Vector3 temp_position)
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
			if (PLAYER.state == PLAYER_FALL_STATE)
				if (PLAYER.direction.x == 0 && PLAYER.direction.x == 0)
				{
					PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.last_move_direction, delta));
					return;
				}

			PLAYER.position = temp_position;
			int collide_temp = 0;
			Vector3 vel_temp = PLAYER.velocity;
			
			vel_temp.z = 0; 
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, delta));
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
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, delta));
			for(int j = 0; j < collisions_scene_walls.len; j++)
			{
				collide_temp = 	CheckCollisionBoxSphere(
									collisions_scene_walls.items[j], 
									(Vector3){temp_position.x, temp_position.y + .5, temp_position.z}, 0.3f 
								);
				if(collide_temp) {return;}
			}
			if(!collide_temp) {PLAYER.position = temp_position; return;}
		}
}

void player_enter_state(PlayerStates new_state)
{
	PlayerStates last_state = PLAYER.state;
	PLAYER.state = new_state;

	switch(new_state)
	{
		case PLAYER_IDLE_STATE:	
			
			break;
		case PLAYER_WALK_STATE:
			
			break;
		case PLAYER_FALL_STATE:
			
			break;
	}
}

void process_player_idle_state(float delta)
{

	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S))
		{player_enter_state(PLAYER_WALK_STATE); return;}
	
	PLAYER.velocity.x = float_move_toward(PLAYER.velocity.x, 0, delta * 70);
	PLAYER.velocity.z = float_move_toward(PLAYER.velocity.z, 0, delta * 70);

	Vector3 temp_position = PLAYER.position;
	PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.velocity, delta));
	process_player_collide_walls(delta, temp_position);
	if (!process_player_is_on_floor( PLAYER.position))
		player_enter_state(PLAYER_FALL_STATE);
}

void process_player_walk_state(float delta)
{
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
	    PLAYER.angle = float_move_toward_angle(PLAYER.angle, atan2f(moveDir.x, moveDir.z) * RAD2DEG, delta * 1000);
	} else {
	    moveDir = V3ZERO;
	    {player_enter_state(PLAYER_IDLE_STATE); return;}
	}

	PLAYER.velocity.x = float_move_toward(PLAYER.velocity.x, moveDir.x, delta * 50);
	PLAYER.velocity.z = float_move_toward(PLAYER.velocity.z, moveDir.z, delta * 50);

	Vector3 temp_position = PLAYER.position;
	PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.velocity, delta));
	process_player_collide_walls(delta, temp_position);
	if (!process_player_is_on_floor(PLAYER.position))
		player_enter_state(PLAYER_FALL_STATE);
	
}

void process_player_fall_state(float delta)
{
	
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
	
	if (Vector3Length(moveDir) > 0.01f) 
	{
	    moveDir = Vector3Scale(Vector3Normalize(moveDir), PLAYER.speed);
	    PLAYER.angle = float_move_toward_angle(PLAYER.angle, atan2f(moveDir.x, moveDir.z) * RAD2DEG, delta * 1000);
		PLAYER.last_move_direction = moveDir;
	} 
	else 
	{
	    moveDir = V3ZERO;
	    ////////////////////////////////////////////////
	    ////////////////////////////////////////////////
	}

	

	PLAYER.velocity.x = float_move_toward(PLAYER.velocity.x, moveDir.x, delta * 50);
	PLAYER.velocity.z = float_move_toward(PLAYER.velocity.z, moveDir.z, delta * 50);

	////////////////////////////////////////////////
	PLAYER.velocity.y = float_move_toward(PLAYER.velocity.y, PLAYER.velocity.y - .31, delta * 500);
	////////////////////////////////////////////////

	Vector3 temp_position = PLAYER.position;


	PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.velocity, delta));
	process_player_collide_walls(delta, temp_position);


	////////////////////////////////////////////////
	if(process_player_is_on_floor( PLAYER.position))
	{
		PLAYER.position.y = PLAYER.position.y + .01;
		PLAYER.velocity.y = 0;
		player_enter_state(PLAYER_IDLE_STATE);
		return;
	}
	////////////////////////////////////////////////
	
}

void process_player(float delta)
{
	switch(PLAYER.state)
	{
		case PLAYER_IDLE_STATE:	
			process_player_idle_state(delta);
			break;
		case PLAYER_WALK_STATE:
			process_player_walk_state(delta);
			break;
		case PLAYER_FALL_STATE:
			process_player_fall_state(delta);
			break;
	}
}



