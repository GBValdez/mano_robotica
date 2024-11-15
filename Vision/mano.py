from typing import NamedTuple

import cv2
import mediapipe as mp
import numpy as np
from math import acos, degrees
import serial

ser= serial.Serial("COM6",9600,timeout=1)
mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_hands = mp.solutions.hands

cap = cv2.VideoCapture(0)
def sendData(dedos):
    cadena = ''.join(['1' if dedo else '0' for dedo in dedos])
    cadena+="\n"
    ser.write(cadena.encode())

def printDedos(dedos,frame):
    colors=[(255,255,255),(255,255,0),(255,0,255),(0,255,255),(0,255,0)]
    labels=["Pulgar","Indice","Medio","Anular","Menique"]
    for (i,finger) in enumerate(dedos):
        init=100+i*60
        cv2.rectangle(frame,(init,10),(init+50,60),colors[i],-1 if finger else 2)
        cv2.putText(frame,labels[i],(init,80),1,1,(255,255,255),2)

def getDedosCerrados(centroid,fingertips,fingerbase):
    coordinateCentroid= np.array(centroid)
    coordinates_ft= np.array(fingertips)
    coordinates_fb= np.array(fingerbase)

    d_centroid_ft= np.linalg.norm(coordinateCentroid-coordinates_ft,axis=1)
    d_centroid_fb= np.linalg.norm(coordinateCentroid- coordinates_fb,axis=1)
    dif = d_centroid_ft-d_centroid_fb
    return dif>0

def getCentroid(coord_list):
    coordinates= np.array(coord_list)
    centroid = np.mean(coordinates,axis=0)
    centroid= int(centroid[0]),int(centroid[1])
    return centroid

def getPoints(points,landmark, width, height):
    coordinates= []
    for index in points:
        x = int(landmark[index].x * width)
        y = int(landmark[index].y * height)
        coordinates.append([x,y])
    return coordinates
# thumb
def thumb(landmark, width, height):
    coordinates_thumb = getPoints([1, 2, 4],landmark, width, height)
    p1 = np.array(coordinates_thumb[0])
    p2 = np.array(coordinates_thumb[1])
    p3 = np.array(coordinates_thumb[2])

    l1 = np.linalg.norm(p2 - p3)
    l2 = np.linalg.norm(p1 - p3)
    l3 = np.linalg.norm(p1 - p2)

    cos_angle = (l1 ** 2 + l3 ** 2 - l2 ** 2) / (2 * l1 * l3)
    cos_angle = max(-1, min(1, cos_angle))  # Asegura que cos_angle esté en el rango [-1, 1]
    angle = degrees(acos(cos_angle))

    thumb_finger = np.array(False)
    if angle > 150:
        thumb_finger = np.array(True)
    return thumb_finger

# def  thumb()

with mp_hands.Hands(
    model_complexity=1,
    max_num_hands=1,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5) as hands:
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        
        frame = cv2.flip(frame, 1)
        height, width, _ = frame.shape
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results:NamedTuple = hands.process(frame_rgb)
        difDedos=np.array([False,False,False,False,False])
        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                thumb_value= thumb(hand_landmarks.landmark,width,height)
                #Coordenadas palms
                cords_palms= getPoints([0,1,2,5,9,13,17], hand_landmarks.landmark, width, height)
                cords_fingertips= getPoints([8, 12, 16,20], hand_landmarks.landmark, width, height)
                cords_finger_base= getPoints([6, 10, 14,18], hand_landmarks.landmark, width, height)
                #Cuatro dedos
                nx,ny= getCentroid(cords_palms)
                cv2.circle(frame,(nx,ny),3,(0,255,0),2)
                difDedos= getDedosCerrados([nx,ny],cords_fingertips,cords_finger_base)
                difDedos=np.append(thumb_value,difDedos)
                mp_drawing.draw_landmarks(
                    image=frame,
                    landmark_list=hand_landmarks,
                    connections=mp_hands.HAND_CONNECTIONS,
                    landmark_drawing_spec=mp_drawing_styles.get_default_hand_landmarks_style(),
                    connection_drawing_spec=mp_drawing_styles.get_default_hand_connections_style())
        printDedos(difDedos, frame)
        sendData(difDedos)
        cv2.imshow('Hand Tracking', frame)
        
        # Presiona ESC para salir
        if cv2.waitKey(1) & 0xFF == 27:
            ser.close()
            break

cap.release()
cv2.destroyAllWindows()
