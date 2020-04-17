# Python script to use as host end of the testBluetooth demo
# You have to connect the robot bluetooth first (code = 0000)


from matplotlib import pyplot
from comPort import ComPort

# First, we create a ComPort object.
# The COMxx to use is the one associated with your bluetooth peripheral
# It can be found in windows's "device manager"
bt = ComPort('COM5')
sampleNb = 250
amplitude = 1000

try:
    # First, we open the COM port
    bt.open()
    # then we send the integer parameters:
    # number of samples wanted
    # amplitude of the sine wave
    bt.sendInt(sampleNb)
    bt.sendInt(amplitude)
    # the robot should answer by sending the excpected number of float samples
    data = []
    for a in range(sampleNb):
        data.append( bt.getFloat() )
    #Finally, we plot the result
    pyplot.plot(data)
    pyplot.xlabel("sample")
    pyplot.ylabel("data")
    pyplot.show()

except OSError:
    # exception raised when the port cannot be opened.
    # Usualy wrong port number or robot not powered.
    print("COM port cannot be opened")
except :
    # Something went wrong during communication.
    # Usualy, we try to read some data and the robot did
    # not send enough bytes.
    print("Communication error")
finally:
    # In any case, we close the 
    bt.close()
