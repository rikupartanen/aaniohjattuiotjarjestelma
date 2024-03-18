import sys
from keras import Model, models


def import_model(filepath: str) -> Model:
    model: Model = models.load_model(filepath)
    return model


def export_model(model: Model):
    model.save("model_export.keras")


def get_layer_weights(layer: str, model: Model):
    return model.get_layer(layer).get_weights()


if __name__ == "__main__":
    if len(sys.argv) >= 3:
        if sys.argv[1] == "summary":
            model = import_model(sys.argv[2])
            print(model.summary())
            exit(0)

    if len(sys.argv) < 2:
        print("No model file specified")
        exit(-1)

    model = import_model(sys.argv[1])
    print(get_layer_weights("normalization", model))
