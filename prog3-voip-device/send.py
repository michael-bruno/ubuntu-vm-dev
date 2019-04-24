from voip import *

while True:
    BROADCAST.sendto("YO bro".encode(), ("<broadcast>", BROADCAST_PORT))
    time.sleep(1)