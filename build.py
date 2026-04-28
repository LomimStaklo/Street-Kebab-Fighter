from os import system
from sys import argv
from time import time
from platform import system as get_os_name

# STREET KEBAB FIGHTER

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

# ---- CONFIG ------------------------------------------------------------------------------
cc: str       = "gcc" # clang / gcc
src: str      = "src/*.c"
flags: str    = "-std=c99 -Wall -Wextra -Wswitch-enum -Wconversion -Wsign-conversion -Wdouble-promotion -Wundef -Wuninitialized -Wshadow -Wnull-dereference -Wno-#warnings -pedantic -fstack-protector -O1"
lib_path: str = "src/lib"
inc_path: str = "src/include"

os_name: str = get_os_name() 
out: str     = "SKF"
libs: str    = ""
cmd_run: str = ""

is_static: bool = False
dbg_info: bool  = False
do_run: bool    = False
do_comp: bool   = False

# ---- USER ARGS ---------------------------------------------------------------------------
argc: int = len(argv)

for i in range(1, argc):
    match argv[i]:
        case "-comp":   do_comp = True
        case "-run":    do_run = True
        case "-dbg":    dbg_info = True
        case "-comp-static": 
            do_comp = True
            is_static = True
        case _: print("[!]", "Unknown instrustion", argv[i])

# ---- OS ----------------------------------------------------------------------------------
match os_name:
    case "Windows":
        out   += ".exe"
        cmd_run = f".\\{out}"
        flags += " -mwindows"
        libs  += "-lmingw32 -lSDL2main -lSDL2 -lSDL2_image"
        if is_static:
            libs += " -static -static-libgcc -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va -lm -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid"

    case "Linux":
        cmd_run = f"./{out}"
        libs = "-lSDL2 -lSDL2_image"
    
    case _:
        print(f"[!] Wrong operating system {os_name}")
        exit(1)

if dbg_info: flags += " -g"
else:        flags += " -s"

# ---- RUN ---------------------------------------------------------------------------------
cmd_comp = f"{cc} -o {out} {src} -I {inc_path} -L {lib_path} {libs} {flags}"

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
