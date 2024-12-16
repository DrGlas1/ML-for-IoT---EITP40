import csv
import os
import numpy as np
from PIL import Image
from matplotlib import pyplot as plt
from sklearn.model_selection import train_test_split
from tensorflow import keras

image_folder = "../dataset/online_images_2"
label_file = "../dataset/online_labels_2.csv"
output_file = "../model/online_data_2.h"
IMAGE_SIZE = (144, 172)
TRAIN_RATIO = 0.85
VAL_RATIO = 0.1
TEST_RATIO = 0.05
EPOCHS = 30
INPUT_SIZE = 75


def view_history(history):
    plt.figure(figsize=(14, 6))
    plt.subplot(1, 2, 1)
    plt.title('Model Accuracy')
    plt.xlabel('Epochs')
    plt.ylabel('Accuracy')
    plt.plot(history.history['accuracy'], label='train')
    plt.plot(history.history['val_accuracy'], label='validation')
    plt.legend()

    plt.subplot(1, 2, 2)
    plt.title('Model loss')
    plt.ylabel('Loss')
    plt.xlabel('Epochs')
    plt.plot(history.history['loss'], label="train")
    plt.plot(history.history['val_loss'], label="validation")
    plt.show()


def load_images(image_folder, filenames):
    data = []
    for filename in filenames:
        img_path = os.path.join(image_folder, filename)
        img = Image.open(img_path).convert('L')
        img_array = np.array(img)
        img_array = img_array.reshape((img_array.shape[0], img_array.shape[1], 1))
        data.append(img_array)
    return np.array(data)


def write_to_h_file(output_file, train_data, train_labels, val_data, val_labels, test_data, test_labels, size):
    with open(output_file, 'w') as f:
        f.write(f"const int first_layer_input_cnt = {size};\n")
        f.write(f"const int train_data_cnt = {len(train_labels)};\n")
        f.write(f"const int validation_data_cnt = {len(val_labels)};\n")
        f.write(f"const int test_data_cnt = {len(test_labels)};\n")
        f.write(f"const int classes_cnt = {len(set(train_labels))};\n\n")

        f.write("const int train_labels[train_data_cnt] = {\n\t")
        f.write(", ".join(map(str, train_labels)))
        f.write("\n};\n")

        f.write("const int validation_labels[validation_data_cnt] = {\n\t")
        f.write(", ".join(map(str, val_labels)))
        f.write("\n};\n")

        f.write("const int test_labels[test_data_cnt] = {\n\t")
        f.write(", ".join(map(str, test_labels)))
        f.write("\n};\n")

        f.write(f"const float train_data[train_data_cnt][first_layer_input_cnt] = {{\n")
        for row in train_data:
            row_str = ", ".join(map(lambda x: f"{x:.6f}", row))
            f.write(f"  {{{row_str}}},\n")
        f.write("};\n")

        f.write(f"const float validation_data[validation_data_cnt][first_layer_input_cnt] = {{\n")
        for row in val_data:
            row_str = ", ".join(map(lambda x: f"{x:.6f}", row))
            f.write(f"  {{{row_str}}},\n")
        f.write("};\n\n")

        f.write(f"const float test_data[test_data_cnt][first_layer_input_cnt] = {{\n")
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
    data = load_images(image_folder, filenames)
    data = data / 255

    train_data, temp_data, train_labels, temp_labels = train_test_split(
        data, labels, test_size=(1 - TRAIN_RATIO)
    )
    val_data, test_data, val_labels, test_labels = train_test_split(
        temp_data, temp_labels,
        test_size=(TEST_RATIO / (VAL_RATIO + TEST_RATIO))
    )

    model = keras.models.Sequential([
        keras.layers.Input(shape=train_data.shape[1:]),
        keras.layers.Conv2D(8, (3, 3), activation="relu"),
        keras.layers.MaxPool2D((3, 3)),
        keras.layers.Conv2D(16, (2, 2), activation="relu"),
        keras.layers.MaxPool2D((3, 3)),
        keras.layers.Conv2D(8, (3, 3), activation="relu"),
        keras.layers.MaxPool2D((3, 3)),
        keras.layers.Flatten(),
        keras.layers.Dense(75, activation="relu"),
        keras.layers.Dense(35, activation="relu"),
        keras.layers.Dense(15, activation="relu"),
        keras.layers.Dense(2, activation="softmax")
    ])
    cnt_dynamic_layers = 3
    model.compile(optimizer="adam", loss="sparse_categorical_crossentropy", metrics=["accuracy"])
    history = model.fit(train_data, np.array(train_labels), epochs=EPOCHS, validation_data=(val_data, np.array(val_labels)))
    view_history(history)
    x = model.layers[-(cnt_dynamic_layers + 1)].output
    model = keras.Model(inputs=model.inputs, outputs=x)
    cnn_pred_train = model.predict(train_data)
    cnn_pred_validation = model.predict(val_data)
    cnn_pred_test = model.predict(test_data)
    write_to_h_file(output_file, cnn_pred_train, train_labels, cnn_pred_validation, val_labels,
                    cnn_pred_test, test_labels, INPUT_SIZE)


if __name__ == "__main__":
    main()
