import numpy as np

data=np.random.randint(5, size=(1, 10, 10, 1))

def maxpooling2d(input_data, pool_size, strides):
    """
    Maxpooling2D function.

    Parameters:
        input_data (ndarray): Input data, 4D array [batch_size, height, width, channels].
        pool_size (tuple): Pooling window size, (pool_height, pool_width).
        strides (tuple): Stride size, (stride_height, stride_width).

    Returns:
        ndarray: Output data after maxpooling.
    """
    batch_size, input_height, input_width, num_channels = input_data.shape
    pool_height, pool_width = pool_size
    stride_height, stride_width = strides

    # Calculate output dimensions
    output_height = (input_height - pool_height) // stride_height + 1
    output_width = (input_width - pool_width) // stride_width + 1

    # Initialize output array
    output_data = np.zeros((batch_size, output_height, output_width, num_channels))

    # Perform max pooling
    for b in range(batch_size):
        for h in range(output_height):
            for w in range(output_width):
                for c in range(num_channels):
                    # Determine the pooling window
                    h_start = h * stride_height
                    h_end = h_start + pool_height
                    w_start = w * stride_width
                    w_end = w_start + pool_width

                    # Extract the region of interest
                    roi = input_data[b, h_start:h_end, w_start:w_end, c]

                    # Apply maxpooling
                    output_data[b, h, w, c] = np.max(roi)

    return output_data

# Generate random input data
input_data = np.array(data)#np.random.randint(2, size=(1, 4, 4, 1)) # Assuming batch_size=1, height=4, width=4, channels=1

print("Input data:")
print(input_data)

# Define pooling window size and stride
pool_size = (2, 2)
strides = (2, 2)

# Perform maxpooling
output_data = maxpooling2d(input_data, pool_size, strides)

print("\nOutput after max pooling:")
print(output_data)