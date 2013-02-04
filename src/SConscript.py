##SOURCES
engine_src = File(filter(lambda s: not("wrap" in s),
                         Glob("engine/*.cc", strings=True)))
char_src = Glob("characters/*.cc")
backend_src = engine_src + char_src
bmpout_src = [File("bmp.cc")]
gufg_exec_src = [File("game.cc")]

##ENVIRONMENT
env = Environment(CXXFLAGS="-O2 -g\
                            -Wall\
                            -Wno-parentheses\
                            -Wno-unused-but-set-variable\
                            -std=c++11",
                  LIBS=['SDL','SDLmain','SDL_image','SDL_mixer','GL', 'GLU', 'glut'])

miscdirs = [".config", ".data"]

#TARGETS
bmpout = env.Program('bmpout', backend_src + bmpout_src)
gufg_exec = env.Program('gufg',backend_src + gufg_exec_src)

binTargets=[bmpout, gufg_exec]
distTargets=["../content", "../src/scripts", Glob("../info/*"), miscdirs, binTargets]

#INSTALL TARGETS
bins = [env.Install(target="..", source=t) for t in binTargets]
dist = env.Install(target="../dist", source=distTargets)

Default("../gufg")
