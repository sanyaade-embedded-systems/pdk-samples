arm-none-linux-gnueabi-g++ -o tiltodemo ..\common\accelerometer.cpp ..\common\collision.cpp ..\common\gamelogic.cpp ..\common\geometry.cpp ..\common\graphics_ogl.cpp ..\common\graphics_sdl.cpp ..\common\main.cpp ..\common\sdl_init.cpp ..\common\sound.cpp "-I%PalmPDK%\include" "-I%PalmPDK%\include\SDL" "-L%PalmPDK%\device\lib" -Wl,--allow-shlib-undefined -lSDL -lSDL_net -lSDL_image -lSDL_mixer -lpdl -lGLES_CM



