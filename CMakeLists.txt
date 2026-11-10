cmake_minimum_required(VERSION 3.16)

# Modified Quake III Arena GPL source. This is not an official id Software build.
project(quake3 VERSION 1.32.0 LANGUAGES C)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR
    "This tree builds as 64-bit only (sizeof(void*) == 8). "
    "The QVM bytecode stays 32-bit; host pointers do not.")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# bin/<Config>/quake3.x86_64 and bin/<Config>/baseq3/*.so (or .dll).
# $<CONFIG> is used so Visual Studio does not append a second config directory.
set(Q3_BIN_DIR "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

function(q3_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE
      /W3
      /wd4018 /wd4100 /wd4133 /wd4146 /wd4244 /wd4267 /wd4305 /wd4996
    )
    target_compile_definitions(${target} PRIVATE
      WIN32
      _CRT_SECURE_NO_WARNINGS
      _CRT_NONSTDC_NO_DEPRECATE
      _USE_MATH_DEFINES
      _WINSOCK_DEPRECATED_NO_WARNINGS
    )
  else()
    target_compile_options(${target} PRIVATE
      -Wall
      -Wno-unused-parameter
      -Wno-unused-function
      -Wno-unused-variable
      -Wno-unused-but-set-variable
      -Wno-sign-compare
      -Wno-missing-field-initializers
      -Wno-pointer-sign
      -Wno-unused-but-set-parameter
      -fno-strict-aliasing
      -fwrapv
      -fcommon
      -fsigned-char
    )
  endif()
endfunction()

function(q3_place target dest)
  # ARCHIVE is the MSVC/MinGW import library. Two modules share an
  # OUTPUT_NAME (baseq3 and missionpack), so the import lib has to
  # follow the module into its own directory.
  set_target_properties(${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${dest}"
    LIBRARY_OUTPUT_DIRECTORY "${dest}"
    ARCHIVE_OUTPUT_DIRECTORY "${dest}"
  )
endfunction()

# ---------------------------------------------------------------------------
# Sources shared by the client and the dedicated server.
# The 32-bit x86/PPC JITs are not built. QVMs run in the interpreter.
# ---------------------------------------------------------------------------
set(Q3_BOTLIB
  code/botlib/be_aas_bspq3.c
  code/botlib/be_aas_cluster.c
  code/botlib/be_aas_debug.c
  code/botlib/be_aas_entity.c
  code/botlib/be_aas_file.c
  code/botlib/be_aas_main.c
  code/botlib/be_aas_move.c
  code/botlib/be_aas_optimize.c
  code/botlib/be_aas_reach.c
  code/botlib/be_aas_route.c
  code/botlib/be_aas_routealt.c
  code/botlib/be_aas_sample.c
  code/botlib/be_ai_char.c
  code/botlib/be_ai_chat.c
  code/botlib/be_ai_gen.c
  code/botlib/be_ai_goal.c
  code/botlib/be_ai_move.c
  code/botlib/be_ai_weap.c
  code/botlib/be_ai_weight.c
  code/botlib/be_ea.c
  code/botlib/be_interface.c
  code/botlib/l_crc.c
  code/botlib/l_libvar.c
  code/botlib/l_log.c
  code/botlib/l_memory.c
  code/botlib/l_precomp.c
  code/botlib/l_script.c
  code/botlib/l_struct.c
)
set_source_files_properties(${Q3_BOTLIB} PROPERTIES COMPILE_DEFINITIONS BOTLIB)

set(Q3_QCOMMON
  code/qcommon/cm_load.c
  code/qcommon/cm_patch.c
  code/qcommon/cm_polylib.c
  code/qcommon/cm_test.c
  code/qcommon/cm_trace.c
  code/qcommon/cmd.c
  code/qcommon/common.c
  code/qcommon/cvar.c
  code/qcommon/files.c
  code/qcommon/huffman.c
  code/qcommon/md4.c
  code/qcommon/msg.c
  code/qcommon/net_chan.c
  code/qcommon/unzip.c
  code/qcommon/vm.c
  code/qcommon/vm_interpreted.c
)

set(Q3_SERVER
  code/server/sv_bot.c
  code/server/sv_ccmds.c
  code/server/sv_client.c
  code/server/sv_game.c
  code/server/sv_init.c
  code/server/sv_main.c
  code/server/sv_net_chan.c
  code/server/sv_snapshot.c
  code/server/sv_world.c
)

set(Q3_GAME_COMMON
  code/game/q_math.c
  code/game/q_shared.c
)

set(Q3_CLIENT
  code/client/cl_cgame.c
  code/client/cl_cin.c
  code/client/cl_console.c
  code/client/cl_input.c
  code/client/cl_keys.c
  code/client/cl_main.c
  code/client/cl_net_chan.c
  code/client/cl_parse.c
  code/client/cl_scrn.c
  code/client/cl_ui.c
)

set(Q3_SOUND
  code/client/snd_adpcm.c
  code/client/snd_dma.c
  code/client/snd_mem.c
  code/client/snd_mix.c
  code/client/snd_wavelet.c
)

set(Q3_RENDERER
  code/renderer/tr_animation.c
  code/renderer/tr_backend.c
  code/renderer/tr_bsp.c
  code/renderer/tr_cmds.c
  code/renderer/tr_curve.c
  code/renderer/tr_flares.c
  code/renderer/tr_font.c
  code/renderer/tr_image.c
  code/renderer/tr_init.c
  code/renderer/tr_light.c
  code/renderer/tr_main.c
  code/renderer/tr_marks.c
  code/renderer/tr_mesh.c
  code/renderer/tr_model.c
  code/renderer/tr_noise.c
  code/renderer/tr_scene.c
  code/renderer/tr_shade.c
  code/renderer/tr_shade_calc.c
  code/renderer/tr_shader.c
  code/renderer/tr_shadows.c
  code/renderer/tr_sky.c
  code/renderer/tr_surface.c
  code/renderer/tr_world.c
)

# The in-tree JPEG is the engine's decoder. FreeType is not in this drop
# (BUILD_FREETYPE stays off; Team Arena fonts are pre-rendered).
set(Q3_JPEG
  code/jpeg-6/jcapimin.c
  code/jpeg-6/jchuff.c
  code/jpeg-6/jcinit.c
  code/jpeg-6/jccoefct.c
  code/jpeg-6/jccolor.c
  code/jpeg-6/jfdctflt.c
  code/jpeg-6/jcdctmgr.c
  code/jpeg-6/jcphuff.c
  code/jpeg-6/jcmainct.c
  code/jpeg-6/jcmarker.c
  code/jpeg-6/jcmaster.c
  code/jpeg-6/jcomapi.c
  code/jpeg-6/jcparam.c
  code/jpeg-6/jcprepct.c
  code/jpeg-6/jcsample.c
  code/jpeg-6/jdapimin.c
  code/jpeg-6/jdapistd.c
  code/jpeg-6/jdatasrc.c
  code/jpeg-6/jdcoefct.c
  code/jpeg-6/jdcolor.c
  code/jpeg-6/jddctmgr.c
  code/jpeg-6/jdhuff.c
  code/jpeg-6/jdinput.c
  code/jpeg-6/jdmainct.c
  code/jpeg-6/jdmarker.c
  code/jpeg-6/jdmaster.c
  code/jpeg-6/jdpostct.c
  code/jpeg-6/jdsample.c
  code/jpeg-6/jdtrans.c
  code/jpeg-6/jerror.c
  code/jpeg-6/jidctflt.c
  code/jpeg-6/jmemmgr.c
  code/jpeg-6/jmemnobs.c
  code/jpeg-6/jutils.c
)

set(Q3_GAME_MODULE
  code/game/ai_chat.c
  code/game/ai_cmd.c
  code/game/ai_dmnet.c
  code/game/ai_dmq3.c
  code/game/ai_main.c
  code/game/ai_team.c
  code/game/ai_vcmd.c
  code/game/bg_misc.c
  code/game/bg_pmove.c
  code/game/bg_slidemove.c
  code/game/g_active.c
  code/game/g_arenas.c
  code/game/g_bot.c
  code/game/g_client.c
  code/game/g_cmds.c
  code/game/g_combat.c
  code/game/g_items.c
  code/game/g_main.c
  code/game/g_mem.c
  code/game/g_misc.c
  code/game/g_missile.c
  code/game/g_mover.c
  code/game/g_session.c
  code/game/g_spawn.c
  code/game/g_svcmds.c
  code/game/g_syscalls.c
  code/game/g_target.c
  code/game/g_team.c
  code/game/g_trigger.c
  code/game/g_utils.c
  code/game/g_weapon.c
  code/game/q_math.c
  code/game/q_shared.c
)

set(Q3_CGAME_MODULE
  code/game/bg_misc.c
  code/game/bg_pmove.c
  code/game/bg_slidemove.c
  code/cgame/cg_consolecmds.c
  code/cgame/cg_draw.c
  code/cgame/cg_drawtools.c
  code/cgame/cg_effects.c
  code/cgame/cg_ents.c
  code/cgame/cg_event.c
  code/cgame/cg_info.c
  code/cgame/cg_localents.c
  code/cgame/cg_main.c
  code/cgame/cg_marks.c
  code/cgame/cg_players.c
  code/cgame/cg_playerstate.c
  code/cgame/cg_predict.c
  code/cgame/cg_scoreboard.c
  code/cgame/cg_servercmds.c
  code/cgame/cg_snapshot.c
  code/cgame/cg_syscalls.c
  code/cgame/cg_view.c
  code/cgame/cg_weapons.c
  code/game/q_math.c
  code/game/q_shared.c
)

set(Q3_UI_MODULE
  code/game/bg_misc.c
  code/q3_ui/ui_addbots.c
  code/q3_ui/ui_atoms.c
  code/q3_ui/ui_cdkey.c
  code/q3_ui/ui_cinematics.c
  code/q3_ui/ui_confirm.c
  code/q3_ui/ui_connect.c
  code/q3_ui/ui_controls2.c
  code/q3_ui/ui_credits.c
  code/q3_ui/ui_demo2.c
  code/q3_ui/ui_display.c
  code/q3_ui/ui_gameinfo.c
  code/q3_ui/ui_ingame.c
  code/q3_ui/ui_loadconfig.c
  code/q3_ui/ui_main.c
  code/q3_ui/ui_menu.c
  code/q3_ui/ui_mfield.c
  code/q3_ui/ui_mods.c
  code/q3_ui/ui_network.c
  code/q3_ui/ui_options.c
  code/q3_ui/ui_playermodel.c
  code/q3_ui/ui_players.c
  code/q3_ui/ui_playersettings.c
  code/q3_ui/ui_preferences.c
  code/q3_ui/ui_qmenu.c
  code/q3_ui/ui_removebots.c
  code/q3_ui/ui_saveconfig.c
  code/q3_ui/ui_serverinfo.c
  code/q3_ui/ui_servers2.c
  code/q3_ui/ui_setup.c
  code/q3_ui/ui_sound.c
  code/q3_ui/ui_sparena.c
  code/q3_ui/ui_specifyserver.c
  code/q3_ui/ui_splevel.c
  code/q3_ui/ui_sppostgame.c
  code/q3_ui/ui_spskill.c
  code/q3_ui/ui_startserver.c
  code/ui/ui_syscalls.c
  code/q3_ui/ui_team.c
  code/q3_ui/ui_teamorders.c
  code/q3_ui/ui_video.c
  code/game/q_math.c
  code/game/q_shared.c
)

set(Q3_MPUI_MODULE
  code/ui/ui_atoms.c
  code/ui/ui_gameinfo.c
  code/ui/ui_main.c
  code/ui/ui_players.c
  code/ui/ui_syscalls.c
  code/ui/ui_util.c
  code/ui/ui_shared.c
  code/game/bg_misc.c
  code/game/q_math.c
  code/game/q_shared.c
)

function(q3_add_module target outname gamedir)
  cmake_parse_arguments(ARG "" "" "SOURCES;DEFS;INCLUDES" ${ARGN})
  add_library(${target} SHARED ${ARG_SOURCES})
  if(WIN32)
    # game.def / cgame.def / ui.def export vmMain and dllEntry.
    if(target MATCHES "qagame")
      target_sources(${target} PRIVATE code/game/game.def)
    elseif(target MATCHES "cgame")
      target_sources(${target} PRIVATE code/cgame/cgame.def)
    else()
      target_sources(${target} PRIVATE code/ui/ui.def)
    endif()
  endif()
  target_include_directories(${target} PRIVATE ${ARG_INCLUDES})
  target_compile_definitions(${target} PRIVATE ${ARG_DEFS})
  q3_warnings(${target})
  if(UNIX)
    target_link_libraries(${target} PRIVATE m)
    target_link_options(${target} PRIVATE "LINKER:--no-undefined")
  endif()
  set_target_properties(${target} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${outname}"
  )
  q3_place(${target} "${Q3_BIN_DIR}/${gamedir}")
endfunction()

q3_add_module(qagame qagamex86_64 baseq3
  SOURCES ${Q3_GAME_MODULE}
  INCLUDES code/game
)
q3_add_module(cgame cgamex86_64 baseq3
  SOURCES ${Q3_CGAME_MODULE}
  INCLUDES code/cgame code/game code/ui
)
q3_add_module(ui uix86_64 baseq3
  SOURCES ${Q3_UI_MODULE}
  INCLUDES code/q3_ui code/ui code/game code/cgame
)

q3_add_module(qagame_mp qagamex86_64 missionpack
  SOURCES ${Q3_GAME_MODULE}
  DEFS MISSIONPACK
  INCLUDES code/game
)
q3_add_module(cgame_mp cgamex86_64 missionpack
  SOURCES ${Q3_CGAME_MODULE} code/cgame/cg_newdraw.c code/ui/ui_shared.c
  DEFS MISSIONPACK
  INCLUDES code/cgame code/game code/ui
)
q3_add_module(ui_mp uix86_64 missionpack
  SOURCES ${Q3_MPUI_MODULE}
  DEFS MISSIONPACK
  INCLUDES code/ui code/game code/cgame
)

# ---------------------------------------------------------------------------
# Engine
# ---------------------------------------------------------------------------
set(Q3_ENGINE
  ${Q3_BOTLIB}
  ${Q3_QCOMMON}
  ${Q3_SERVER}
  ${Q3_GAME_COMMON}
)

if(WIN32)
  set(Q3_PLATFORM_CLIENT
    code/win32/win_gamma.c
    code/win32/win_glimp.c
    code/win32/win_input.c
    code/win32/win_main.c
    code/win32/win_net.c
    code/win32/win_qgl.c
    code/win32/win_shared.c
    code/win32/win_snd.c
    code/win32/win_syscon.c
    code/win32/win_wndproc.c
    code/win32/winquake.rc
  )
  set(Q3_PLATFORM_DED
    code/null/null_client.c
    code/null/null_input.c
    code/null/null_snddma.c
    code/null/win_dedicated.c
    code/win32/win_main.c
    code/win32/win_net.c
    code/win32/win_shared.c
    code/win32/win_syscon.c
    code/win32/winquake.rc
  )
  set(Q3_PLATFORM_LIBS ws2_32 winmm user32 gdi32 advapi32 shell32 ole32)
  set(Q3_CLIENT_LIBS opengl32)
else()
  find_package(X11 REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(Threads REQUIRED)
  find_library(Q3_XXF86VM_LIB NAMES Xxf86vm REQUIRED)
  find_library(Q3_XXF86DGA_LIB NAMES Xxf86dga REQUIRED)
  set(Q3_PLATFORM_CLIENT
    code/unix/linux_common.c
    code/unix/linux_glimp.c
    code/unix/linux_joystick.c
    code/unix/linux_qgl.c
    code/unix/linux_signals.c
    code/unix/linux_snd.c
    code/unix/unix_main.c
    code/unix/unix_net.c
    code/unix/unix_shared.c
  )
  set(Q3_PLATFORM_DED
    code/null/null_client.c
    code/null/null_input.c
    code/null/null_snddma.c
    code/unix/linux_common.c
    code/unix/linux_signals.c
    code/unix/unix_main.c
    code/unix/unix_net.c
    code/unix/unix_shared.c
  )
  set(Q3_PLATFORM_LIBS
    ${CMAKE_DL_LIBS}
    m
    Threads::Threads
  )
  set(Q3_CLIENT_LIBS
    OpenGL::GL
    X11::X11
    X11::Xext
    ${Q3_XXF86VM_LIB}
    ${Q3_XXF86DGA_LIB}
  )
endif()

if(WIN32)
  add_executable(quake3 WIN32
    ${Q3_ENGINE} ${Q3_CLIENT} ${Q3_SOUND} ${Q3_RENDERER} ${Q3_JPEG} ${Q3_PLATFORM_CLIENT})
  add_executable(quake3ded WIN32
    ${Q3_ENGINE} ${Q3_PLATFORM_DED})
else()
  add_executable(quake3
    ${Q3_ENGINE} ${Q3_CLIENT} ${Q3_SOUND} ${Q3_RENDERER} ${Q3_JPEG} ${Q3_PLATFORM_CLIENT})
  add_executable(quake3ded
    ${Q3_ENGINE} ${Q3_PLATFORM_DED})
endif()

foreach(bin quake3 quake3ded)
  target_link_libraries(${bin} PRIVATE ${Q3_PLATFORM_LIBS})
  q3_warnings(${bin})
  q3_place(${bin} "${Q3_BIN_DIR}")
endforeach()
target_link_libraries(quake3 PRIVATE ${Q3_CLIENT_LIBS})
target_compile_definitions(quake3ded PRIVATE DEDICATED)

set_target_properties(quake3 PROPERTIES OUTPUT_NAME "quake3.x86_64")
set_target_properties(quake3ded PROPERTIES OUTPUT_NAME "quake3ded.x86_64")

# q3asm turns LCC assembly into QVM bytecode. The retargeted LCC itself is
# left as the upstream sources under lcc/; it is not part of this build.
add_executable(q3asm
  q3asm/q3asm.c
  q3asm/cmdlib.c
)
target_include_directories(q3asm PRIVATE q3asm)
q3_warnings(q3asm)
q3_place(q3asm "${Q3_BIN_DIR}")
if(UNIX)
  target_link_libraries(q3asm PRIVATE m)
endif()
