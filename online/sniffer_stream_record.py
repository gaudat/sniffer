import sys
import serial
from datetime import datetime

if len(sys.argv) < 2:
    print("Usage: %s [SERIAL_PORT] [SERIAL_BAUDRATE] <OUTPUT_FILE>"%sys.argv[0])

serial_port_name = sys.argv[0]
serial_port_baudrate = int(sys.argv[1])
if len(sys.argv) == 2:
    dump_file = "sniffer_%f.log"%datetime().timestamp()
dump_file = sys.argv[2]

sport = serial.Serial(serial_port_name, serial_port_baudarate)
# Set a reasonable timeout to prevent
sport.timeout = 0.1

with open(dump_file,'w+',0) as dump:
    l = sport.readline()
    print(l)
    dump.write("%f %s",datetime().timestamp(),l)