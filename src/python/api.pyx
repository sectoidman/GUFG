cimport api
from api cimport interface
cdef class Interface:
  cdef interface * _interface
  def __cinit__(self):
    self._interface = new interface()
  def __dealloc__(self):
    del self._interface
