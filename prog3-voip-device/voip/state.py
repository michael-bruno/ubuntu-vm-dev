# File:     State.py
# Author:   Michael Bruno
# Descr:    Python implementation of a 'state' interface to be used in conjunction
#           with a state machine.

class Interface(type):
    "Meta class containing class-specific members."

    def __init__(cls, name, bases, attrs):
        cls.switch = {}
        cls.enter = lambda a: None
        cls.exit = lambda a: None

class State(metaclass=Interface):
    def __init__(self, *args, **kwargs):
        "Executes 'enter' function and initializes 'State' object."

        self.__class__.enter([])
        return super().__init__(*args, **kwargs)

    def on_event(self, event):
        "Executes 'exit' function and returns evaluated 'State' type."

        self.__class__.exit([])
        return self.__class__.switch.get(event, self.__class__)

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return self.__class__.__name__

    @classmethod
    def available(cls):
        "Returns class 'switch' object."
        
        return cls.switch
