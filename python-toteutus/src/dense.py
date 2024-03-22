from functools import reduce
from typing import TypeVar
import numpy as np


T = TypeVar('T')  # Generic type T


def matMulList(vec: list, kernel: list[list]):
    """
    Matrix multiply input `vec` with input `kernel` and return the result.

    This implementation uses python built-in lists.
    """
    tmp = []
    for j in range(len(kernel[0])):
        _sum = 0
        for i in range(len(kernel)):
            _sum += kernel[i][j] * vec[i]
        tmp.append(_sum)
    return tmp


def matMul(vec: np.ndarray, mat: np.ndarray):
    """
    Matrix multiply input `vec` with input kernel `mat` and return the result.

    This implementation uses numpy `ndarrays` as input and returns built-in `list`.
    """
    tmp = []
    for j in range(mat.shape[1]):
        _sum = 0
        for i in range(mat.shape[0]):
            _sum += mat[i, j] * vec[i]
        tmp.append(_sum)
    return tmp


def dense(input: list[T], kernel: list[list[T]], bias: list[T] = None):
    """Calculate tensordot between `input` and `kernel` and return the result.

    If `bias` is provided add it to the result.

    Parameters:
    * `input` has to be a flat `list` of `int` or `float` elements.
    * `kernel` is a two dimensional `list` of elements of the same type as `input`
      and `kernel`'s first dimension has to be the same length as `input`.
    * `bias` is a `list` with the same type as `input` and the same length as `kernel`'s
      second dimension.
    """
    if len(input) != len(kernel):
        raise ValueError("Input shape does not match kernel shape")
    test_len = len(kernel[0])
    for r in kernel:
        if len(r) != test_len:
            raise ValueError("Kernel shape is not constant")
    tmp = matMulList(input, kernel)

    if type(bias) is list:
        if len(bias) != len(tmp):
            raise ValueError("Bias does not match with kernel output shape")
        for i in range(len(tmp)):
            tmp[i] += bias[i]
    return tmp


def np_dense(input: np.ndarray[T], kernel: np.ndarray[T], bias: np.ndarray[T] = None):
    """Calculate tensordot between `input` and `kernel` and return the result.

    If `bias` is provided add it to the result.

    Parameters:
    * `input` has to be a flat `ndarray` of `int` or `float` elements.
    * `kernel` is a two dimensional `ndarray` of elements of the same type as `input`
      and `kernel`'s first dimension has to be the same length as `input`.
    * `bias` is a `ndarray` with the same type as `input` and the same length as `kernel`'s
      second dimension.
    """
    if input.shape[0] != kernel.shape[0]:
        raise ValueError("Input shape does not match kernel shape")
    tmp = matMul(input, kernel)

    tmp = np.array(tmp)
    if type(bias) is np.ndarray:
        if bias.shape != tmp.shape:
            raise ValueError("Bias does not match with output")
        for i in range(len(tmp)):
            tmp[i] += bias[i]

    return tmp


def test_dense(input: np.ndarray[T], kernel: np.ndarray[T], bias: np.ndarray[T] = None):
    """Calculate tensordot between `input` and `kernel` and return the result.

    If `bias` is provided add it to the result.

    This implementation follows closely with tensorflow's dense function.\n
    For input of (None, a) and kernel of (a, b) return an output of (None, b).\n
    For input of (None, ..., a) and kernel of (a, b) return an output of (None, ..., b).

    Parameters:
    * `input` has to be a `ndarray` of `int` or `float` elements.
    * `kernel` is a two dimensional `ndarray` of elements of the same type as `input`
      and `kernel`'s first dimension has to be the same length as `input`'s last dimension.
    * `bias` is a `ndarray` with the same type as `input` and the same length as `kernel`'s
      second dimension.
    """
    if input.shape[-1] != kernel.shape[0]:
        raise ValueError("Input shape does not match kernel shape")

    input_mid = reduce(lambda x, y: x*y, input.shape[1:-1], 1)
    reshaped = input.reshape((input.shape[0], input_mid, input.shape[-1]))
    tmp = []
    for s in range(input.shape[0]):
        for m in range(input_mid):
            tmp.append(matMul(reshaped[s, m], kernel))

    tmp = np.asarray(tmp)
    if type(bias) is np.ndarray:
        if bias.shape != tmp[0].shape:
            raise ValueError("Bias does not match with output")
        for row in tmp:
            for i in range(len(row)):
                row[i] += bias[i]

    return tmp.reshape((*input.shape[:-1], kernel.shape[-1]))


if __name__ == "__main__":
    """Test and examples of all functions and their inputs"""

    test_kernel = [[1, 2, 3], [1, 2, 3], [1, 2, 3]]
    test_input = [1, 2, 3]
    test_bias = [1, 2, 3]

    print(dense(test_input, test_kernel, test_bias))

    np_test_kernel = np.array(test_kernel, dtype=np.int32)
    np_test_input = np.array(test_input, dtype=np.int32)
    np_test_bias = np.array(test_bias, dtype=np.int32)

    result = np_dense(np_test_input, np_test_kernel, np_test_bias)
    print(repr(result))

    np_test_input = np.array([[np_test_input, np_test_input], [
                             np_test_input, np_test_input]])

    result = test_dense(np_test_input, np_test_kernel)
    print("Result 1", repr(result), sep='\n')
    result = test_dense(np_test_input, np_test_kernel, np_test_bias)
    print("Result 2", repr(result), sep='\n')
