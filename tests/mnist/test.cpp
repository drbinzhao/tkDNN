#include<iostream>
#include "tkdnn.h"

const char *input_bin   = "../tests/mnist/input.bin";
const char *c0_bin      = "../tests/mnist/layers/Convolution0.bin";
const char *c0_bias_bin = "../tests/mnist/layers/Convolution0.bias.bin";
const char *c1_bin      = "../tests/mnist/layers/Convolution1.bin";
const char *c1_bias_bin = "../tests/mnist/layers/Convolution1.bias.bin";
const char *d2_bin      = "../tests/mnist/layers/InnerProduct2.bin";
const char *d2_bias_bin = "../tests/mnist/layers/InnerProduct2.bias.bin";
const char *d3_bin      = "../tests/mnist/layers/InnerProduct3.bin";
const char *d3_bias_bin = "../tests/mnist/layers/InnerProduct3.bias.bin";
const char *output_bin   = "../tests/mnist/output.bin";

int main() {

    // Network layout
    tkDNN::Network net;
    tkDNN::dataDim_t dim(1, 1, 28, 28, 1);
    tkDNN::Layer *l;
    l = new tkDNN::Conv2d     (&net, dim, 20, 5, 5, 1, 1, c0_bin, c0_bias_bin);
    l = new tkDNN::Pooling    (&net, l->output_dim, 2, 2, 2, 2, tkDNN::POOLING_MAX);
    l = new tkDNN::Conv2d     (&net, l->output_dim, 50, 5, 5, 1, 1, c1_bin, c1_bias_bin);
    l = new tkDNN::Pooling    (&net, l->output_dim, 2, 2, 2, 2, tkDNN::POOLING_MAX);
    l = new tkDNN::Dense      (&net, l->output_dim, 500, d2_bin, d2_bias_bin);
    l = new tkDNN::Activation (&net, l->output_dim, CUDNN_ACTIVATION_RELU);
    l = new tkDNN::Dense      (&net, l->output_dim, 10, d3_bin, d3_bias_bin);
    l = new tkDNN::Softmax    (&net, l->output_dim);
 
    // Load input
    value_type *data;
    value_type *input_h;
    readBinaryFile(input_bin, dim.tot(), &input_h, &data);

    printDeviceVector(dim.tot(), data);
    dim.print(); //print initial dimension
    
    TIMER_START

    // Inference
    data = net.infer(dim, data);
    
    TIMER_STOP
    dim.print();   

    // Print result
    std::cout<<"\n======= RESULT =======\n";
    printDeviceVector(dim.tot(), data);
 
    // Print real test
    std::cout<<"\n==== CHECK RESULT ====\n";
    value_type *out;
    value_type *out_h;
    readBinaryFile(output_bin, dim.tot(), &out_h, &out);
    printDeviceVector(dim.tot(), out);
 
    return 0;
}