# File:     voip_states.py
# Author:   Michael Bruno
# Descr:    State definitions for VOIP module.

from state import State

class Awaiting(State):
    pass

class Calling(State):
    pass

class InCall(State):
    pass

class EndingCall(State):
    pass

class Accepting(State):
    pass

class Rejecting(State):
    pass
