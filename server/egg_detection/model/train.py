from ultralytics import YOLO

def train():
    model = YOLO('yolov8n.pt')  # or yolov8s.pt, yolov8m.pt, etc.
    model.train(data='data.yaml', epochs=50, imgsz=640, batch=16)

if __name__ == '__main__':
    train()
