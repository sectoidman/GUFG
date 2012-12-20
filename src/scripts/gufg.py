#!/usr/bin/env python2

"""
Usage:
  game.py [-h] [-m <ROUNDS>] [-r <RATE>] [-w | -f]

-h --help   Show this screen
-m ROUNDS   Set the number of rounds
-r RATE     Set the frame rate in Hz
-f          Set fullscreen
-w WIDTH    Set a window width

"""

from time import time
from docopt import docopt
from runtime.engine import interface

class game:
  def __init__(self, rate, rounds):
    self.interface = interface()
    assert self.interface.screenInit()
    self.interface.createPlayers()
    self.interface.loadMisc()
    self.interface.startGame()
    if rounds > 0 and rounds < 10:
      self.interface.numRounds = rounds
    self.frameWidth = 1.0 / rate
  def step(self):
    endTime = time() + self.frameWidth
    self.interface.readInput()
    self.interface.resolve()
    self.interface.draw()
    self.interface.cleanup()
    while time() <= endTime:
      pass
  def loop(self):
   """
    Loops forever or until count is exceeded.
    Each step of the loop performs all relevant game actions
    for one frame.
   """
   while not self.interface.gameover:
    self.step()

if __name__ == '__main__':
  arguments = docopt(__doc__)
  rate = int(arguments['-r'] or 60)
  rounds = int(arguments['-m'] or 2)
  #Width and fullscreen could be handled here
  gufg = game(rate, rounds)
  gufg.loop()

