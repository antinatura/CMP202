# CMP202: Data Structures and Algorithms 2 

### Assessment Unit 1

> In this assessment, you will demonstrate your achievement of the following learning outcomes through a software project and video presentation:
> * Understand the emerging importance of parallel programming in modern software development, and experiment with the performance impact of parallelising parts of an application.
> * Describe a variety of application-specific algorithms (sorting/numerical/image processing) and associated data structures in common use, and discuss the benefits and limitations of parallelisation.
> * Be aware of the standard techniques of software performance measurement, including profiling, and apply these techniques to identify performance bottlenecks in real programs.

##### 1. Guidelines

> Your application must:
> * Implement a parallelised algorithm that solves a problem;
> * Make use of appropriate parallelisation strategies;
> * Quantify and reason about the (positive and/or negative) impact of your design choices.
> * **Be constructed so that you can vary the number of threads or thread groups being used and measure the application's performance in an appropriate way**

##### 2. My implementation

Multithreaded calculation of the mandelbrot set that outputs a progress bar on runtime. The number of threads to launch can be changed by the user.

1. **main.cpp** contains the main thread. User input is obtained and basic input validation is performed. The mandelbrot object is created, threads are launched, and the timing code is executed from here.
2. **mandelbrot.cpp** and **mandelbrot.h** contain the member variables and functions (including thread functions) of the mandelbrot class.
3. **mandelbrot.PNG** contains a sample of how the .tga file the program outputs should look.
