from os.path import realpath
import distutils.sysconfig

##SOURCES
engine_src = Glob("c++/engine/*.cc")
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
        'GL'],

#TARGETS
engine = env.SharedLibrary('engine', engine_src + char_src + swig_src,
                   LIBS=libs, LIBPATH=["."], SHLIBPREFIX="_")

wrapper = "c++/engine/engine.py"

runtime = env.Install(source=[wrapper,engine,
                              Glob("../src/python/*.py")],
                      target="../dist/runtime")
scripts = env.Install(source="../src/scripts",
                      target="../dist")
gufg = env.Install(source="../src/gufg", target="../dist")
distFiles = env.Install(source=["../content",
                                Glob("../info/*")],
                        target="../dist")
