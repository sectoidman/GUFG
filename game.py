
"""
Usage:
  game.py [-hmr] [-w | -f]

-h --help   Show this screen
-m ROUNDS   Set the number of rounds
-r RATE     Set the frame rate in Hz
-f          Set fullscreen
-w WIDTH    Set a window width

"""

from time import time
from docopt import docopt
from interface import interface

class frame:
  def __init__(self, rate):
    self.rate = rate
    self.width = 1.0 / rate
    self.count = 0
    self.time = 0.0
  def __call__(self):
    self.count += 1
    self.time = time() + self.width
    while time() <= self.time:
      pass

class game:
  def __init__(self, rate, rounds):
    self.timer = frame(rate)
    self.interface = interface()
    assert self.interface.screenInit()
    self.interface.createPlayers()
    self.interface.loadMisc()
    self.interface.startGame()
    if rounds > 0 and rounds < 10:
      self.interface.numRounds = rounds
  def step(self):
      self.interface.readInput()
      self.interface.resolve()
      self.interface.draw()
      self.interface.cleanup()
  def loop(self, count=float('infinity')):
   """
    Loops forever or until count is exceeded.
    Each step of the loop performs all relevant game actions
    for one frame.
   """
   while not self.interface.gameover:
     while self.timer.count <= count:
      self.step()
      self.timer()

if __name__ == '__main__':
  arguments = docopt(__doc__)
  rate = int(arguments['-r'] or 60)
  rounds = int(arguments['-m'] or 2)
  #Width and fullscreen could be handled here
  gufpe = game(rate, rounds)
  gufpe.loop()

