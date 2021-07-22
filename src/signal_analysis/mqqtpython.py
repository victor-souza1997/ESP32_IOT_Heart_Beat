import paho.mqtt.client as mqtt
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import timeit
import pandas as pd

N = 200 #tamanho do array plot
client_get = mqtt.Client(client_id='', protocol=mqtt.MQTTv31)
client_get.connect("broker.hivemq.com", 1883)
array_msg = np.array([0] * N, dtype=float);#string que salvara 


def gaussian(u, sigma, f):
    return 1/(sigma*np.sqrt(2*np.pi))*np.exp(-1/2*pow((f-u)/sigma, 2))

def classificar(f):
    df = pd.read_csv("../../contents/classificacao.csv")
    df = df.values
    if(gaussian(df[0,0], df[0,1], f) > gaussian(df[1,0], df[1,1], f)):
        print("paciente bem")
    else:
        print("paciente mal")

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
def callback(client, userdata, message):
    global array_msg, fig, aux1
    #print(str(message.payload.decode("utf-8")))	
    temp = float(str(message.payload.decode("utf-8")))
    print(temp)
    classificar(temp)
    #print(array_msg)
    #np.savetxt("../../contents/data.csv", temp, delimiter=",")
    array_msg = shift5(array_msg, -1)
    array_msg[-1] = temp

if __name__ == "__main__":
	topic =  "amostrar/heart_beat" 
	client_get.on_message = callback
	client_get.subscribe(topic, qos=0)
	try:
		client_get.loop_forever()
	except KeyboardInterrupt:
		client_get.disconnect()