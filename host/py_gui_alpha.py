import serial
import serial.tools.list_ports as list_ports
import string
import time
import json
import PySimpleGUI as sg
sg.ChangeLookAndFeel('Reddit')

# CH340 product ID - custom HW ID
USB_product_ID 	= 29987
device 			= None
hwID		 	= 'D3C4D3B0'

# times to try to connect to uart 
triesToConnect_UART = 10
uartSuccess = False

def get_com_list():
	device_count = 0
	CH340_targets[4] = ['', '', '', '']
	ports = list(serial.tools.list_ports.comports())
	for p in ports:
		if p.pid == USB_product_ID:
			CH340_targets[counter] = p.device
			counter += 1
	return CH340_targets


def serial_init(CH340_targets, device):
	counter = 0
	global uartSuccess
	if port:
		while hasattr(device, 'close') is False:
			for possible_target in CH340_targets:
				try:
					device = serial.Serial(possible_target, 9600, timeout= 5)
					# print('uart succesfully initialized')
					if device.readline().decode("utf-8")[0] == hwID:
						print('found it')
						uartSuccess = True
					else:
						uartSuccess = False
						time.sleep(1.5)
				except serial.SerialException: 
					print('trying to connect')
					time.sleep(0.1)
					counter += 1
					if counter == triesToConnect_UART:
						print('Check device selection')
						break

		return device


layout = [  
			[sg.Button('Connect')],
			[sg.Text('On time:'), sg.In(size=(8,1), key='onTime'), sg.Text('waiting', key='labelOn'), sg.Text('[ms]')],
			[sg.Text('Off time:'), sg.In(size=(8,1), key='offTime'), sg.Text('waiting', key='labelOff'), sg.Text('[ms]')],
			[sg.Button('Send', disabled = True), sg.Button('Stop', disabled = True)]
        ]

window = sg.Window('Solenoid Control 1.1', layout)
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
			window.Element('Stop').Update(disabled=False)
			window.Element('Connect').Update('Connected')
		else:
			window.Element('Connect').Update(disabled=True)
			window.Element('Connect').Update('Check Serial')

# if user closes, switch off solenoid 
window.close()
if device:
	dont_fry_me_plz()
	device.close()
