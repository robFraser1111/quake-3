# Quake III Arena, modified GPL source

This is a modified build of the [Quake III Arena source](https://github.com/id-Software/Quake-III-Arena) that id Software released under the GNU General Public License, version 2, on August 20, 2005 (upstream commit `dbe4ddb10315479fc00086f08e25d968b4b43c49`).

It is not an official id Software or ZeniMax product, and it is not endorsed by them. The original license, credits, and the upstream `README.txt` are kept. `COPYING.txt` is the GPL.

`BUILDING.md` is how to compile it and how to point the binary at a legal copy of the game data.

## What changed

The engine, the base game modules, the Team Arena modules, and `q3asm` build as 64-bit code with CMake. Current GCC, Clang, and Visual Studio 2022 (toolset v143, Windows 10/11 SDK) are the compilers this tree is set up for. GitHub Actions compiles a Release build on Linux and on Windows. The Linux job also starts the dedicated server far enough to see it stop because no game data is present. Nothing in CI launches a match.

The QVM files shipped inside the retail pk3s are still 32-bit bytecode. On this build they run in the interpreter. The 32-bit x86 JIT is not compiled. Native `qagame` / `cgame` / `ui` modules are built beside the executable for when you set `vm_game`, `vm_cgame`, and `vm_ui` to `0`.

Q3Radiant, the old `q3map`, `bspc`, and the retargeted LCC sources are still in the tree. They are not part of this build.

## Game data

This repository does not contain Quake III Arena game data. There are no `.pk3` or `.pak` files. You need your own legal copy of the game to run it. Team Arena data, if you have it, stays in `missionpack/`.

## License

GNU General Public License, version 2. See `COPYING.txt`.

Some files in the id drop are not covered by the GPL (the JPEG library, the unzip code, MD4, and a few others). Their notices are in `README.txt` and in the files themselves. Those notices stay.
