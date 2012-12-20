import distutils.sysconfig

##SOURCES
engine_src = File(filter(lambda s: not("wrap" in s),
                         Glob("c++/engine/*.cc", strings=True)))
char_src = Glob("c++/characters/*.cc")
swig_src = Glob("c++/engine/*.i")
 #srcs should be SCons-provided File() objects or iterators of those
 #you can get a list using Glob or just put a known File into a list.
bmpout_src = File("c++/bmp.cc")
replay_src = File("c++/replay.cc")
gufg_exec_src = File("c++/game.cc")


##ENVIRONMENT
env = Environment(PYEXT_USE_DISTUTILS=True,
                  SWIGFLAGS=['-c++','-python'],
                  CPPPATH=[distutils.sysconfig.get_python_inc()],
                  CXXFLAGS="-O2 -g\
                            -Wno-overloaded-virtual\
                            -Wno-format\
                            -Wno-format-contains-nul\
                            -Wctor-dtor-privacy\
                            -std=c++11")

libs=['SDL','SDLmain','SDL_image','SDL_mixer','GL']

miscdirs = [".config", ".data"]
for d in miscdirs:
  Execute(Mkdir(d))
wrappy = "c++/engine/engine.py"

#TARGETS
engine = env.SharedLibrary('engine', engine_src + char_src)
runtime = env.SharedLibrary('runtime', engine + swig_src, SHLIBPREFIX="_",LIBS=libs)

#Then specify a Program or SharedLibrary as a method of the env we set up,
#these take a name, then an iterable list of SCons file objects
#(or file iterables)
#which can include source and objects we assigned earlier, such as engine
bmpout = env.Program('bmpout', [engine, bmpout_src], LIBS=libs)
replay = env.Program('replay', [engine, replay_src], LIBS=libs)
gufg_exec = env.Program('gufg', [engine, gufg_exec_src], LIBS=libs)


#INSTALL
env.Install(source=[wrappy,
                    runtime,
                    Glob("../src/python/*.py")],
            target="runtime")

env.Install(source=["runtime",
                    "../content",
                    "../src/scripts",
                    Glob("../info/*"),
                    bmpout,
                    replay,
                    gufg_exec,
                    miscdirs],
            target="../dist")

env.InstallAs(target='../dist/gufg.py', source='../src/scripts/gufg.py')
