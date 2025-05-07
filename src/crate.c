

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
														Vector3Add(GetMeshBoundingBox(model_crate.meshes[2]).min, position), 
														Vector3Add(GetMeshBoundingBox(model_crate.meshes[2]).max, position)
													};
		BoundingBox bounding_box_wall = (BoundingBox){
														Vector3Add(GetMeshBoundingBox(model_crate.meshes[0]).min, position), 
														Vector3Add(GetMeshBoundingBox(model_crate.meshes[0]).max, position)
													};

		CollisionBox *floor_collision_box_p = malloc(sizeof(CollisionBox));
		*floor_collision_box_p = (CollisionBox){1, bounding_box_floor};


		CollisionBox *wall_collision_box_p = malloc(sizeof(CollisionBox));
		*wall_collision_box_p = (CollisionBox){1, bounding_box_wall};


		append_CrateSlice(crates_slice, (Crate){0, floor_collision_box_p, wall_collision_box_p, position});
		append_CollisionBoxSlice(scene_floor_collisions_box, floor_collision_box_p);
		append_CollisionBoxSlice(scene_wall_collisions_box, wall_collision_box_p);

		printf("[INFO] Floor Pointer solo %p\n", floor_collision_box_p);
		printf("[INFO] Floor Pointer en Crates %p\n", crates.items[i].floor);
		printf("[INFO] Floor Pointer en Scene Floor %p\n", scene_floor_collisions.items[scene_floor_collisions.len]);

	}

}

void draw_crates()
{
	for(unsigned char i = 0; i < crates.len; i++)
		if (crates.items[i].state == 0)
			DrawMesh(model_crate.meshes[1], model_crate.materials[2],Vector3ToMatrix(crates.items[i].position));
	
}