import cv2

# 웹캠 열기 (0은 기본 카메라)
cap = cv2.VideoCapture(0)

# 웹캠이 제대로 열렸는지 확인
if not cap.isOpened():
    print("웹캠을 열 수 없습니다.")
    exit()

print("웹캠이 연결되었습니다. 'q'를 누르면 종료됩니다.")

while True:
    # 프레임 읽기
    ret, frame = cap.read()
    
    # 프레임을 제대로 읽었는지 확인
    if not ret:
        print("프레임을 읽을 수 없습니다.")
        break
    
    # 프레임 화면에 출력
    cv2.imshow('Webcam', frame)
    
    # 'q' 키를 누르면 종료
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 리소스 해제
cap.release()
cv2.destroyAllWindows()
