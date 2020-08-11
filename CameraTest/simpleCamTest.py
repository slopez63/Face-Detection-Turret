import cv2


#Camera Settings
frameWidth = 320
frameHeight = 240
frameBrightness = 25


cap = cv2.VideoCapture(0)
#cap.set(3, frameWidth)
#cap.set(4, frameHeight)
#cap.set(10, frameBrightness)


if not (cap.isOpened()):
    print("Could not open video device")

while True:
    #Read camera image
    success, img = cap.read()
    
    print(img)
    
    # Our operations on the frame come here
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    cv2.imshow("Result", gray)
    
    #Key q can be pressed to quit program
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break