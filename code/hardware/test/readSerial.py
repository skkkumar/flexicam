import serial
import os
import subprocess


if __name__ == '__main__':
    #find the usb device for arduino
    os.chdir("/dev")
    proc = subprocess.Popen(['ls'],         stdout=subprocess.PIPE,  stderr=subprocess.PIPE) 
    out = proc.stdout.read()
    #print "matching ports " , out
    print " error " , proc.stderr.read()
    lines = out.splitlines()
    USBPORT = ''
    for i in range(len(lines)):
        if lines[i].startswith('ttyUSB'):
            USBPORT = lines[i]
        elif lines[i].startswith('ttyACM'):
            USBPORT = lines[i]
        else:
            continue
    USBPORT = '/dev/' + USBPORT     
    print 'USBPORT' , USBPORT
    

    #connect to the serial port
    ser = serial.Serial(USBPORT, 2000000, timeout=2, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.
    ser.flushInput()
    ser.flushOutput()

    While True:
        bytesToRead = ser.inWaiting()
        ser.read(bytesToRead)
