cdef extern from "c++/engine/interface.h":
  cdef cppclass interface:
    interface() except +
    int screenInit()
    void createPlayers()
    void loadMisc()
    void startGame()
    int numRounds
    void resolve()
    void draw()
    void cleanup()
    void readInput()
    int gameover

