#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"

#include "dlfcn.h"

int main()
{
	void* state = NULL;
	while(true)
	{
		while(system("./build.sh wark") != 0)
		{
			fprintf(stderr, "Whoops! Failed to compile!\n");
			fprintf(stderr, "Press return to try again.\n");
			getchar();
		}
		
		void* module = dlopen("./wark_module.so", RTLD_NOW);
		
		if(module == NULL)
		{
			fprintf(stderr, "Failed to load library. (%s)\n", dlerror());
			fprintf(stderr, "Press return to try again.\n");
			getchar();
			
			// Jump back to the beginning of the loop to recompile.
			continue;
		}
		
		
		typedef void* module_main_func(void* state);

		union 
		{
		    void* obj;
		    module_main_func* func;
		} caster;

		caster.obj = dlsym(module, "module_main");
		module_main_func* module_main = caster.func;
		
		state = module_main(state);
		
		dlclose(module);
		
		if(state == NULL) return EXIT_SUCCESS;
	}
	
}