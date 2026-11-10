# Building this Quake III Arena tree

This tree builds with CMake as 64-bit code. Release is the configuration GitHub Actions compiles. The CI job does not start a match.

You need your own legal copy of Quake III Arena for the game data. Nothing in this repository is a `.pk3` or `.pak`.

The version string printed at startup is `Q3 1.32b-mod`. The network protocol is still 68.

## Install

### Windows

1. Visual Studio 2022, workload **Desktop development with C++**.
2. A Windows 10 or Windows 11 SDK (the installer component is enough).
3. CMake (the one Visual Studio installs is enough).

No separate DirectX SDK. DirectInput 8, DirectSound, and OpenGL come from the Windows SDK.

### Linux

GCC or Clang, CMake, and the X11/OpenGL headers the original client links:

```
sudo apt install build-essential cmake pkg-config \
  libgl1-mesa-dev libx11-dev libxext-dev libxxf86vm-dev libxxf86dga-dev
```

`libxxf86vm` is the VidMode extension. `libxxf86dga` is the old DGA extension the 1.32 client still calls.

## Build

From the repository root.

Windows, x64 Release:

```
cmake -S . -B build -A x64
cmake --build build --config Release
```

Linux:

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

The binaries land in `build/bin/Release/` (or `build/bin/Debug/` if you configured Debug):

```
quake3.x86_64            client
quake3ded.x86_64         dedicated server (dedicated is forced on)
q3asm                    QVM assembler
baseq3/qagamex86_64      game module
baseq3/cgamex86_64       client game
baseq3/uix86_64          menu
missionpack/             the same three modules, built with MISSIONPACK
```

On Windows each name has `.exe` or `.dll`. On Linux the modules are `.so` with no `lib` prefix, which is the name `Sys_LoadDll` looks up.

A 32-bit configure is rejected. The QVM file format is still 32-bit; only the host engine is 64-bit.

## Game data and launch

Point `fs_basepath` at the directory that **contains** `baseq3`, not at `baseq3` itself. `baseq3` must contain your legal `pak0.pk3` (and the later paks from your install).

Steam's folder is usually one of:

```
C:\Program Files (x86)\Steam\steamapps\common\Quake 3 Arena
C:\Program Files (x86)\Steam\steamapps\common\Quake III Arena
~/.steam/steam/steamapps/common/Quake 3 Arena
~/.local/share/Steam/steamapps/common/Quake 3 Arena
```

Use whichever of those actually has `baseq3/pak0.pk3`.

From `build/bin/Release`:

```
quake3.x86_64.exe +set fs_basepath "C:\Program Files (x86)\Steam\steamapps\common\Quake 3 Arena"
```

```
./quake3.x86_64 +set fs_basepath "$HOME/.steam/steam/steamapps/common/Quake 3 Arena"
```

Without `pak0.pk3` the process reaches filesystem init and stops with `Couldn't load default.cfg`. That is what the Linux CI job checks. It is not a successful game launch.

The retail pk3s contain `vm/qagame.qvm`, `vm/cgame.qvm`, and `vm/ui.qvm`. The cvars `vm_game`, `vm_cgame`, and `vm_ui` default to `2`, which loads those QVMs. This build interprets them. There is no 64-bit JIT.

The native modules next to the executable are used when those cvars are `0`. Start the game with the working directory set to `build/bin/Release` (so `./baseq3/qagamex86_64.so` is visible), or copy the modules into the game's `baseq3` and `missionpack` directories:

```
+set vm_game 0 +set vm_cgame 0 +set vm_ui 0
```

`0` tries the native module first and falls back to the QVM. Team Arena uses the modules under `missionpack/` when that game directory is active.

A dedicated server, from the same directory:

```
./quake3ded.x86_64 +set fs_basepath "/path/to/Quake 3 Arena" +map q3dm17
```

`quake3ded` is built with `DEDICATED`. `dedicated` is already `1` and read-only. It does not open a window.

Configs and downloaded files go to `~/.q3a` on Linux. On Windows they follow the usual Quake 3 homepath (the working directory, unless you set `fs_homepath`).

## What the 64-bit port actually changes

The retail VM file is a 32-bit machine. Offsets inside it stay 32-bit. `entityState_t`, `playerState_t`, `usercmd_t`, and `entityShared_t` contain no pointers, so the engine can read the shared prefix of a QVM entity with the same layout as the 1999 bytecode. `playerState_t` is 468 bytes and `usercmd_t` is 24. Those sizes are checked at compile time.

Host pointers are not 32-bit. The places that used to stuff a pointer into an `int` and would drop the high half:

- The DLL syscall boundary. `dllEntry`, `vmMain`, and `VM_DllSyscall` pass `intptr_t`. On x86_64 the old `&arg` trick does not see the rest of the arguments, so every argument is copied with `va_arg`. `VM_ArgPtr` returns a native pointer unchanged for a DLL, and adds the VM data base for a QVM offset.
- `GAME_CLIENT_CONNECT` returns a `char *`. That value is no longer truncated before `VM_ExplicitArgPtr`.
- The hunk is aligned with `uintptr_t`. The old `(int)` cast discarded the top of the address.
- `tess.xyz` alignment and the interpreter's `OP_BLOCK_COPY` alignment test mask the full address.
- Windows stores `HINSTANCE` and the window procedure in cvars as full hex pointers and reads them back with `strtoull`. `SetWindowLongPtr` / `GWLP_WNDPROC` replaces `SetWindowLong` on the console edit box. Window procedures return `LRESULT`.
- `Sys_SnapVector` on non-x86 is the C truncation used by the `SnapVector` macro. The 32-bit NASM `snapvector` / `ftol` objects are not linked.

`NETF`, `FOFS`, and the botlib `*_OFS` macros cast a null-based field address to `int`. The value is a struct offset, not a live pointer. Those structs are far smaller than 2GB, so the low 32 bits are the offset.

Still true, and left that way because it matches the file format:

- Hunk, pk3, and message offsets are 32-bit. A pak past 2GB will not load.
- The bytecode interpreter is the QVM implementation on x86_64. It is slower than the old 32-bit JIT.
- x86_64 compiles `float` and `double` as SSE, not 80-bit x87. A server and a client built from this tree match each other. They can disagree by a ulp with an original 32-bit executable on a few movement edges.

## Known limits

- **No proprietary data.** The build stops without your own `baseq3/pak0.pk3`.
- **Legacy OpenGL.** The renderer creates a compatibility context. A driver that only exposes a core profile will not render.
- **Linux sound** is still OSS (`/dev/dsp` via `linux/soundcard.h`). PulseAudio and PipeWire do not speak that device unless you add an OSS shim yourself.
- **Linux input and video** are still X11, including DGA and VidMode. There is no Wayland backend and no SDL.
- **Windows sound** is DirectSound. **Windows mouse** init loads `dinput8.dll` (`DirectInput8Create`). The old `dinput.dll` / `DirectInputCreateA` entry point is not in current Windows.
- **IPX** still compiles. Current Windows has no IPX stack, so those sockets fail and the game falls back to UDP.
- **Q3Radiant, q3map, bspc, and LCC** are the upstream sources. This CMake build does not compile them. `q3asm` does build, which is the assembler half of making a QVM. The LCC retarget (`lcc/`) is unchanged and is not a CI target.
- **FreeType** was already compiled out of the 1.32 drop (`BUILD_FREETYPE` is unset). Team Arena fonts are the pre-rendered ones in the game data.
- CI compiles Release and, on Linux, checks that the dedicated binary reports missing `default.cfg`. It does not connect to a server or draw a frame.
