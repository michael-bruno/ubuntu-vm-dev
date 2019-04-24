# File:     voip_device.py
# Author:   Michael Bruno
# Descr:    Python implementation for VOIP audio streaming.

import os
import threading

from voip_config import *


class VoipDevice(object):
    "Class for VOIP device implementation logic and behavior."

    width = 60

    def __init__(self, handle,ip_addr):
        self.ip_addr = ip_addr

        self.status = status.awaiting.value
        self.selected = iput.dev_
        self.handle = handle

        self.busy = ecmd.awaiting

        "Bools"
        self.running = True

        "State bools"
        self.awaiting = False
        self.calling = False
        self.recieving = False
        self.in_call = False

        self.counter = 0
        self.text = ""

        self.buffer = llist()
        self.devices = []

    @property
    def display(self):
        devices_list = ""
        for device in enumerate(self.devices):
            index, handle = device

            pointer = ' ' * (5 - (self.counter % 5)) + '<' * (self.counter % 5)
            devices_list += f"|\t({index+1}): {handle} {pointer if index+1 == self.selected.value else ''}\n"

        return f"""|{"=" * VoipDevice.width}
| VOIP DEVICE @ {gtime()} \t\t\t {self.text}
|{"=" * VoipDevice.width}
| [ESC]   - Exit        [Y] - Accept Call
| [1-9]   - Select      [N] - Reject Call
| [ENTER] - Call        [X] - Cancel Call
|{"=" * VoipDevice.width}
| Status:   {self.status}
| Handle:   {self.handle}
|
| Devices:
{devices_list}
"""
    @property
    def get_dev(self):
        return self.devices[self.selected.value-1]

    def wait(self,length=1):
        time.sleep(length)

    def set_status(self, status):
        self.status = status

    def read_broadcast(self):
        """
        Returns tuple:
            'handle'            (str)
            'command_object'    (vcmd)
        """

        handle = ""
        command = ""

        try:
            data, addr = BROADCAST.recvfrom(READMAX)
            data = str(data,"utf-8")

            handle, command = data.split("@")
            command = command.strip()

        except socket.timeout:
            pass

        return handle.strip(), vcmd.convert(handle,command)


    def run(self):
        run_thread = threading.Thread(target=self.background,daemon=True)
        run_thread.start()

        dis_thread = threading.Thread(target=self.discovery,daemon=True)
        dis_thread.start()

        clear_thread = threading.Thread(target=self.clear_buffer,daemon=True)
        clear_thread.start()

        rem_thread = threading.Thread(target=self.remove_devices,daemon=True)
        rem_thread.start()

        aud_thread = threading.Thread(target=self.audio_queue,daemon=True)
        aud_thread.start()

        self.Awaiting()

        log("Closing device..")

    def audio_queue(self):
        while self.running:
            pass

    def clear_buffer(self):
        size = len(self.buffer)
        while self.running:
            increased = size > len(self.buffer)

            if increased and len(self.buffer) > 2:
                continue

            if len(self.buffer) >= 1:
                self.buffer.pop(0)

            size = len(self.buffer)
                
            self.wait(1.5)

    def remove_devices(self):
        while self.running:
            for dev in self.devices:
                for cmd in self.buffer:
                    if dev == cmd.handle:
                        break
                else:
                    self.devices.remove(dev)
            self.wait(1)
    
    def discovery(self):
        size = len(self.buffer)
        while self.running:
            handle, cmd_obj = self.read_broadcast()
            
            if handle and handle != self.handle:
                self.buffer.append(cmd_obj)

                if handle not in self.devices:
                    self.devices.append(handle)

    def background(self):
        while self.running:

            os.system('clear')
            print(self.display)

            self.counter += 1
            self.wait(0.2)


#======== DEVICE BEHAVIOR =========


    def Incoming(self,rcall: Call):
        "Device is recieving a call."

        def parse_input(usr_input):
            usr_input = iput.convert(usr_input)

            for cmd in self.buffer:
                if not cmd.processed and isinstance(cmd,Call) and self.handle == cmd.device_to_call and rcall.handle == cmd.handle:
                    cmd.processed = True

            # accept call
            if usr_input == iput.yes:
                BROADCAST.sendto(f"{self.handle}@{ecmd.accept.value} {rcall.handle} {self.ip_addr}".encode(), ("<broadcast>", BROADCAST_PORT))

                self.recieving = False

                self.InCall(rcall)

            # reject call
            if usr_input == iput.no:
                BROADCAST.sendto(f"{self.handle}@{ecmd.reject.value} {rcall.handle}".encode(), ("<broadcast>", BROADCAST_PORT))

                self.recieving = False

            if rcall not in self.buffer:
                self.recieving = False
        
        with NonBlockingConsole() as nbc:
            self.recieving = True

            while self.recieving:
                self.set_status(f"{status.incoming.value} {rcall.handle}")
                parse_input(nbc.get_data())
        
    def Awaiting(self):
        "Device is awaiting."

        def parse_input(usr_input):
            usr_input = iput.convert(usr_input)

            for cmd in self.buffer:
                if not cmd.processed and isinstance(cmd,Call) and self.handle == cmd.device_to_call:
                    cmd.processed = True

                    PYTTSX3.say(f"Incoming call from {cmd.handle}")
                    PYTTSX3.runAndWait()
                    self.Incoming(cmd)
                    break

            if usr_input in iput.devices():
                self.selected = usr_input

            if usr_input == iput.enter and 0 < self.selected.value <= len(self.devices):
                self.Calling()

            if usr_input == iput.done:
                self.awaiting = False

        def ping_handle():
            while self.awaiting:
                if not self.calling:
                    BROADCAST.sendto(f"{self.handle}@{self.busy.value}".encode(), ("<broadcast>", BROADCAST_PORT))

                self.wait()

        with NonBlockingConsole() as nbc:
            self.awaiting = True

            ping_thread = threading.Thread(target=ping_handle,daemon=True)
            ping_thread.start()

            while self.running and self.awaiting:
                self.set_status(f"{status.awaiting.value}")
                parse_input(nbc.get_data())

    def Calling(self):
        "Device is attempting to call selected device."

        def parse_input(usr_input):
           
            usr_input = iput.convert(usr_input)

            for cmd in self.buffer:
                "Call has been rejected"
                if not cmd.processed and isinstance(cmd,Reject) and self.handle == cmd.device_to_reject:
                    cmd.processed = True

                    self.calling = False

                    PYTTSX3.say(f"{cmd.handle} has rejected your call.")
                    PYTTSX3.runAndWait()
                    break

                elif not cmd.processed and isinstance(cmd,Accept) and self.handle == cmd.device_to_accept:
                    cmd.processed = True

                    self.calling = False
                    self.InCall(cmd)
                    break

                elif not cmd.processed and isinstance(cmd,InCall) and cmd.handle == self.get_dev:
                    cmd.processed = True

                    self.calling = False
                    PYTTSX3.say(f"{cmd.handle} is not available.")
                    PYTTSX3.runAndWait()
                    break

            if usr_input == iput.cancel:
                self.calling = False

        def ping_call():
            while self.calling and self.get_dev in self.devices:
                BROADCAST.sendto(f"{self.handle}@{ecmd.call.value} {self.get_dev} {BROADCAST.getsockname()[0]}".encode(), ("<broadcast>", BROADCAST_PORT))
                self.wait()

        with NonBlockingConsole() as nbc:
            self.calling = True

            call_thread = threading.Thread(target=ping_call,daemon=True)
            call_thread.start()

            while self.running and self.calling:
                self.set_status(f"{status.calling.value} {self.get_dev}")
                parse_input(nbc.get_data())

    def InCall(self,icall):

        def parse_input(usr_input):
            usr_input = iput.convert(usr_input)

            if usr_input == iput.cancel:
                BROADCAST.sendto(f"{self.handle}@{ecmd.endcall.value} {icall.handle}".encode(), ("<broadcast>", BROADCAST_PORT))
                self.in_call = False

            for cmd in self.buffer:
                "Call has been terminated"
                if not cmd.processed and isinstance(cmd,EndCall) and self.handle == cmd.device_to_endcall:
                    cmd.processed = True

                    self.in_call = False

        def capture_loop():
            while self.in_call and icall.handle in self.devices:
                numframes, write = CAPTURE.read()
                STREAMING.sendto(write, (icall.your_ip, STREAMING_PORT))

        def playback_loop():
            while self.in_call and icall.handle in self.devices:
                try:
                    read = STREAMING.recv(SIZE_TO_RW)
                    PLAYBACK.write(read)
                    
                except:
                    pass

        with NonBlockingConsole() as nbc:

            self.busy = ecmd.incall
            self.in_call = True

            cap_thread = threading.Thread(target=capture_loop,daemon=True)
            cap_thread.start()

            play_thread = threading.Thread(target=playback_loop,daemon=True)
            play_thread.start()           

            self.text = f"({icall.handle})*"

            while self.in_call and icall.handle in self.devices:
                parse_input(nbc.get_data())

                self.set_status(f"{status.incall.value}")
                
            self.text = ""
            PYTTSX3.say(f"Call ended.")
            PYTTSX3.runAndWait()
            self.busy = ecmd.awaiting
    

