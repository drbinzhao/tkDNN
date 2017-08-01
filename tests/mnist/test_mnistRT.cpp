#include<iostream>
#include "tkdnn.h"
#include "NvInfer.h"

const char *input_bin   = "../tests/mnist/input.bin";
const char *c0_bin      = "../tests/mnist/layers/c0.bin";
const char *c1_bin      = "../tests/mnist/layers/c1.bin";
const char *d2_bin      = "../tests/mnist/layers/d2.bin";
const char *d3_bin      = "../tests/mnist/layers/d3.bin";
const char *output_bin   = "../tests/mnist/output.bin";

// Logger for info/warning/errors
class Logger : public nvinfer1::ILogger			
{
	void log(Severity severity, const char* msg) override
	{
		// suppress info-level messages
		if (severity != Severity::kINFO)
			std::cout << msg << std::endl;
	}
} gLogger;

int main() {

    std::cout<<"\n==== CUDNN ====\n";
    // Network layout
    tkDNN::Network net;
    tkDNN::dataDim_t dim(1, 1, 28, 28, 1);
    tkDNN::Layer *l;
    l = new tkDNN::Conv2d     (&net, dim, 20, 5, 5, 1, 1, 0, 0, c0_bin);
    l = new tkDNN::Pooling    (&net, l->output_dim, 2, 2, 2, 2, tkDNN::POOLING_MAX);
    l = new tkDNN::Conv2d     (&net, l->output_dim, 50, 5, 5, 1, 1, 0, 0, c1_bin);
    l = new tkDNN::Pooling    (&net, l->output_dim, 2, 2, 2, 2, tkDNN::POOLING_MAX);
    l = new tkDNN::Dense      (&net, l->output_dim, 500, d2_bin);
    l = new tkDNN::Activation (&net, l->output_dim, CUDNN_ACTIVATION_RELU);
    l = new tkDNN::Dense      (&net, l->output_dim, 10, d3_bin);
    l = new tkDNN::Softmax    (&net, l->output_dim);
 
    // Load input
    value_type *data;
    value_type *input_h;
    readBinaryFile(input_bin, dim.tot(), &input_h, &data);

    dim.print(); //print initial dimension
    
    TIMER_START
    // Inference
    data = net.infer(dim, data);
    TIMER_STOP
    dim.print();   

    // Print real test
    std::cout<<"\n==== CHECK CUDNN RESULT ====\n";
    value_type *out;
    value_type *out_h;
    readBinaryFile(output_bin, dim.tot(), &out_h, &out);
    std::cout<<"Diff: "<<checkResult(dim.tot(), out, data)<<"\n";
 

    std::cout<<"\n==== TensorRT ====\n";
	// create the builder
	nvinfer1::IBuilder* builder = nvinfer1::createInferBuilder(gLogger);
	nvinfer1::INetworkDefinition* network = builder->createNetwork();

    return 0;
}