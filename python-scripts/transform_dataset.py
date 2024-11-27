import os
import cv2
import numpy as np
import pandas as pd
from itertools import cycle


input_folder = "../dataset"
output_folder = os.path.join(input_folder, "pretrain")
label_file = os.path.join(input_folder, "labels.csv")
target_size = (144, 176)

os.makedirs(output_folder, exist_ok=True)
car_folder = os.path.join(input_folder, "car_scaled")
non_car_folder = os.path.join(input_folder, "non_car_scaled")

car_images = [os.path.join(car_folder, f) for f in os.listdir(car_folder)]
non_car_images = [os.path.join(non_car_folder, f) for f in os.listdir(non_car_folder)]

mixed_dataset = list(zip(cycle(car_images), non_car_images))[:min(len(car_images), len(non_car_images))]
mixed_dataset = [img for pair in mixed_dataset for img in pair]

labels = [0 if "non" in img else 1 for img in mixed_dataset]

label_data = []
n = len(mixed_dataset)
print(f"0/{n}")
for i, img_path in enumerate(mixed_dataset):
    image = cv2.imread(img_path)
    resized_image = cv2.resize(image, target_size)
    normalized_image = resized_image / 255.0

    output_filename = f"{i:05d}.png"
    output_path = os.path.join(output_folder, output_filename)
    cv2.imwrite(output_path, (normalized_image * 255).astype(np.uint8))
    label_data.append((output_filename, labels[i]))
    print(f"{i+1}/{n}")

label_df = pd.DataFrame(label_data, columns=["filename", "label"])
label_df.to_csv(label_file, index=False)
