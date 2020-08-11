import cv2
import serial
from threading import Timer
import time

faceCascade = cv2.CascadeClassifier( cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

#Camera Settings
frameWidth = 160
frameHeight = 120
frameBrightness = 25

#How much space left for bounds
padding = 50
wCenter = frameWidth
hCenter = frameHeight

cap = cv2.VideoCapture(0)
cap.set(3, frameWidth)
cap.set(4, frameHeight)
cap.set(10, frameBrightness)

#Serial
#port = '/dev/arduino'
#ser = serial.Serial(port, 9600, timeout=1, write_timeout = 1)
#ser.flush()

#Timer Warning (Seconds)
faceTime = 3
triggerTime = 0.5

trigger = 0

# Timer that makes sure face is detected for at least 5 seconds (starts trigger)
def timeout():
    global trigger
    trigger = 1
    triggerTimer = Timer(triggerTime, triggerTimeOut)
    triggerTimer.start()

detectionTimer = Timer(faceTime , timeout)

#Timer that stops the trigger
def triggerTimeOut():
    global trigger
    trigger = 0
    
triggerTimer = Timer(triggerTime, triggerTimeOut)


while True:
    #Read camera image
    success, img = cap.read()

    #Convert into gray image
    imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    #Detect faces in gray image
    faces = faceCascade.detectMultiScale(imgGray, 1.1, 4)

    #Face detected coordinates
    xPos = wCenter
    yPos = hCenter

    #Variables to send to arduino
    #Final servo move request
    #xLeft 1, xRight 2, yUp 1, yDown 2, no move 0 (AKA IN CENTER)
    #If values stay as -9 means face not found
    xServo = -9
    yServo = -9

    for (x, y, w, h) in faces:
        cv2.rectangle(img, (x,y), (x+w, y+h), (255,0,0), 2)
        
        #Found face assume it was on center
        xServo = 0
        yServo = 0
        
        #Start a timer if there hasnt been one
        if(not detectionTimer.is_alive()):
            detectionTimer = Timer(faceTime , timeout)
            detectionTimer.start()

        #Get center point of detection
        xPos = int((x + x + w) / 2)
        yPos = int((y + y + h) / 2)
        
    #Check if point goes outside of bounds
    if(xPos < wCenter - padding):
        xServo = 1
        #print("Move Left")    
            
    if(xPos > wCenter + padding):
        xServo = -1
        #print("Move Right")
        
    if(yPos < hCenter - padding):
        yServo = -1
        #print("Move Up")
            
    if(yPos > hCenter + padding):
        yServo = 1
        #print("Move Down")
            
    #Send final message to serial device (A and F represent start and end of message)
    stringMsg =  "A" + str(xServo) + "," + str(yServo) + "," + str(trigger) + "F"
    print(stringMsg)
    
#     ser.write(stringMsg.encode())
    #Ex: True,1,0 or False,1,1 ...
    
    if(detectionTimer.is_alive()):
        if(xServo != 0 or yServo != 0):
            detectionTimer.cancel()
            #print("CANCELED");
    
    #Debugging Tools start from here

    #Rectangle padding bound (Debugging)
    pX = wCenter - (padding)
    pY = hCenter - (padding)
    pX2 = wCenter + (padding)
    pY2 = hCenter + (padding)
    
    cv2.rectangle(img, (pX,pY), (pX2, pY2), (0,255,0), 2)
    cv2.circle(img,(wCenter, hCenter), 10, (255,0,0), -1)

    #Face detection center point (Debugging)
    cv2.circle(img,(int(xPos), int(yPos)), 10, (0,255,0), -1)

    #Display Image with detection objects (Debugging)
    cv2.imshow("Result", img)

    #Key q can be pressed to quit program
    if cv2.waitKey(1) & 0xFF == ord('q'):
#         ser.flush();
        break
