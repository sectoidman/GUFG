cdef extern from "../c++/engine/interface.h":
  cdef cppclass interface:
    interface()
    void cleanup()
    void createPlayers()
    void draw()
    void loadMisc()
    void readInput()
    void resolve()
    bint screenInit()
    void startGame()
    int numRounds
    bint gameover
