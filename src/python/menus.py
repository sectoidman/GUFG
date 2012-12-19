from input import Input
from screen import Screen

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
