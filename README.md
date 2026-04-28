<p align="center">
  <img src="images/SKF_logo.png" alt="Street Kebab Fighter Logo" width="128"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-Windows-blue"/>
  <img src="https://img.shields.io/badge/license-MIT-green"/>
  <img src="https://img.shields.io/badge/language-C-darkblue"/>
</p>

# Street Kebab Fighter

> *A 2D fighting game about settling disputes the old-fashioned way with your favorite characters from "Čaršija"*

---
> [!WARNING]
> The game is still in development.

## Features
- *1 playable character* 
- *1 playable stage* 

---

## Controls

| Action | Player 1 | Player 2 |
|---|---|---|
| Jump | **W** | **UP** |
| Move Forward, Backward  | **A, D** | **LEFT, RIGHT** |
| Crouch | **S** | **DOWN** |
| Light Attack | **F** | **I** |
| Medium Attack | **G** | **O** |
| Heavy Attack | **H** | **P** |
| Block | Backward + Light Attack |  Backward + Light Attack |

---

## Platform Support

| Platform | Status |
|---|---|
| Windows | ✅ Supported |
| Linux | 🔜 Coming soon |

---

## Dependencies

| Dependency | Version |
|---|---|
| SDL2 | *2.32.6* |
| SDL2_image | *2.8.2* |
| Python (build script) | *3.13.2+* |

---

## Build

The game can be built using a Python script, but it only supports **gcc** or **clang** compilers.

**Using the build script (recommended):**
```
python build.py -comp -run
```

**Manual GCC build (gcc or clang):**
```
gcc -o SKF.exe src/*.c -I src/include -L src/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -std=c99 -Wall -Wextra -Wswitch-enum -Wconversion -Wsign-conversion -Wdouble-promotion -Wundef -Wuninitialized -Wshadow -Wnull-dereference -Wno-#warnings -pedantic -fstack-protector -O1 -mwindows -s
```
### Build options

By default, the compilation is linked dynamically.  
The *"-comp-static"* flag will link the game statically.

---

## Roadmap

- [x] *Collision and attacking systems*
- [ ] *Asset baking* - Allow compiling all assets directly into the game binary instead of loading them at runtime  
- [ ] *Linux support*

---

## Credits

- ✏️ *Art — LomimStaklo*
- ✏️ *Jojo's Bizarre Adventure Regular — Patrick H. Lauke*

---

## License

This project is licensed under the MIT License — see [LICENSE](LICENSE) for details.
