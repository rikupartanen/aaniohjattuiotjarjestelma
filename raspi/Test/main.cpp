#include <iostream>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

extern "C" {
#include "src/common.h"
#include "src/model.h"
#include "src/sample.h"
#include "FFTi.h"
}

int main() {
  std::cout << "Hello World!" << std::endl;
  // PRINT_ARR(sample, 16000, "Sample:");
  float *spec = (float*)malloc(16000 * sizeof(float));
  FFTi(sample, (float(*)[129])spec);

  struct tensor *input;
  {
    input = (struct tensor*)malloc(sizeof(struct tensor));
    input->vec = spec;
    input->shape = (size_t*)malloc(3 * sizeof(size_t));

    input->shape[0] = 129;
    input->shape[1] = 124;
    input->shape[2] = 0;
  }
  // clang-format on

  struct tensor *out = model(input);
  if (!out) return -1;

  printf("OUT\n");
  _print_tensor(out);
  print_shape(out);

  free_tensor(out);

  if (!out) return -1;
  // return 0;

  const auto portsInfos = QSerialPortInfo::availablePorts();
  qInfo() << portsInfos.length();
  for (const QSerialPortInfo &port : portsInfos)
    qInfo() << "Port: " << port.portName() << "\n";

  return 0;
}
