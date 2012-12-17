from pygame import Surface
from OpenGL.GL import GLuint


class Glyph(GLuint):
  pass


class Screen(Surface):
  height, width, floor, wall = int(), int(), int(), int()
  fullscreen = bool()
  scalingFactor = float()
  glyphs = [Glyph()] * 91

  def __init__(self, width, height):
    pass

  def drawGlyph(self, string, ):
    pass

class Background(GLuint):
  def __init__(imageName=None):
    pass
