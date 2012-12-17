from pygame import event, KEYDOWN, KEYUP, QUIT

class Input:
  sAxis = [bool()] * 4
  posEdge = [int()] * 6
  negEdge = [bool()] * 6
  def __init__(self):
    pass
  def get(self):
    """
      Accept input into input containers for anything that wants it.
    """
    for i in event.get(): #[:20]?
      for t, e in events.items():
        if t == i.type:
          e(self)
  def keydown(self):
    pass
  def keyup(self):
    pass

class Player(Input):
  def __init__(self):
    self.events = {\
      KEYDOWN: self.keydown,
      KEYUP: self.keyup
      }
    self.counter = 0

