import distutils.sysconfig

##SOURCES
engine_src = File(filter(lambda s: not("wrap" in s),
                         Glob("c++/engine/*.cc", strings=True)))
char_src = Glob("c++/characters/*.cc")
swig_src = Glob("c++/engine/*.i")

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

libs = ['SDL',
        'SDLmain',
        'SDL_image',
        'SDL_mixer',
        'GL']

miscdirs = [".config", ".data"]
for d in miscdirs:
  Execute(Mkdir(d))
wrappy = "c++/engine/engine.py"


#TARGETS
engine = env.SharedLibrary('engine', engine_src + char_src + swig_src,
                   LIBS=libs, LIBPATH=["."], SHLIBPREFIX="_")
#INSTALL
env.Install(source=[wrappy,
                    engine,
                    Glob("../src/python/*.py")],
            target="runtime")

env.Install(source=["runtime",
                    "../content",
                    "../src/scripts",
                    Glob("../info/*"),
                    miscdirs],
            target="../dist")

env.InstallAs(target='../dist/gufg', source='../src/scripts/gufg.py')
