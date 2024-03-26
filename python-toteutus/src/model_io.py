import sys

import numpy as np
from keras import Model, models, Layer


def import_model(filepath: str) -> Model:
    """Load model from file"""
    model: Model = models.load_model(filepath)
    return model


def export_model(model: Model):
    "Save model to file `model_export.keras`"
    model.save("model_export.keras")


def get_layer_weights(layer: str, model: Model) -> list[np.ndarray]:
    """Get layer weights from given model"""
    return model.get_layer(layer).get_weights()


def get_weights_shape(layer: list[np.ndarray]):
    tmp = []
    for i in layer:
        tmp.append(i.shape)
    return tmp


class DummyLayer(Layer):
    def __init__(self):
        super(DummyLayer, self).__init__()

    def call(self, inputs):
        return inputs


def get_reference_layer(layer: str, model: Model):
    """Split the model in to three parts start, layer and rest.

    Example:
    ```python
    model = import_model("../model/test_model.keras")
    (start, layer, end) = get_reference_layer("dense", model)

    result = start(waveform)
    layer_result = layer(result)
    print(layer_result)
    ```
    """
    modelStart = models.Sequential()
    selectedLayer: Layer
    modelEnd = models.Sequential()

    start = True
    l: Layer
    for l in model.layers:
        if l.name == layer:
            start = False
            selectedLayer = l
            continue
        if start:
            modelStart.add(l)
        else:
            modelEnd.add(l)

    if not len(modelStart.layers):
        modelStart.add(DummyLayer())
    if not len(modelEnd.layers):
        modelEnd.add(DummyLayer())

    modelStart.build(model.input_shape)
    modelEnd.build(selectedLayer.output.shape)
    return (modelStart, selectedLayer, modelEnd)


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

    # Example of geting reference layer
    # Argv should hold model path, can be replaced by string
    model = import_model(sys.argv[1])
    (start, layer, end) = get_reference_layer("resizing", model)

    print("Original input", model.input_shape)
    print("Start input", start.input_shape)
    print("Layer input", layer.input.shape)
    print("End input", end.input_shape)

    # To use the model parts, call the part with the input data
    # result = start(waveform)
    # layer_result = layer(result)
