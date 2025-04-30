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
	PLAYER.position = (Vector3){1.3, 5, 11};
	PLAYER.direction = Vector2Zero();
	PLAYER.angle = 0;

	PLAYER.state = PLAYER_IDLE_STATE;

}


void set_player_state(PlayerStates new_state)
{
	if(new_state == PLAYER.state) return;

	//PlayerStates last_state = PLAYER.state;
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

void process_player(float delta)
{
	/*
	switch(PLAYER.state)
	{
		case PLAYER_IDLE_STATE:	
			process_player_idle_state(delta);
			break;
		case PLAYER_WALK_STATE:
			process_player_walk_state(delta);
			break;
		case PLAYER_FALL_STATE:
			//process_player_fall_state(delta);
			break;
	}
	*/


	// :player :input
	if (IsKeyDown(KEY_A)) PLAYER.direction.x = -1;
	else if (IsKeyDown(KEY_D)) PLAYER.direction.x = 1;
	else PLAYER.direction.x = 0;
	
	if (IsKeyDown(KEY_W)) PLAYER.direction.y = 1;
	else if (IsKeyDown(KEY_S)) PLAYER.direction.y = -1;
	else PLAYER.direction.y = 0;


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
	} 
	else 
	{
		
	    moveDir = V3ZERO;
	}





	// :player :move
	PLAYER.velocity.x = float_move_toward(PLAYER.velocity.x, moveDir.x, delta * 50);
	PLAYER.velocity.z = float_move_toward(PLAYER.velocity.z, moveDir.z, delta * 50);

	Vector3 temp_position = PLAYER.position;
	PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.velocity, delta));

	RayCollision floor_ray_collision = {0};
	unsigned char on_floor = 0;
	unsigned char on_air = 0;

	for(int i = 0; i < collisions_scene_walls.len; i++)
	{
		if (!on_floor)
		{
			floor_ray_collision = GetRayCollisionBox(
									(Ray){PLAYER.position, DOWN_AXIS},
									collisions_scene_walls.items[i]
								);
			if(floor_ray_collision.hit)
			{
				//printf("DISTANCE %f\n", floor_ray_collision.distance);	
				if(floor_ray_collision.distance <= .24)
				{
					on_floor = 1;
					PLAYER.velocity.y = 0;
					PLAYER.position.y = floor_ray_collision.point.y ; 
					set_player_state(PLAYER_IDLE_STATE);	
					
				}
				else if(floor_ray_collision.distance > .5)
				{	
					if(!on_air)
					{
						on_air = 1;
						PLAYER.velocity.y = float_move_toward(PLAYER.velocity.y, PLAYER.velocity.y - delta * 25, delta * 400);
						set_player_state(PLAYER_FALL_STATE);
					}
				}
				
							
			}
		}

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

			// 4
			for(int j = 0; j < collisions_scene_walls.len; j++)
			{
				collide_temp = CheckCollisionBoxSphere(
									collisions_scene_walls.items[j], 
									(Vector3){temp_position.x, temp_position.y + .5, temp_position.z}, 0.3f 
								);
				if(collide_temp) {break;}
			}
			//5
			if(!collide_temp) {PLAYER.position = temp_position;}
			//6
			vel_temp.z = PLAYER.velocity.z;
			vel_temp.x = 0;
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, delta));
			//7
			for(int j = 0; j < collisions_scene_walls.len; j++)
			{
				collide_temp = 	CheckCollisionBoxSphere(
									collisions_scene_walls.items[j], 
									(Vector3){temp_position.x, temp_position.y + .5, temp_position.z}, 0.3f 
								);
				if(collide_temp) {break;}
			}
			//8
			if(!collide_temp) {PLAYER.position = temp_position;}

			/*
			// 1
			PLAYER.position = temp_position;
			// 2
			int collide_temp = 0;
			// 3
			Vector3 vel_temp = PLAYER.velocity;
			vel_temp.z = 0; 
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, delta));
			
			// 4
			for(int j = 0; j < collisions_scene_walls.len; j++)
			{
				collide_temp = CheckCollisionBoxSphere(
									collisions_scene_walls.items[j], 
									(Vector3){temp_position.x, temp_position.y + .5, temp_position.z}, 0.3f 
								);
				if(collide_temp) {break;}
			}
			//5
			if(!collide_temp) {PLAYER.position = temp_position;}

			//6
			vel_temp.z = PLAYER.velocity.z;
			vel_temp.x = 0;
			temp_position = Vector3Add(PLAYER.position, Vector3Scale(vel_temp, delta));
			//7
			for(int j = 0; j < collisions_scene_walls.len; j++)
			{
				collide_temp = 	CheckCollisionBoxSphere(
									collisions_scene_walls.items[j], 
									(Vector3){temp_position.x, temp_position.y + .5, temp_position.z}, 0.3f 
								);
				if(collide_temp) {break;}
			}
			//8
			if(!collide_temp) {PLAYER.position = temp_position;}

			*/
		}
		




	}

}



