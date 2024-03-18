# Requirements

TensorFlow - 2.16.1

# Model format

To read full model info execute this command in this folder
`python3 ../src/model_io.py summary <MODEL_NAME>`

The model was generated using the following code

```python
model = models.Sequential([
    layers.Input(shape=input_shape),
    # Downsample the input.
    layers.Resizing(32, 32),
    # Normalize.
    norm_layer,
    layers.Conv2D(32, 3, activation='relu'),
    layers.Conv2D(64, 3, activation='relu'),
    layers.MaxPooling2D(),
    layers.Dropout(0.25),
    layers.Flatten(),
    layers.Dense(64, activation='relu'),
    layers.Dropout(0.5),
    layers.Dense(num_labels),
])
```

Model layer names:

- resizing
- normalization
- conv2d
- conv2d_1
- max_pooling2d
- dropout
- flatten
- dense
- dropout_1
- dense_1
