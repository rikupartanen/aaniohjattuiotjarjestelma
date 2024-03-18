import sys
from tensorflow import keras

def import_model(filepath: str):
    model = keras.models.load_model(filepath)
    return model

def export_model(model: [keras.Model]):
    model.save("model_export.keras")

if __name__ == "__main__":
    if len(sys.argv) >= 3:
        if sys.argv[1] == "summary":
            model = import_model(sys.argv[2])
            print(model.summary())
            exit()

    if len(sys.argv) < 2:
        print("No model file specified")
        exit(-1)
    import_model(sys.argv[1])

