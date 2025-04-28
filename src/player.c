typedef enum
{
	PLAYER_IDLE_STATE = 0,
	PLAYER_WALK_STATE = 1,
	PLAYER_FALL_STATE = 2,
} PlayerStates;



void setup_player()
{
	PLAYER.speed = 10.0f;
	PLAYER.health = 10;
	PLAYER.velocity = Vector3Zero();
	PLAYER.position = (Vector3){2, 2, 11};
	PLAYER.direction = Vector2Zero();
	PLAYER.angle = 0;
	PLAYER.state = PLAYER_IDLE_STATE;
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
				collide_temp = CheckCollisionBoxSphere(
									collisions_scene_walls.items[j], 
									(Vector3){temp_position.x, temp_position.y + .5, temp_position.z}, 0.3f 
								);
				if(collide_temp) {return;}
			}
			if(!collide_temp) {PLAYER.position = temp_position; return;}
		}
}







void enter_state(PlayerStates which)
{

}

void procces_player_idle_state(float delta)
{

}

void procces_player_walk_state(float delta)
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
	    moveDir = Vector3Zero();
	}

	PLAYER.velocity.x = float_move_toward(PLAYER.velocity.x, moveDir.x, delta * 50);
	PLAYER.velocity.z = float_move_toward(PLAYER.velocity.z, moveDir.z, delta * 50);

	Vector3 temp_position = PLAYER.position;
	PLAYER.position = Vector3Add(PLAYER.position, Vector3Scale(PLAYER.velocity, delta));
	process_player_collide_walls(delta, temp_position);

}


void process_player(float delta)
{
	procces_player_idle_state(delta);
}