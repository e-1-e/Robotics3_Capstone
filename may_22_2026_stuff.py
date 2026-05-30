import requests, time, json, serial
print("SLALALALALA")
active = True

ser = serial.Serial("COM13", 115200, write_timeout=None) if active else None

wakeUpProgram = False
readyForData = True
a = b''
def processData(res):
	global readyForData, a

	if not readyForData:
		time.sleep(0.001)
		print("notReadyForData: ", end="")
		while ser.in_waiting > 0:
			a += ser.read()
			#print("slatt")
			if a != b'':
				print(a.decode("utf-8"))
				if (a.decode("utf-8").find("READY") != -1):
					readyForData = True
		return

	print("time")
	readyForData = False
	#print(res)
	remoteData = json.loads(res)
	
	'''
	Send data in format: ?
	"<{num1}, {num2}, {num3}>"
	where num1 is baseRotator input
	and num2 is armHinger input
	and num3 is mode

	Nah we json the shi with the above keys
	
	'''
	# converted = (f"<{remoteData["accelGyroStuff"][5]}, {remoteData["accelGyroStuff"][4]}, 0>").encode("utf-8")
	# converted = (f"<{remoteData["joyStickStuff"][1]}, {remoteData["joyStickStuff"][0]}, 1>").encode("utf-8")

	# convertedData = {
	# 	"moveX" : remoteData["accelGyroStuff"][5] * -1,
	# 	"moveY": remoteData["accelGyroStuff"][4],
	# 	"mode": 1,
	# 	"buttonPress": remoteData["joyStickStuff"][2]
	# }

	convertedData = {
		"moveX" : remoteData["joyStickStuff"][1],
		"moveY": remoteData["joyStickStuff"][0],
		"mode": 1,
		"buttonPress": remoteData["joyStickStuff"][2]
	}


	
	#print(converted)
	#print("before2")
	print(json.dumps(convertedData).encode("utf-8"))
	sentDat = json.dumps(convertedData).encode("utf-8")
	for i in range(len(sentDat)):
		print(chr(sentDat[i]), end="")
		ser.write(sentDat[i].to_bytes())
		time.sleep(0.001)
	print()
	ser.flush()
	#print("after")
	# print(ser.readline().decode("utf-8"))
	a = b""
	while not ser.in_waiting:
		print(ser.in_waiting)
		time.sleep(0.001)
	while ser.in_waiting > 0:
		a += ser.read()
		#print("slatt")
	if a != b'':
		print(a.decode("utf-8"))
		if (a.decode("utf-8").find("READY") != -1):
			readyForData = True
	
	
	

with requests.Session() as bobjunior:
	while True:
		time.sleep(0.02)
		response = None
		#print("befroe")
		for i in range(50):
			try:
				response = bobjunior.get("http://192.168.4.1", timeout=5)
			except Exception as e:
				print(e)
			if response:
				break

			
		if not response:
			continue
		if active and wakeUpProgram:
			#print("triggered two")
			processData(response.text)
			
		else:
			print(response.text)
			if active and not wakeUpProgram:
				a = b""
				while ser.in_waiting > 0:
					a += ser.read()
					#print("slatt")
				if a != b'':
					print(a.decode("utf-8"))
					if (a.decode("utf-8").find("OMG SKIBIDI") != -1):
						wakeUpProgram = True
						print("triggered")
			

		
'''
sample response.text:
{
  "joyStickStuff": [
    1915,
    1897,
    0
  ],
  "accelGyroStuff": [
    -0.017578,
    0.086426,
    0.958984,
    -0.183105,
    0.488281, (armHinger) scale (-500, 500)
    3.540039 (baseRotator)
  ]
}
'''