import sys

from voip import VoipDevice

if len(sys.argv) != 3:
    print("Usage: python3 voip_app.py [device_handle] [ip]")
    sys.exit()

device_handle = sys.argv[1]
ip_addr = sys.argv[2]

device = VoipDevice(device_handle,ip_addr)
device.run()
