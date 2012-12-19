from player import *
from instance import *

class Fighter(Player):
  pass

class FightingGame(Instance):
  players = [Player()] * 2
  gameover = False
  time = 99 * 60
  floor = 50
  wall = 50
  def __init__(self, width=1600, height=900, rounds=2):
    super(FightingGame, self).__init__(self, width, height)
    self.background = Background(width=width*2, height=height*2)

  def matchInit(self):
    pass

  def roundInit(self):
    pass

  def start(self):
    pass
    self.backend = interface(len(players))
    #Initialize a simplified c++ interface object with stuff loaded in py
    #All performance-critical stuff should entirely happen there.

  def main(self):
    pass
    # Get inputs
    # Pass them to the backend, let it resolve/draw stuff

  def quit(self):
    self.gameover = True
