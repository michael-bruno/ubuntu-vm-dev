# File:     VoipTest.py
# Author:   Michael Bruno
# Descr:    Application specific implemenation of 'VoipDevice'.

from VoipDevice import VoipDevice
from VoipStates import e

device = VoipDevice()

device.on_tick = lambda: print(
    "Available:", *["{0}: {1}".format(k.value,k.name) for k in device.state.available()], sep="\n- ")
device.run()
