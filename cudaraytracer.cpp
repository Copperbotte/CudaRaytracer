// cudaraytracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//CUDA by example
//https://developer.download.nvidia.com/books/cuda-by-example/cuda-by-example-sample.pdf

#include <iostream>
#include <Windows.h>

#include <fstream>
#include <string>
#include <sstream>
#include <random>

#include "glad/glad.h"
#include "glfw/glfw3.h"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glfw3.lib")
#pragma comment (lib, "glfw3dll.lib")

#include <cuda_runtime.h>
#include <helper_cuda.h>

#pragma comment (lib, "cudart_static.lib")

using namespace std;

const int width = 800,
          height = 600;
unsigned char colorbuffer[width * height * 4];

int clamp(int x, int low, int high)
{
    if (x < low) x = low;
    if (high < x) x = high;
    return x;
}

__constant__ float m[2];

struct cuComplex
{
    float r, i;
    __device__ cuComplex(float a, float b) :r(a), i(b) {}
    __device__ float mag2(void) { return r * r + i * i; }
    __device__ cuComplex operator* (const cuComplex& a) {
        return cuComplex(r * a.r - i * a.i, i * a.r + r * a.i);
    }
    __device__ cuComplex operator+(const cuComplex& a) {
        return cuComplex(r + a.r, i + a.i);
    }
};

__device__ int julia(int x, int y)
{
    const float scale = 1.5;
    float jx = scale * (float)(x - width / 2) / (float)(width / 2);
    float jy = scale * (float)(y - height / 2) / (float)(width / 2);

    //cuComplex c(-0.8, 0.156);
    cuComplex c(m[0], m[1]);
    cuComplex a(jx, jy);

    for (int i = 0; i < 200; i++)
    {
        a = a * a + c;
        if (a.mag2() > 2000)
            return 0;
    }
    return 1;
}

__global__ void kernel(unsigned char* ptr)
{
    int x = blockIdx.x;
    int y = blockIdx.y;
    int offset = x + y * gridDim.x;
    
    int juliaValue = 255 * julia(x, y);

    ptr[offset * 4 + 0] = juliaValue;
    ptr[offset * 4 + 1] = juliaValue;
    ptr[offset * 4 + 2] = juliaValue;
    ptr[offset * 4 + 3] = 255;
}

int main()
{
    const char* prog_title = "Cuda Raytracer";
    cout << prog_title << '\n';

    //initialize library
    if (!glfwInit())
    {
        cout << "GLFW Failed to init" << endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, prog_title, NULL, NULL);
    if (!window)
    {
        cout << "GLFW Window failed to create" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        glfwTerminate();
        return -1;
    }

    cout << "Opengl " << glGetString(GL_VERSION) << " GLSL" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    glViewport(0, 0, width, height);

    for (int i = 0; i < width * height * 4; ++i)
        colorbuffer[i] = 0xFF;

    
    for (int n = 0; n < width * height; ++n)
    {
        char* c = (char*)(colorbuffer) + 4 * n;
        c[0] = 0xFF;
        c[1] = 0x8F;
        c[2] = 0x00;
    }

    unsigned char* dev_bmp;
    cudaError_t err = cudaSuccess;
    err = cudaMalloc((void**)&dev_bmp, width * height * 4 * sizeof(char));
    if (err != cudaSuccess)
    {
        cout << "Failed to allocate bitmap on gpu\nerror code: " << cudaGetErrorString(err);
        glfwTerminate();
        return -1;
    }

    dim3 grid(width, height);

    while (!glfwWindowShouldClose(window))
    {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        my = height - my;

        mx = clamp(mx, 0, width - 1);
        my = clamp(my, 0, height - 1);

        float fm[2] = { (float)mx / width, (float)my / height };

        err = cudaMemcpyToSymbol(m, fm, 2*sizeof(float), 0, cudaMemcpyHostToDevice);
        if (err != cudaSuccess)
        {
            cout << "Failed to copy memory to gpu\nerror code: " << cudaGetErrorString(err);
            glfwTerminate();
            return -1;
        }

        kernel <<<grid, 1>>> (dev_bmp);
        err = cudaMemcpy(colorbuffer, dev_bmp, width * height * 4 * sizeof(char), cudaMemcpyDeviceToHost);
        if (err != cudaSuccess)
        {
            cout << "Failed to copy memory from gpu\nerror code: " << cudaGetErrorString(err);
            glfwTerminate();
            return -1;
        }

        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, colorbuffer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    err = cudaFree(dev_bmp);
    if (err != cudaSuccess)
    {
        cout << "Failed to free cuda memory\nerror code: " << cudaGetErrorString(err);
        return -1;
    }

    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
