import os

def relabel_yolo_labels_to_class_zero(label_dir):
    count = 0
    for root, _, files in os.walk(label_dir):
        for file in files:
            if file.endswith(".txt"):
                label_path = os.path.join(root, file)
                with open(label_path, "r") as f:
                    lines = f.readlines()

                new_lines = []
                for line in lines:
                    parts = line.strip().split()
                    if len(parts) == 5:
                        parts[0] = '0'  # Replace class index
                        new_lines.append(" ".join(parts) + "\n")

                with open(label_path, "w") as f:
                    f.writelines(new_lines)

                count += 1

    print(f"Relabeled {count} label files to class 0.")

relabel_yolo_labels_to_class_zero("./train/labels")
relabel_yolo_labels_to_class_zero("./test/labels")
relabel_yolo_labels_to_class_zero("./valid/labels")
