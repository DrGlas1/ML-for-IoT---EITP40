import pandas as pd
import numpy as np
import os
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, MaxPooling2D, Flatten, Dense, Dropout
from tensorflow.keras.optimizers import Adam
from sklearn.model_selection import train_test_split

IMAGE_SIZE = (144, 176)
BATCH_SIZE = 32
EPOCHS = 10


def load_data(data_folder, labels_csv):
    labels_df = pd.read_csv(labels_csv)
    images = []
    labels = []

    for _, row in labels_df.iterrows():
        img_path = os.path.join(data_folder, row["filename"])
        label = row["label"]
        img = tf.keras.preprocessing.image.load_img(img_path, color_mode="grayscale", target_size=IMAGE_SIZE)
        img_array = tf.keras.preprocessing.image.img_to_array(img) / 255.0

        images.append(img_array)
        labels.append(label)
    X = np.array(images, dtype=np.float32)
    y = np.array(labels, dtype=np.float32)

    return X, y


def create_cnn(input_shape):
    model = Sequential([
        Conv2D(32, (3, 3), activation="relu", input_shape=input_shape),
        MaxPooling2D(pool_size=(2, 2)),
        Conv2D(64, (3, 3), activation="relu"),
        MaxPooling2D(pool_size=(2, 2)),
        Flatten(),
        Dense(128, activation="relu"),
        Dropout(0.5),
        Dense(1, activation="sigmoid")
    ])

    model.compile(optimizer=Adam(learning_rate=0.001),
                  loss="binary_crossentropy",
                  metrics=["accuracy"])
    return model


def plot_history(history):
    accuracy = history.history['accuracy']
    val_accuracy = history.history['val_accuracy']
    loss = history.history['loss']
    val_loss = history.history['val_loss']
    epochs = range(1, len(accuracy) + 1)
    plt.figure(figsize=(12, 5))
    plt.subplot(1, 2, 1)
    plt.plot(epochs, accuracy, label='Training', marker='o')
    plt.plot(epochs, val_accuracy, label='Validation', marker='o')
    plt.title('Accuracy')
    plt.legend()
    plt.subplot(1, 2, 2)
    plt.plot(epochs, loss, label='Training', marker='o')
    plt.plot(epochs, val_loss, label='Validation', marker='o')
    plt.title('Loss')
    plt.legend()
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    pretrain_folder = "../dataset/pretrain"
    labels_csv = "../dataset/labels.csv"

    X, y = load_data(pretrain_folder, labels_csv)

    X_train, X_validation, y_train, y_validation = train_test_split(X, y, test_size=0.2, random_state=42)
    model = create_cnn(input_shape=(IMAGE_SIZE[0], IMAGE_SIZE[1], 1))

    history = model.fit(X_train, y_train,
                        validation_data=(X_validation, y_validation),
                        epochs=EPOCHS,
                        batch_size=BATCH_SIZE)
    loss, accuracy = model.evaluate(X_validation, y_validation)
    print(f"Performance: loss = {loss}, accuracy = {accuracy}")
    plot_history(history)
