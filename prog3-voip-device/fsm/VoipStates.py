# File:     VoipStates.py
# Author:   Michael Bruno
# Descr:    Application specific implementation of 'State' class.

from State import State
import enum

class e(enum.IntEnum):
    done = 0
    wait = 1
    call = 2
    recieve = 3
    talk = 4

    @classmethod
    def eput(cls, string=None):
        got = input(string)
        return cls(int(got if got.isdigit() and int(got) in [_.value for _ in cls] else cls.done))

class Waiting(State):
    pass

class Calling(State):
    pass

class Recieving(State):
    pass

class Talking(State):
    pass

Waiting.enter = lambda a: print("\nI'm waiting\n")
Waiting.exit = lambda a: print("\nStopped waiting\n")
Waiting.switch = {
    e.call: Calling,
    e.recieve: Recieving
}

Calling.enter = lambda a: print("\nI'm calling\n")
Calling.exit = lambda a: print("\nStopped calling\n")
Calling.switch = {
    e.talk: Talking,
    e.wait: Waiting
}

Recieving.enter = lambda a: print("\nI'm recieving\n")
Recieving.exit = lambda a: print("\nStopped recieving\n")
Recieving.switch = {
    e.talk: Talking,
    e.wait: Waiting
}

Talking.enter = lambda a: print("\nI'm talking\n")
Talking.exit = lambda a: print("\nStopped exitting\n")
Talking.switch = {
    e.wait: Waiting,
}
