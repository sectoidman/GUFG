from pygame import Surface
from OpenGL.GL import GLuint


class Glyph(GLuint):
  pass


class Screen(Surface):
  height, width, floor, wall = int(), int(), int(), int()
  fullscreen = bool()
  scalingFactor = float()
  glyphs = [Glyph()] * 91

  def __init__(self, args):
    pass

  def drawGlyph(self, string, ):
    pass


class Input:
  sAxis = [bool()] * 2
  posEdge = [int()] * 2
  negEdge = [bool()] * 2


class Player:
  pass


class Instance(Screen, Input):
  """
  """

  def __init__(self):
    super(Instance, self).__init__()

  def readInput(self):
    """
      Accept input into input containers for anything that wants it.
    """
    pass

  def resolve(self):
    """
      Every inheriting type will need a resolve loop.
      This function is purely virtual, and does nothing.
    """
    pass


class Background(GLuint):
  def __init__(imageName=None):
    pass


class Menu(Screen, Input):
  def __init__(self):
    super(Menu, self).__init__()


class CharacterSelect(Menu):
  numChars = 2
  background = Background()
  pass


class Start(Menu):
  pass


class KeyConfig(Menu):
  pass


class Rematch(Menu):
  pass


class FightingGame(Instance):
  players = [Player()] * 2
  gameover = False
  time = 99 * 60

  def __init__(self):
    super(FightingGame, self).__init__()
    self.background = Background()

  def createPlayers(self):
    pass

  def createDemons(self, replay=None):
    pass

  def matchInit(self):
    pass

  def roundInit(self):
    pass

  def start(self):
    pass
    # backend = None
    #Initialize a simplified c++ interface object with stuff loaded in py
    #All performance-critical stuff should entirely happen there.

  def main(self):
    pass
    # Get inputs
    # Pass them to the backend, let it resolve/draw stuff
