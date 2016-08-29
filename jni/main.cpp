//
//  main.cpp
//  label_image
//
//  Created by VHackintosh on 8/27/16.
//  Copyright © 2016 V. All rights reserved.
//

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/framework/tensor.h>

using namespace std;
using namespace cv;

int main(int argc, const char * argv[])
{
    if( argc < 2 )
    {
        return -1;
    }
    
    Mat inputImg = imread( argv[1] );
    
    if( inputImg.empty() )
    {
        printf("Empty image\n");
        return -1;
    }
    
    float meanImg = 128.0f;
    float stdImg = 128.0f;
    
    Mat inputSm;
    resize( inputImg, inputSm, Size(299, 299), 0, 0, INTER_LINEAR );
    Size dstSize = inputSm.size();
    
    inputSm.convertTo(inputSm, CV_32F);
    inputSm -= meanImg;
    inputSm /= stdImg;
    
    // creating a Tensor for storing the data
    tensorflow::Tensor input_tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1,dstSize.height,dstSize.width,inputSm.channels()}));
    
    auto input_tensor_mapped = input_tensor.tensor<float, 4>();
    
    // copying the data into the corresponding tensor
    for (int y = 0; y < dstSize.height; ++y)
    {
        const float* srcPtr = inputSm.ptr<float>(y);
        
        for (int x = 0; x < dstSize.width; ++x)
        {
            input_tensor_mapped(0, y, x, 0) = srcPtr[3*x + 0];
            input_tensor_mapped(0, y, x, 1) = srcPtr[3*x + 1];
            input_tensor_mapped(0, y, x, 2) = srcPtr[3*x + 2];
        }
    }
    
    
    // initializing the graph
    tensorflow::GraphDef graph_def;
    
    // Name of the folder in which inception graph is present
    string graphFile = "tensorflow_inception_graph_stripped.pb";
    
    // Loading the graph to the given variable
    tensorflow::Status graphLoadedStatus = ReadBinaryProto(tensorflow::Env::Default(),graphFile,&graph_def);
    if (!graphLoadedStatus.ok())
    {
        printf("%s", graphLoadedStatus.ToString().c_str() );
        return 1;
    }
    
    // creating a session with the graph
    unique_ptr<tensorflow::Session> session_inception(tensorflow::NewSession(tensorflow::SessionOptions()));
    //session->reset(tensorflow::NewSession(tensorflow::SessionOptions()));
    tensorflow::Status session_create_status = session_inception->Create(graph_def);
    
    if (!session_create_status.ok()){
        return 1;
    }
    // running the loaded graph
    vector<tensorflow::Tensor> finalOutput;
    
    string InputName = "Mul";
    string OutputName = "softmax";
    
    tensorflow::Status run_status  = session_inception->Run({{InputName,input_tensor}},{OutputName},{},&finalOutput);
    
    if (!run_status.ok())
    {
        printf("%s\n", run_status.ToString().c_str() );
        return 1;
    }
    
    // finding the labels for prediction
    printf( "final output size= %d\n", (int)finalOutput.size() );
    
    tensorflow::Tensor output = move(finalOutput.at(0));
    auto scores = output.flat<float>();
    printf( "scores size = %d\n", (int)scores.size() );
    
    // Label File Name
    string labelfile = "imagenet_comp_graph_label_strings.txt";
    ifstream label(labelfile);
    string line;
    
    // sorting the file to find the top labels
    vector<pair<float,string>> sorted;
    
    for (unsigned int i =0; i<=1000 ;++i)
    {
        getline(label,line);
        sorted.emplace_back(scores(i),line);
        //cout << scores(i) << " / line=" << line << endl;
    }
    
    sort(sorted.begin(),sorted.end());
    reverse(sorted.begin(),sorted.end());
    printf("size of the sorted file is %d\n", (int)sorted.size() );
    
    for(unsigned int i =0 ; i< 5;++i)
    {
        
        printf("The output of the current graph has category %s with probability %f\n", sorted[i].second.c_str(), sorted[i].first );
    }
    
    return 0;
}
