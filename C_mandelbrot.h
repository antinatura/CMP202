#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <complex>
#include <mutex>
#include <atomic>
#include <condition_variable>

using std::complex;

// the size of the image to generate
const int HEIGHT = 1200;
const int WIDTH = 1920;

class Mandelbrot
{
private:
	const int MAX_ITERATIONS = 500; // number of times to iterate before assuming that a point isn't in the mandelbrot set

	uint32_t image[HEIGHT][WIDTH]; 	// image data. each pixel is represented as 0xRRGGBB.

	int num_threads; // number of threads to run
	std::atomic<int> threads_left; // counter that keeps track of how many threads haven't finished
	std::mutex mtx; // mutex to protect the ready flag
	bool ready; // sets that the progress has been updated and can be output
	std::condition_variable cv; // conditional variable that notifies the waiting consumer thread

public:
	Mandelbrot(int);
	~Mandelbrot();
	void compute_mandelbrot(double, double, double, double, int, int);
	void update_progress();
	void show_progress();
	void draw_mandelbrot(const char*);

	// getters
	int get_num_threads();
};

