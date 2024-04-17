from enum import Flag, auto
import sys
from typing import TextIO

from keras import Layer, Model
from numpy import ndarray

from model_io import import_model, get_weights_shape


class LayerMap:
    layers: dict[str, dict] = dict()
    offset = 0

    def add_layer(self, layer_name: str, weights: list):
        shape = get_weights_shape(weights)
        if len(weights) == 0:
            pass
        elif (layer_name != "normalization"):
            kernel_offset = self.offset
            bias_offset = kernel_offset + len(weights[0].flat) * 2
            self.layers[layer_name] = {
                'kernel': weights[0].flat,
                'kernel_offset': kernel_offset,
                'kernel_shape': shape[0],
                'bias': weights[1].flat,
                'bias_offset': bias_offset,
                'bias_shape': shape[1],
            }
            self.offset = bias_offset + len(weights[1].flat) * 2
        else:
            kernel_offset = self.offset
            self.layers[layer_name] = {
                'kernel': [x for l in weights for x in (l if type(l) == ndarray else [l])],
                'kernel_offset': kernel_offset,
                'kernel_shape': (3, ),
            }
            self.offset = kernel_offset + len(weights) * 2

    def c_mem_map(self, file: TextIO):
        for k in self.layers:
            if (self.layers[k].get('kernel')):
                file.write(self.__map_part(k, 'kernel'))
                file.write('\n')
            if (self.layers[k].get('bias')):
                file.write(self.__map_part(k, 'bias'))
                file.write('\n')

    def __map_part(self, layer_name: str, part: str):
        layer = self.layers[layer_name]
        offset_hex = hex(layer.get(f'{part}_offset'))
        ret = f'const unsigned int {layer_name}_{part}_offset = {offset_hex};\n'
        part_len = len(layer.get(f'{part}'))
        ret += f'const unsigned int {layer_name}_{part}_len = {part_len};\n'
        ret += f'const size_t {layer_name}_{part}_shape[] = {{'
        for s in layer.get(f'{part}_shape'):
            ret += f'{s}, '
        ret += '0};\n'
        return ret

    def c_flash_map(self, file: TextIO):
        file.write("""#ifndef WEIGHTS_H__
#define WEIGHTS_H__
#include <stddef.h>
#include "layer.h"
""")

        for k in self.layers:
            layer = self.layers[k]
            if (layer.get('kernel')):
                file.write(self.__flash_part(layer, k, 'kernel'))
            if (layer.get('bias')):
                file.write(self.__flash_part(layer, k, 'bias'))

        file.write(self.__hash_map())
        file.write("#endif //WEIGHTS_H__")

    def __flash_part(self, layer: dict, name: str, part: str):
        ret = f'const _Float16 {name}_{part}[] = {{\n'
        for i, v in enumerate(layer.get(f'{part}', [])):
            if (i % 6 == 0 and i != 0):
                ret += '\n'
            ret += f'{str(v):>15}, '
        ret = ret.removesuffix(', ')
        return ret + '};\n'

    def count_layers(self):
        _len = 0
        for l in self.layers:
            if (self.layers[l].get('kernel')):
                _len += 1
            if (self.layers[l].get('bias')):
                _len += 1
        return _len

    def __hash_map(self):
        ret = "\n"
        ret += 'struct layer_map map[256] = {\n'
        for l in self.layers:
            ret += self.__hash_layer(l)
        ret = ret.removesuffix(',\n')
        ret += '\n};\n'
        return ret

    def __layer_name_hash(slef, name: str):
        ret = 0
        for c in bytes(name, 'utf-8'):
            ret += c
        return ret.to_bytes(4, 'little')[0]

    def __hash_layer(self, layer_name):
        ret = ""
        layer = self.layers[layer_name]
        _hash = self.__layer_name_hash(layer_name)

        ret += f'[{hex(_hash)}] = {{'
        if layer.get('kernel'):
            klen = len(layer.get('kernel'))
            ret += f"""
            .kernel = {layer_name}_kernel,
            .klen = {klen},"""
        else:
            ret += f"""
            .kernel = NULL,
            .klen = 0,
            """

        if layer.get('bias'):
            blen = len(layer.get('bias'))
            ret += f"""
            .bias = {layer_name}_bias,
            .blen = {blen},"""
        else:
            ret += f"""
            .bias = NULL,
            .blen = 0,"""
        ret += "\n},\n"
        return ret

    def toml_map(self, file: TextIO):
        for k in self.layers:
            file.write(f"[{k}]\n")
            file.write(f'name = "{k}"\n')
            file.write(self.__toml_part(k, 'kernel'))
            file.write(self.__toml_part(k, 'bias'))
            file.write('\n')

    def __toml_part(self, layer_name: str, part: str):
        shape = self.layers[layer_name].get(f'{part}_shape', (0,))
        str_shape = ", ".join(str(x) for x in shape)
        ret = f'{part[0]}shape = [{str_shape}, 0]\n'
        return ret

    def __str_part(self, layer: dict, part: str):
        part_len = len(layer.get(part, []))
        part_offset = layer.get(f'{part}_offset', None)
        offset_hex = hex(part_offset) if part_offset != None else None
        ret = f'\t\t{part}_offset: {offset_hex}\n'
        ret += f'\t\t{part}_len: {part_len}\n'
        return ret

    def __str__(self):
        content = '{\n'
        for k in self.layers:
            content += f'\t{k}:\n'
            layer = self.layers[k]
            content += self.__str_part(layer, 'kernel')
            content += self.__str_part(layer, 'bias')
        content += '}'
        return content


def get_layer_map(model: Model):
    lMap = LayerMap()
    l: Layer
    for l in model.layers:
        lMap.add_layer(l.name, l.get_weights())
    return lMap


# Main execution


class Files(Flag):
    HEADER = auto()
    CONF = auto()
    ALL = HEADER | CONF


def parse_command(cmd: str):
    match cmd.lower():
        case 'all':
            return Files.ALL
        case 'header':
            return Files.HEADER
        case 'conf':
            return Files.CONF
        case _:
            return Files.ALL


def main(argc: int, argv: list[str]):
    if argc < 2:
        sys.stderr.write("Error no model specified\n")
        exit(-1)
    model = import_model(argv[1])
    lMap = get_layer_map(model)
    print(lMap)

    try:
        files = parse_command(argv[2])
    except IndexError:
        files = Files.ALL

    if files & Files.HEADER:
        with open("weights.h", "w") as file:
            lMap.c_flash_map(file)
    if files & Files.CONF:
        with open("map.toml", "w") as file:
            lMap.toml_map(file)


if __name__ == "__main__":
    main(len(sys.argv), sys.argv)
