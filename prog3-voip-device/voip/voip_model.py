# File:     voip_model.py
# Author:   Michael Bruno
# Descr:    Event model for VOIP states.

from voip_device import VoipDevice
from voip_states import *

class Model(object):
    def __init__(self, initial=Awaiting):
        self.on_tick = lambda: None
        self.running = True

        "State type"
        self.state = initial

        "State object"
        self.statefull = None

    def set_event(self, event):
        self.state = self.statefull.on_event(event)

    def step(self,event):
        self.on_tick()
        self.statefull = self.state()

        self.set_event(event)
        return event != e.done

    def run(self):
        while self.running:
            self.running = self.tick()