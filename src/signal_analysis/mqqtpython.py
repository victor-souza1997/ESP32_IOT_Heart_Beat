import paho.mqtt.client as mqtt
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import timeit


N = 200;
array_msg = np.array([0] * N, dtype=float);#string que salvara 
#fig = plt.figure()
#aux1 = fig.add_subplot(111)
#plt.ion()
#fig.show()
#fig.canvas.draw()

def shift5(arr, num, fill_value=np.nan):
    result = np.empty_like(arr)
    if num > 0:
        result[:num] = fill_value
        result[num:] = arr[:-num]
    elif num < 0:
        result[num:] = fill_value
        result[:num] = arr[-num:]
    else:
        result[:] = arr
    return result

client_get = mqtt.Client(client_id='', protocol=mqtt.MQTTv31)
client_get.connect("broker.hivemq.com", 1883)

def callback(client, userdata, message):
    global array_msg, fig, aux1
    #print(str(message.payload.decode("utf-8")))	
    temp = float(str(message.payload.decode("utf-8")))
    print(temp)
    #print(array_msg)
    np.savetxt("data.csv", array_msg, delimiter=",")
    array_msg = shift5(array_msg, -1)
    array_msg[-1] = temp

#print(array_msg)
if __name__ == "__main__":
	topic =  "amostrar/heart_beat" 
	client_get.on_message = callback

	client_get.subscribe(topic, qos=0)
	try:
		client_get.loop_forever()
	except KeyboardInterrupt:
		client_get.disconnect()