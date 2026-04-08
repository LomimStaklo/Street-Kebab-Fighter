from os import system
from sys import argv
from time import time
from platform import system as get_os_name

# STREET KEBAB FIGHTER

#-static-libgcc -Wl,-Bstatic 
#-Wl,-Bdynamic 
#-luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid 
#-lfreetype -lpng -lz -ljpeg -lwebp -lvorbisfile -lvorbis -logg -lmodplug

#flags += " -mwindows -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 
# -lole32 -loleaut32 -lshell32 -lversion -luuid -static-libgcc"

# ====== CONFIG ======= #
cc       = "clang" # clang / gcc
src      = "src/*.c"
flags    = "-std=c99 -Wall -Wextra -Wno-#warnings -Wshadow -pedantic -fstack-protector -O1" # -mwindows
lib_path = "src/lib"
inc_path = "src/include"
# ====== CONFIG ======= #

# ====== OS ======= #
os_name = get_os_name() 
out     = "SKF"
libs    = ""
cmd_run = ""

match os_name:
    case "Windows":
        out  += ".exe"
        libs = "-lmingw32 -lSDL2main -lSDL2 -lSDL2_image"
        cmd_run = f".\\{out}"
    case "Linux":
        libs = "-lSDL2 -lSDL2_image"
        cmd_run = f"./{out}"
    case _:
        print(f"[!] Wrong operating system {os_name}")
        exit(1)
# ====== OS ======= #

# ====== VARIABLES ======= # 
cmd_comp  = f"{cc} -o {out} {src} -I {inc_path} -L {lib_path} {libs} {flags}"
argc      = len(argv)
do_run    = False
do_comp   = False
dbg_info  = False
# ====== VARIABLES ======= # 

def run_time(func) -> None:
    _start_t: float = time()
    func()
    duration: float = time() - _start_t
    print(f"[i] {duration:.2f} sec")

def run_cmd(cmd: str, msg: str) -> None:
    print("[*]", msg, "...")
    if system(cmd): 
        print("[!]", msg, "failed")
        exit(1)

for i in range(1, argc):
    match argv[i]:
        case "-comp": do_comp = True
        case "-run":  do_run = True
        case "-dbg":  dbg_info = True
        case _: print("[!]", "Unknown instrustion", argv[i])

if dbg_info: cmd_comp += " -g"
#else:        cmd_comp += " -s"

# ======= RUN ======= #

if argc <= 1:
    print(cmd_comp)  
    run_time(lambda: run_cmd(cmd_comp, "Compiling"))
    run_cmd(cmd_run, "Running")
else:
    if do_comp: 
        print(cmd_comp)
        run_time(lambda: run_cmd(cmd_comp, "Compiling"))
    if do_run: 
        run_cmd(cmd_run, "Running")