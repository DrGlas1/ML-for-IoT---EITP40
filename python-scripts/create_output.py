import os
import csv
import numpy as np
from PIL import Image
from sklearn.model_selection import train_test_split

image_folder = "../dataset/online_images"
label_file = "../dataset/online_labels.csv"
output_file = "../model/online_data.h"
IMAGE_SIZE = (25, 30)
TRAIN_RATIO = 0.75
VAL_RATIO = 0.2
TEST_RATIO = 0.05


def process_images(image_folder, filenames, image_size):
    data = []
    for filename in filenames:
        img_path = os.path.join(image_folder, filename)
        img = Image.open(img_path).convert('L')
        resized_img = img.resize(image_size)
        img_array = np.array(resized_img).flatten()
        data.append(img_array)
    return np.array(data)


def write_to_h_file(output_file, train_data, train_labels, val_data, val_labels, test_data, test_labels, image_size):
    size = image_size[0] * image_size[1]
    with open(output_file, 'w') as f:
        f.write(f"const int first_layer_input_cnt = {size};\n")
        f.write(f"const int train_data_cnt = {len(train_labels)};\n")
        f.write(f"const int validation_data_cnt = {len(val_labels)};\n")
        f.write(f"const int test_data_cnt = {len(test_labels)};\n")
        f.write(f"const int classes_cnt = {len(set(train_labels))};\n\n")

        f.write("const int train_labels[] = {\n")
        f.write(", ".join(map(str, train_labels)))
        f.write("\n};\n")

        f.write("const int validation_labels[] = {\n")
        f.write(", ".join(map(str, val_labels)))
        f.write("\n};\n")

        f.write("const int test_labels[] = {\n")
        f.write(", ".join(map(str, test_labels)))
        f.write("\n};\n")

        f.write(f"const float train_data[{len(train_labels)}][{size}] = {{\n")
        for row in train_data:
            row_str = ", ".join(map(lambda x: f"{x:.6f}", row))
            f.write(f"  {{{row_str}}},\n")
        f.write("};\n")

        f.write(f"const float validation_data[{len(val_labels)}][{size}] = {{\n")
        for row in val_data:
            row_str = ", ".join(map(lambda x: f"{x:.6f}", row))
            f.write(f"  {{{row_str}}},\n")
        f.write("};\n\n")

        f.write(f"const float test_data[{len(test_labels)}][{size}] = {{\n")
        for row in test_data:
            row_str = ", ".join(map(lambda x: f"{x:.6f}", row))
            f.write(f"  {{{row_str}}},\n")
        f.write("};\n")


def main():
    labels = []
    filenames = []
    with open(label_file, 'r') as f:
        reader = csv.reader(f)
        next(reader)
        for row in reader:
            filenames.append(row[0])
            labels.append(int(row[1]))

    data = process_images(image_folder, filenames, IMAGE_SIZE)
    data = data / 255.0

    train_data, temp_data, train_labels, temp_labels = train_test_split(
        data, labels, test_size=(1 - TRAIN_RATIO)
    )
    val_data, test_data, val_labels, test_labels = train_test_split(
        temp_data, temp_labels,
        test_size=(TEST_RATIO / (VAL_RATIO + TEST_RATIO))
    )
    write_to_h_file(output_file, train_data, train_labels, val_data, val_labels, test_data, test_labels, IMAGE_SIZE)


if __name__ == "__main__":
    main()
