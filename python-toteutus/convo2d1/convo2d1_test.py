#!/usr/bin/python3

# version 0.01 2024-03-21 AK
# test version, includes dummy data, runnable as-is

# inputs: 1 2d list as source, 1 3d list as kernels, optional 1 1d list of bias values
# output: 3d list
# no strides or padding
# source and kernel sizes detected automatically

DEBUG = 1  # debug level 0 - 2
from pprint import pprint  # for neater debug output

kuva1 = [
    [1,2,3,1,2,3,1,2,3],
    [4,5,6,4,5,6,4,5,6],
    [7,8,9,7,8,9,7,8,9],
    [1,2,3,1,2,3,1,2,3],
    [4,5,6,4,5,6,4,5,6],
    [7,8,9,7,8,9,7,8,9],
    [1,2,3,1,2,3,1,2,3],
    [4,5,6,4,5,6,4,5,6],
    [7,8,9,7,8,9,7,8,9]]
kuva2 = [
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1],
    [1,1,1,1,1,1,1,1,1]]
kernel1 = [[1,1,1],
           [1,1,1],
           [1,1,1]]
kernel2 = [[2,2,2],
           [2,2,2],
           [2,2,2]]
kernel3 = [[3,3,3],
           [3,3,3],
           [3,3,3]]
kernel4 = [[4,4,4],
           [4,4,4],
           [4,4,4]]
kernelit=[kernel1, kernel2, kernel3, kernel4]
biaslist = [1000, 2000, 3000, 4000]

# takes a source image, list of 2d kernels, optional bias value
def convo2d1(source, kernel_list, bias=None):
    """
    convo2d1(source, kernel_list, bias=None)
    source as 2d python list,
    kernel_list as 3d python list,
    bias as 1d python list, default 0

    returns 3d python list
    """
    sourcex = len(source[0])
    sourcey = len(source)
    kernelx = len(kernel_list[0][0])
    kernely = len(kernel_list[0])
    kernel_list_count = len(kernel_list)
    if bias != None:  # handle optional bias
        bias_count = len(bias)
        if kernel_list_count != bias_count:
            print("ERROR: kernel and bias count mismatch,", kernel_list_count, "kernels,", bias_count, "bias values")
            return "ERROR: kernel and bias count mismatch"
    if DEBUG >= 1: print("Got",sourcex, "x", sourcey, "source image")
    if DEBUG >= 1: print("Got",kernel_list_count, "kernels in kernel_list")
    if DEBUG >= 1: print("Got",kernelx, "x", kernely, "kernel size from first kernel")

    # create an empty 2d output list for each kernel
    output = [None for _ in range(kernel_list_count)]  # generate number of output lists equal to number of kernels
    for i in range((len(output))):  # generate y values for each output list
        output[i] = [None for _ in range(sourcey-(kernely-1))]
        for j in range((len(output[i]))):  # generate x values for each y
            output[i][j] = [None for _ in range(sourcex-(kernelx-1))]
    if DEBUG >= 1: print("Made", len(output), "pieces of" ,len(output[0][0]), "x", len(output[0]), "output image(s)")

    for kernel_n in range(kernel_list_count):  # iterate over output lists and kernels in kernel list
        output_y = 0
        while output_y < len(output[0]):  # iterate over output list y values
            output_x = 0
            while output_x < len(output[0][0]):  # iterate over output list x values
                temp = 0
                for kernel_y in range(kernely):  # iterate over kernel cells
                    for kernel_x in range(kernelx):
                        temp = temp + source[output_y][output_x] * kernel_list[kernel_n][kernel_y][kernel_x]
                if DEBUG >= 2: print("output_y=", output_y, " output_x=", output_x, " kernel_n=", kernel_n, " kernel_y=", kernel_y, " kernel_x=", kernel_x, "inserting", temp, "to", "output[", kernel_n, "][", output_y, "][", output_x, "]")
                if bias == None:
                    output[kernel_n][output_y][output_x] = temp  # write result to output cell
                else:
                    output[kernel_n][output_y][output_x] = temp + bias[kernel_n]  # add bias from same bias list index as output list index
                output_x = output_x + 1  # if we had strides they would go here
            output_y = output_y + 1  # if we had strides they would go here
    return output

def showkernels():
    print("Kernels:")
    for n in kernelit:
        print(n)

print("Result:")
pprint(convo2d1(kuva2, kernelit, biaslist))
print("\n\n")
