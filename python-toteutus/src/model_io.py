import sys

import numpy as np
from keras import Model, models


def import_model(filepath: str) -> Model:
    model: Model = models.load_model(filepath)
    return model


def export_model(model: Model):
    model.save("model_export.keras")


def get_layer_weights(layer: str, model: Model) -> list[np.ndarray]:
    return model.get_layer(layer).get_weights()


def get_weights_shape(layer: list[np.ndarray]):
    tmp = []
    for i in layer:
        tmp.append(i.shape)
    return tmp


if __name__ == "__main__":
    argc = len(sys.argv)
    if argc >= 3:
        if sys.argv[1] == "summary":
            model = import_model(sys.argv[2])
            print(model.summary())
            exit(0)
        if sys.argv[1] == "layer" and argc >= 4:
            model = import_model(sys.argv[2])
            weights = get_layer_weights(sys.argv[3], model)
            print(model.get_layer(sys.argv[3]).get_build_config())
            print(get_weights_shape(weights))
            print(weights)
            exit(0)

    if len(sys.argv) < 2:
        print("No model file specified")
        exit(-1)

    model = import_model(sys.argv[1])
    print(get_layer_weights("dense", model))
