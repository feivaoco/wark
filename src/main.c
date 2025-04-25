#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"

#if defined(_WINDOWS)
	#include "windows.h"
#elif defined(_LINUX)
	#include "dlfcn.h"
#endif


int main()
{
    void* state = NULL;

    while (true)
    {
#if defined(_WINDOWS)
        if (system(".\\build.bat wark") != 0)
#elif defined(_LINUX)
        if (system("./build.sh wark") != 0)
#endif
        {
            fprintf(stderr, "Error en Compilacion de libreria\n");
            fprintf(stderr, "Presiona una tecla para reintentar.\n");
            getchar();
            continue;
        }

#if defined(_WINDOWS)
        HMODULE module = LoadLibraryA("wark_module.dll");
        if (module == NULL)
        {
            fprintf(stderr, "Falla en Cargar la libreria. (Error %lu)\n", GetLastError());
            fprintf(stderr, "Presiona una tecla para reintentar.\n");
            getchar();
            continue;
        }
        typedef void* (__cdecl *module_main_func)(void* state);
        module_main_func module_main = (module_main_func)GetProcAddress(module, "module_main");
        if (module_main == NULL)
        {
            fprintf(stderr, "Falla en encontrar la funcion 'module_main'. (Error %lu)\n", GetLastError());
            fprintf(stderr, "Presiona una tecla para reintentar.\n");
            FreeLibrary(module);
            getchar();
            continue;
        }
#elif defined(_LINUX)
        void* module = dlopen("./wark_module.so", RTLD_NOW);
        if (module == NULL)
        {
            fprintf(stderr, "Falla en Cargar la libreria. (%s)\n", dlerror());
            fprintf(stderr, "Presiona una tecla para reintentar.\n");
            getchar();
            continue;
        }
        typedef void* module_main_func(void* state);
        union { void* obj; module_main_func* func; } caster;
        caster.obj = dlsym(module, "module_main");
        module_main_func* module_main = caster.func;
        if (module_main == NULL)
        {
            fprintf(stderr, "Falla en encontrar la funcion 'module_main'. (Error %s)\n", dlerror());
            fprintf(stderr, "Presiona una tecla para reintentar.\n");
            dlclose(module);
            getchar();
            continue;
        }
#endif

        state = module_main(state);

#if defined(_WINDOWS)
        FreeLibrary(module);
#elif defined(_LINUX)
        dlclose(module);
#endif

        if (state == NULL){ printf("[EXIT] Cerrando Programa exitosamente\n"); return EXIT_SUCCESS;}
        
        printf("[RELOAD] Recargando en Caliente\n");
    }
}