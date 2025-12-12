# interface/srv/MoveJoints.srv

int32[] ids          # <--- string[] names 에서 변경됨
float64[] positions  # 목표 위치 (Radian)
float64 velocity     # 이동 속도
