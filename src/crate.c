

void append_CrateSlice(CrateSlice *xs, Crate x)
{
	if(xs->len >= xs->cap)
	{
		if(xs->cap == 0) xs->cap = 256;
		else xs->cap *= 2;
		xs->items = realloc(xs->items, xs->cap * sizeof(*xs->items)); 
	}
	xs->items[xs->len++] = x;	
}

void free_CrateSlice(CrateSlice *xs){
	if(xs->items){
		for(unsigned char i = 0; i < xs -> len; i++)
		{
			if(xs->items[i].character)
				{
					UnloadModelAnimations(xs->items[i].character->model_animations, xs->items[i].character->anims_count);
					free(xs->items[i].character);
				}
			if(xs->items[i].floor) free(xs->items[i].floor);
			if(xs->items[i].wall) free(xs->items[i].wall);
		}
		free(xs->items);
		xs->items = NULL;
	}
	xs->len = 0;
	xs->cap = 0;
}



void setup_crates(CrateSlice *crates_slice, Vector3 crates_positions[], unsigned char crates_count, CollisionBoxSlice *scene_floor_collisions_box, CollisionBoxSlice *scene_wall_collisions_box)
{
	free_CrateSlice(crates_slice);
	for(unsigned char i = 0; i < crates_count; i++)
	{	
		Vector3 position = crates_positions[i];


		BoundingBox bounding_box_floor = (BoundingBox){
														Vector3Add(GetMeshBoundingBox(model_crate_collisions.meshes[1]).min, position), 
														Vector3Add(GetMeshBoundingBox(model_crate_collisions.meshes[1]).max, position)
													};
		BoundingBox bounding_box_wall = (BoundingBox){
														Vector3Add(GetMeshBoundingBox(model_crate_collisions.meshes[0]).min, position), 
														Vector3Add(GetMeshBoundingBox(model_crate_collisions.meshes[0]).max, position)
													};

		CollisionBox *floor_collision_box_p = malloc(sizeof(CollisionBox));
		*floor_collision_box_p = (CollisionBox){1, bounding_box_floor};


		CollisionBox *wall_collision_box_p = malloc(sizeof(CollisionBox));
		*wall_collision_box_p = (CollisionBox){1, bounding_box_wall};


		CharacterModel *crate_character = malloc(sizeof(CharacterModel));
		*crate_character = (CharacterModel){0};
		load_model(&crate_character->model, "assets/3dmodels/props/crate_character.glb", "model_crate_character");
	
		load_model_animations(crate_character, "assets/3dmodels/props/crate_character.glb", "model_crate_character");

		append_CrateSlice(crates_slice, (Crate)
			{
				crate_character,
				0, floor_collision_box_p, wall_collision_box_p, position
			}
		);
		append_CollisionBoxSlice(scene_floor_collisions_box, floor_collision_box_p);
		append_CollisionBoxSlice(scene_wall_collisions_box, wall_collision_box_p);

		//printf("[INFO] Floor Pointer solo %p\n", floor_collision_box_p);
		//printf("[INFO] Floor Pointer en Crates %p\n", crates.items[i].floor);
		//printf("[INFO] Floor Pointer en Scene Floor %p\n", scene_floor_collisions.items[scene_floor_collisions.len-1]);

	}

}

void draw_crates()
{
	for(unsigned char i = 0; i < crates.len; i++)
	{
		switch(crates.items[i].state)
		{
			case 0:
				DrawModel(crates.items[i].character->model, crates.items[i].position, 1, WHITE);
				//DrawMesh(model_crate_collisions.meshes[1], model_crate_character->materials[1],Vector3ToMatrix(crates.items[i].position));
				break;
			case 1:
				crates.items[i].character->anim_current_frame = crates.items[i].character->anim_current_frame + 1;
				if(crates.items[i].character->anim_current_frame >  crates.items[i].character->model_animations[crates.items[i].character->anim_index].frameCount-5)
				{
					crates.items[i].character->anim_current_frame =  crates.items[i].character->model_animations[crates.items[i].character->anim_index].frameCount-5;
					crates.items[i].state = 2;
				}
				else
				{
			        UpdateModelAnimation(crates.items[i].character->model, crates.items[i].character->model_animations[0], crates.items[i].character->anim_current_frame );
					DrawModel(crates.items[i].character->model, crates.items[i].position, 1, WHITE);
				}
				//crates.items[i].character->anim_current_frame = (crates.items[i].character->anim_current_frame + 1)%crates.items[i].character->model_animations[crates.items[i].character->anim_index].frameCount;
				break;
			case 2:
				break;
		}
	}
}