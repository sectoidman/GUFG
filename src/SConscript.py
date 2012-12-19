from os.path import realpath
import distutils.sysconfig
from re import compile as regex

##SOURCES
engine_src = Glob("c++/engine/*.cc")
char_src = Glob("c++/characters/*.cc")
cython_src = Glob("python/*.p{yx,xd}")
print cython_src
##STUFF FOR BUILDING CYTHON
pyxcpp = Builder(action='cython --cplus $SOURCE')
def cythonLibrary(env, name, source, **kwargs):
  cpp = env.PyxCpp(source)
  env.SharedLibrary(name, cpp, **kwargs)

##ENVIRONMENT
env = Environment(BUILDERS={'PyxCpp':pyxcpp},
                  PYEXT_USE_DISTUTILS=True,
                  CPPPATH=[distutils.sysconfig.get_python_inc()],
                  CXXFLAGS="-O2 -g\
                            -Wno-overloaded-virtual\
                            -Wno-format\
                            -Wno-format-contains-nul\
                            -Wctor-dtor-privacy\
                            -std=c++11")

env.AddMethod(cythonLibrary, 'CythonLibrary')

libs = ['engine',
        'SDL',
        'SDLmain',
        'SDL_image',
        'SDL_mixer',
        'GL'],

#TARGETS
env.SharedLibrary('engine', engine_src + char_src)
env.CythonLibrary("api", cython_src,
                  LIBS=libs,
                  LIBPATH=["."])
Program('../gufg', 'c++/game.cc',
            LIBS=libs,
            LIBPATH=["."])
