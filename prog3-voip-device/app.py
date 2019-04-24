import sys

from voip import VoipDevice

if len(sys.argv) != 2:
    print("Usage: python3 voip_app.py [device_handle] [clear]")

clear = True
device_handle = sys.argv[1]
if len(sys.argv) == 3:
    clear = False

device = VoipDevice(device_handle,clear)
device.run()
