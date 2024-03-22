import numpy as np

def conv2d2(image, kernel, bias, stride=(1, 1), padding=(0, 0)):
    """
    Argumentit:
    - image: 3D numpy array sisääntulevasta kuvasta (height, width, channels).
    - kernel: 3D numpy array kernelistä (height, width, channels).
    - stride: Tuple jonka mukaan liikutaan kuvan päällä (default: (1, 1)).
    - padding: Tuple jonka mukaan lisätään nollia reunoille (default: (0, 0)).

    Vielä selvitettävää mitä arvoja ja arrayta sun muuta tulee edelliseltä funktiolta.
    """

    # Argumenttien keräys
    image_height, image_width, image_channels = image.shape
    kernel_height, kernel_width, _ = kernel.shape
    stride_height, stride_width = stride
    padding_height, padding_width = padding

    # Lasketaan outputin koot, esim 32*32 kuvan outputit on 30 standardiarvoilla
    output_height = (image_height + 2 * padding_height - kernel_height) // stride_height + 1
    output_width = (image_width + 2 * padding_width - kernel_width) // stride_width + 1

    # Kuviin padding jos halutaan, tuloksen alustaminen
    padded_image = np.pad(image, ((padding_height, padding_height), (padding_width, padding_width), (0, 0)), mode='constant')
    result = np.zeros((output_height, output_width, len(bias)))

    # Konvoluutio
    for height in range(0, output_height, stride_height):
        for width in range(0, output_width, stride_width):
            # Region of interest kuvasta
            roi = padded_image[height:height + kernel_height, width:width + kernel_width, :]

            # Kertominen ja yhteenlasku
            conv_result = np.sum(roi * kernel, axis=(0, 1)) + bias

            # Tulos outputtiin
            result[height // stride_height, width // stride_width, :] = conv_result

    return result

def testaus():
    # Random testausdataa
    image = np.random.rand(16, 16, 3)  # Esimerkki 32x32x3 random kuva
    kernel = np.random.rand(3, 3, 3)    # Esimerkki 3x3x3 random kernel
    stride = (1, 1)                     # Esimerkki stride
    padding = (0, 0)                    # Esimerkki padding
    bias = [1000, 2000, 3000]           # Esimerkki bias

    # Printtaa mitä haluat
    print("Image:",image.shape, '\n')
    print(image)
    result = conv2d2(image, kernel, bias, stride, padding)
    print("Convolution result shape:", result.shape, '\n')
    #print("Kernel:", kernel, '\n')
    print("Result:",result)

if __name__ == "__main__":
    testaus()
