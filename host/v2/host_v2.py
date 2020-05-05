import serial
import serial.tools.list_ports as list_ports
import string
from time import sleep
import json
import PySimpleGUI as sg
import numpy

sg.ChangeLookAndFeel('Reddit')

# CH340 product ID
USB_product_ID = 29987
device = None

# times to try to connect to uart
triesToConnect_UART = 10
uartSuccess = False

# settings / tokens
resValToken = '>R'

# resistor values 
array_in = [
1980,
3810,
8110,
8060,
32500,
148800,
511000,
2220000,
3850000,
4810000,
9790000,
11520000,
10630000,
9990000]

# corresponding powers of two 
array_in_pow2 = [
2,
4,
8,
16,
32,
64,
128,
512,
1024,
2048,
4096,
8192,
16384,
32768]


def get_com_list():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if p.pid == USB_product_ID:
            return p.device


def serial_init(port, device):
    counter = 0
    global uartSuccess
    if port:
        while hasattr(device, 'close') is False:
            try:
                device = serial.Serial(port, 9600, timeout=5)
                print('uart succesfully initialized')
                while device.readline().decode("utf-8")[0] != '>':
                    print('waiting token')
                uartSuccess = True
                sleep(2.5)
            except serial.SerialException:
                print('trying to connect')
                sleep(0.1)
                counter += 1
                if counter == triesToConnect_UART:
                    print('Check device selection')
                    break
        return device

def subset_sum(array_in, array_out):

    temp_val = 0
    counter = 0
    n = len(array_in)
    total_combinations = 1<<n
    temp_array = [0] * total_combinations

    for i in range(0,total_combinations):
        temp_val = 0

        for j in range (0,n):
            if i & (1<<j):
                temp_val += array_in[j]

        array_out.append(str(temp_val))

    array_out = temp_array



array_out = []
subset_sum(array_in, array_out)
array_out = list(map(int, array_out))

array_out_pow2 = []
subset_sum(array_in_pow2, array_out_pow2)
array_out_pow2 = list(map(int, array_out_pow2))
array_out_pow2 = [65535-x for x in array_out_pow2]

layout = [
    [sg.Button('Connect')],
    [sg.Text('Min/Max Resistance:'), sg.Text( str(min(array_out)) +'/'+ str(max(array_out)) )  ],
    [sg.Text('Resistor Value:'), sg.In(size=(8, 1), key='resval'), sg.Text('waiting for setting', key='actualval'), sg.Text('[Ω]')],
    [sg.Button('Send', disabled=True)],
    #[sg.Combo([array_out])]
]


window = sg.Window('Decade Box 0.2.0', layout)

while True:
    event, values = window.read()
    # print(event, values)

    if event is None:
        break

    # uart connect
    if event == 'Connect':
        device = serial_init(get_com_list(), device)
        if uartSuccess is True:
            window.Element('Connect').Update(disabled=True)
            window.Element('Send').Update(disabled=False)
            window.Element('Connect').Update('Connected')
        else:
            window.Element('Connect').Update(disabled=True)
            window.Element('Connect').Update('Check Serial')

    if event == 'Send':

        if not (int(values['resval'].isdigit())) or \
               (int(values['resval'])) > max(array_out) or \
               (int(values['resval'])) < min(array_out):
            values['resval'] = '0'

        near_value = min(array_out, key=lambda x:abs(x-int(values['resval'])))

        print(near_value)
        fetched_resistance = array_out[array_out.index(near_value)]
        fetched_pow2 = array_out_pow2[array_out.index(near_value)]
        window.Element('actualval').Update(str(fetched_resistance))

        resSettings = '{"en":1,"resVal":' + str(fetched_resistance) + ', "shiftVal":' + str(fetched_pow2) + '}'
        device.write(resValToken.encode())
        print(resValToken)
        device.write(resSettings.encode())
        print(resSettings)

        # device.write(resValToken.encode())
        # print(resValToken)
        # device.write(resSettings.encode())
        # print(resSettings)

        # print(min(array_out, key=lambda x:abs(x-10000000)))
   
window.close()
if device:
    device.close()


## -------- Resistor Values ---------
## Base
## R1: 1.980k
## R2: 3.81k
## R3: 8.11k
## R4: 8.06k
## R5: 32.5k
## R6: 148.8k
## R7: 511k
## 
## Upper floor:
## R8: 2.22m
## R9: 3.85m
## R10: 4.81m
## R11: 9.79m
## R12: 11.52m
## R13: 10.63m
## R14: 9.99m