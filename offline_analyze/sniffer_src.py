from datetime import datetime
from time import sleep
class SnifferLive(object):
    """Sniffer data source returning every 0.5 second"""
    
    def __init__(self, src_name=""):
        # For testing, time multiplier in returning data
        self.time_mul = 1
        self.time_delay = 0.5
        # File source name
        if "com" or "/tty" in src_name:
            # Live data is coming in, initialize serial port
            # Lol I'm not going to implement it in one week's time
            assert False, "Not implemented"
        self.src_name = src_name
        self.src_file = open(src_name)
        
    def seek(self, offset=0):
        # Seek the input file to its head, or to specified offset
        if self.src_file:
            self.src_file.seek(offset)
        else:
            assert False, "Source file not opened"
    
    def reload(self, src_name=""):
        # Reload the input file
        self.src_file.close()
        if not src_name:
            # Reload same file
            self.src_file.open(self.src_name)
        else:
            self.src_file.open(src_name)

    def readline(self):
        # Seek until a newline
        c = self.src_file.read()
        while not c == "\n":
            c = self.src_file.read()
        # Pretend there is some delay
        sleep(self.time_delay/self.time_mul)
        return self.src_file.readline()