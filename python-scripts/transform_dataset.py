import os
import cv2
import numpy as np
import pandas as pd
from itertools import cycle


input_folder = "../dataset/arduino_cam"
output_folder = os.path.join(input_folder, "../arduino_images")
label_file = os.path.join(input_folder, "../arduino_labels.csv")
target_size = (144, 176)

os.makedirs(output_folder, exist_ok=True)
car_folder = os.path.join(input_folder, "car")
non_car_folder = os.path.join(input_folder, "non_car")

car_images = [os.path.join(car_folder, f) for f in os.listdir(car_folder)]
non_car_images = [os.path.join(non_car_folder, f) for f in os.listdir(non_car_folder)]

mixed_dataset_images = list(zip(cycle(car_images), non_car_images))[:min(len(car_images), len(non_car_images))]
mixed_dataset_images = [img for pair in mixed_dataset_images for img in pair]

labels = [0 if "non" in img else 1 for img in mixed_dataset_images]

label_data = []
for i, img_path in enumerate(mixed_dataset_images):
    image = cv2.imread(img_path)

    output_filename = f"{i:05d}.png"
    output_path = os.path.join(output_folder, output_filename)
    cv2.imwrite(output_path, image)

    label_data.append((output_filename, labels[i]))

label_df = pd.DataFrame(label_data, columns=["filename", "label"])
label_df.to_csv(label_file, index=False)
