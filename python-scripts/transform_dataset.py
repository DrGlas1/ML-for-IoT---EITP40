import os
import random

import cv2
import numpy as np
import pandas as pd
from itertools import cycle
from PIL import Image
import pillow_avif


input_folder = "../dataset"
output_folder = os.path.join(input_folder, "../dataset/la_ferrari_and_optimus")
label_file = os.path.join(input_folder, "../dataset/la_ferrari_and_optimus.csv")
target_size = (176, 144)

os.makedirs(output_folder, exist_ok=True)
class_1_folder = os.path.join(input_folder, "la_ferrari")
class_2_folder = os.path.join(input_folder, "optimus")

class_1_images = [os.path.join(class_1_folder, f) for f in os.listdir(class_1_folder)]
class_2_images = [os.path.join(class_2_folder, f) for f in os.listdir(class_2_folder)]

mixed_dataset_images = list(zip(cycle(class_1_images), class_2_images))[:min(len(class_1_images), len(class_2_images))]
mixed_dataset_images = [img for pair in mixed_dataset_images for img in pair]

random.shuffle(mixed_dataset_images)

labels = [0 if img.startswith(class_1_folder) else 1 for img in mixed_dataset_images]

label_data = []
for i, img_path in enumerate(mixed_dataset_images):
    image = Image.open(img_path)
    image = image.convert("RGB")
    image_np = np.array(image)  # Convert to numpy array
    grayscale_image = cv2.cvtColor(image_np, cv2.COLOR_RGB2GRAY)  # Use COLOR_RGB2GRAY since image was in RGB mode
    resized_image = cv2.resize(grayscale_image, target_size)
    normalized_image = resized_image / 255.0

    output_filename = f"{i:05d}.png"
    output_path = os.path.join(output_folder, output_filename)
    cv2.imwrite(output_path, (normalized_image * 255).astype(np.uint8))

    label_data.append((output_filename, labels[i]))

label_df = pd.DataFrame(label_data, columns=["filename", "label"])
label_df.to_csv(label_file, index=False)
