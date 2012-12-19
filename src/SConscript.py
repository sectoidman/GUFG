import distutils.sysconfig
from os.path import realpath

engine_src = Glob("c++/engine/*.cc")
char_src = Glob("c++/characters/*.cc")
swig_src = Glob("c++/engine/*.i")

env = Environment(CPPPATH=[distutils.sysconfig.get_python_inc()],
                  SHLIBPREFIX="_",
                  CXXFLAGS="-O2 -g\
                            -Wno-overloaded-virtual\
                            -Wno-format\
                            -Wno-format-contains-nul\
                            -Wctor-dtor-privacy\
                            -std=c++11")
env.LoadableModule('engine', engine_src + char_src + swig_src,
            LIBS=['engine',
                  'SDL',
                  'SDLmain',
                  'SDL_image',
                  'SDL_mixer',
                  'GL'],
            LIBPATH=['.'], SWIGFLAGS="-c++ -python")
