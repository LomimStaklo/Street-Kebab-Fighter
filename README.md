# Street Kebab Fighter
Game made in C with SDL2

## Build
Currently game supports only **gcc** and **clang** compilers.

To compile the game you have to run **bulid.py** file or in the terminal
``` 
python build.py 
```
Or if you don't have **python**:
``` 
gcc -o main.exe src/*.c -I src/include -L src/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -Wall -Wextra -Wno-#warnings -pedantic -Wshadow -std=c99 -fstack-protector -Wno-unused-parameter -O1 -s
```
