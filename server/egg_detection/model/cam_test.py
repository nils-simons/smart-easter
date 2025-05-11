from ultralytics import YOLO

model = YOLO('runs/detect/train/weights/best.pt')

model.predict(source=0, show=True)