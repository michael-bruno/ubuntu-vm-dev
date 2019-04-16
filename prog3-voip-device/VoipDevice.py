# File:     VoipDevice.py
# Author:   Michael Bruno
# Descr:    StateMachine class controlling 'State' flow.

from VoipStates import Waiting
from VoipStates import e


class VoipDevice(object):
    def __init__(self, initial=Waiting):
        self.on_tick = lambda: None
        self.running = True

        "State type"
        self.state = initial

        "State object"
        self.statefull = None

    def set_event(self, event):
        self.state = self.statefull.on_event(event)

    def tick(self):
        self.on_tick()
        self.statefull = self.state()

        event = e.eput("> ")
        self.set_event(event)
        return event != e.done

    def run(self):

        while self.running:
            self.running = self.tick()
