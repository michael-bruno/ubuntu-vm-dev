# File:     voip_config.py
# Author:   Michael Bruno
# Descr:    Configuration for audio device and socket.
#           Imported by 'voip_device'.

import sys
import tty
import enum
import time
import select
import threading
import pyttsx3
import termios
import socket
import datetime
import alsaaudio

LOCK =  threading.Lock()

"Text-to-speech object"
PYTTSX3 = pyttsx3.init()
PYTTSX3.setProperty('volume',1)

"Port constants"
BROADCAST_PORT = 4097
STREAMING_PORT = 4098

"Audio streaming constants"
SAMPLE_RATE = 44100
PERIOD_SIZE = 1000

SIZE_TO_RW = PERIOD_SIZE * 2

"Capture device"
CAPTURE = alsaaudio.PCM(alsaaudio.PCM_CAPTURE,mode=alsaaudio.PCM_NONBLOCK,device='default')

CAPTURE.setchannels(1)
CAPTURE.setrate(SAMPLE_RATE)
CAPTURE.setformat(alsaaudio.PCM_FORMAT_S16_LE)
CAPTURE.setperiodsize(PERIOD_SIZE)


"Playback device"
PLAYBACK = alsaaudio.PCM(alsaaudio.PCM_PLAYBACK,mode=alsaaudio.PCM_NONBLOCK, device='default')

PLAYBACK.setchannels(1)
PLAYBACK.setrate(SAMPLE_RATE)
PLAYBACK.setformat(alsaaudio.PCM_FORMAT_S16_LE)
PLAYBACK.setperiodsize(PERIOD_SIZE)

"Broadcast Socket"
BROADCAST = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

BROADCAST.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
BROADCAST.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
BROADCAST.bind(("", BROADCAST_PORT))
BROADCAST.settimeout(0.1)

"Streaming Socket"
STREAMING = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

STREAMING.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
STREAMING.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
STREAMING.bind(("", STREAMING_PORT))
STREAMING.settimeout(0.1)

READMAX = 1024

def gtime():
    return datetime.datetime.now().strftime("%H:%M:%S")


def log(msg):
    cur = datetime.datetime.now()
    print(f'[LOG]::{cur.strftime("%H:%M:%S")}::{msg}')


class llist(list):
    "List subclass to hold a fixed size"
    max = 10

    def append(self,item):
        list.append(self,item)
        if len(self) > llist.max:
            self[:1] = []

class status(enum.Enum):
    "Enum class for device status"

    awaiting = "Awaiting call"
    calling = "Calling"
    incoming = "Incoming call from"
    incall = "Call in progress"

class ecmd(enum.Enum):
    "Enum class for device commands"

    call = "call"
    reject = "reject"
    accept = "accept"
    endcall = "endcall"
    incall = "incall"
    awaiting = "awaiting"

    none = "none"

    @classmethod
    def convert(cls,data):
        return cls(data) if data in [_.value for _ in cls] else cls.none

class iput(enum.Enum):
    "Enum class for user input"

    done = 100
    enter = 101
    cancel = 102

    yes = "y"
    no = "n"

    dev_ = -1
    dev1 = 1
    dev2 = 2
    dev3 = 3
    dev4 = 4
    dev5 = 5
    dev6 = 6
    dev7 = 7
    dev8 = 8
    dev9 = 9

    none = None

    @staticmethod
    def devices():
        return [
            iput.dev1,
            iput.dev2,
            iput.dev3,
            iput.dev4,
            iput.dev5,
            iput.dev6,
            iput.dev7,
            iput.dev8,
            iput.dev9
        ]

    @staticmethod
    def convert(data):
        "Converts keyboard input to Enum"

        if data:
            if '\n' in data:
                return iput.enter

            if data == '\x1b':
                return iput.done

            if "x" in data:
                return iput.cancel

            if "n" in data:
                return iput.no

            if "y" in data:
                return iput.yes

            if data.isdigit():
                return iput(int(data))

        return iput.none


class NonBlockingConsole(object):
    """
    Context manager class ('with' compatable) for getting non-blocking keyboard input found here:
        - https://stackoverflow.com/questions/2408560/python-nonblocking-console-input/10079805
    """

    def __enter__(self):
        self.old_settings = termios.tcgetattr(sys.stdin)
        tty.setcbreak(sys.stdin.fileno())
        return self

    def __exit__(self, type, value, traceback):
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self.old_settings)

    def get_data(self):
        if select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], []):
            return sys.stdin.read(1)
        return False


class vcmd(object):
    "VOIP command class for recieved commands"

    def __init__(self,handle):
        self.handle = handle
        self.processed = False

    def __eq__(self,other):
        return (self.__class__.__name__ == other.__class__.__name__) and self.handle == other.handle

    def __str__(self):
        return self.__repr__()

    def __repr__(self):
        return f"{self.__class__.__name__} ({self.processed})"

    @staticmethod
    def convert(handle,data):
        "Convert 'cmd_str' to 'vcmd' object."

        command = vcmd(handle)
        data = data.strip()

        if "incall" in data:
            cmd = data.split()
            command = InCall(handle)

        elif "endcall" in data:
            cmd,dev = data.split()
            command = EndCall(handle,dev)         

        elif "call" in data:
            cmd,dev,ip = data.split()
            command = Call(handle,dev,ip)

        elif "reject" in data:
            cmd,dev = data.split()
            command = Reject(handle,dev)

        elif "accept" in data:
            cmd,dev,ip = data.split()
            command = Accept(handle,dev,ip)   


        
        return command


class Call(vcmd):
    def __init__(self,handle,device_to_call,your_ip):
        super().__init__(handle)

        self.device_to_call = device_to_call
        self.your_ip = your_ip


class Reject(vcmd):
    def __init__(self,handle,device_to_reject):
        super().__init__(handle)

        self.device_to_reject = device_to_reject


class Accept(vcmd):
    def __init__(self,handle,device_to_accept,your_ip):
        super().__init__(handle)

        self.device_to_accept = device_to_accept
        self.your_ip = your_ip


class InCall(vcmd):
     def __init__(self,handle):
        super().__init__(handle)


class EndCall(vcmd):
    def __init__(self,handle,device_to_endcall):
        super().__init__(handle)

        self.device_to_endcall = device_to_endcall
