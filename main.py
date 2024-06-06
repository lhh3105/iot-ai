import serial
import joblib
import pandas as pd
import numpy as np
import BlynkLib

ESP8266Serial = serial.Serial('COM4', 9600)
BLYNK_AUTH = 'vKenIR6pxw3Gps0TYIiajcad6K_QB-4S'
blynk = BlynkLib.Blynk(auth=BLYNK_AUTH)

def read_data(line):
    data = line.split(',')
    BPM = float(data[0])
    Sp02 = float(data[1])
    return BPM, Sp02

model = joblib.load('ensemble_model.pkl')
while True:
    blynk.run()
    if ESP8266Serial.in_waiting > 0:
        line = ESP8266Serial.readline()
        line = line.decode().strip()
        print(line)
        if line.__contains__('Predict') == False:
            BPM, SpO2 = read_data(line)
            if 30 <= BPM <= 130 and 70 <= SpO2 <= 100:
                df = pd.DataFrame({'HR (BPM)': [BPM], 'SpO2 (%)': [SpO2]})
                prediction = model.predict(df)
                txt = 'Predict: ' + str(prediction[0])
                if prediction[0] == 0:
                    blynk.virtual_write(2, 'Bình thường')
                else:
                    blynk.virtual_write(2, 'Có bất thường')
            else:
                blynk.virtual_write(2, 'Cảm biến đang có vấn đề')