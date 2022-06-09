// Name: cw_mandelbrot.sln
// Desc: Paralelised Mandelbrot with a progress bar
// Auth: Ance Strazdina
// Date: 14/05/2022

#include <vector>
#include <thread>
#include <string>
#include <chrono>

#include "C_mandelbrot.h"

using std::thread;
using std::chrono::steady_clock;

int main(int argc, char* argv[]) {
	std::cout << "PARALLELISED MANDELBROT\n";

	std::string input; // user input
	int thread_count; // number of threads to run
	std::vector<thread*> threads; // vector of thread pointers

	// get user input for number threads to use for calculating the mandelbrot
	std::cout << "\nEnter thread count (leave empty to use the number of CPU cores): ";
	std::getline(std::cin, input);

	// input validation
	if (input.empty()) {
		thread_count = thread::hardware_concurrency(); // if enter is pressed, defaults to the number of cpu cores. 
													   // NOTE: if HEIGHT does not evenly divide by the cpu cores, mandelbrot.tga will have a very small chunk missing at the top
		if (thread_count == 0) // just in case hardware_concurrency() returns 0
			thread_count = 8;
	}
	else {
		thread_count = stoi(input);
		while (thread_count == 0 || thread_count > HEIGHT || HEIGHT % thread_count != 0) { // if inputted thread count exceeds image height, or makes mandelbrot fragments not distribute evenly
			std::cout << "Can't distribute Mandelbrot fragments evenly. Try again: ";
			std::getline(std::cin, input);
			if (input.empty())
				thread_count = thread::hardware_concurrency();
			else 
				thread_count = stoi(input);
		}
	}

	std::cout << "\nCalculating Mandelbrot with " << thread_count << " threads.\n";
	std::cout << "Please wait...\n\n";

	Mandelbrot* mandelbrot = new Mandelbrot(thread_count); // create mandelbrot object
	int fragment_size = HEIGHT / mandelbrot->get_num_threads(); // find the size of the fragment each thread will calculate
	

	steady_clock::time_point start = steady_clock::now(); // start timing (for calculating the entire set)


	// create threads 
	thread progress(&Mandelbrot::show_progress, mandelbrot); // consumer thread for showing the progress
	for (int i = 0; i < mandelbrot->get_num_threads(); ++i) {
		threads.push_back(new thread(&Mandelbrot::compute_mandelbrot, mandelbrot, -2.0, 1.0, 1.125, -1.125, (0 + fragment_size * i), (fragment_size + fragment_size * i))); // these coordinates show the full set

		// threads.push_back(new thread(&Mandelbrot::compute_mandelbrot, mandelbrot, -0.751085, -0.734975, 0.118378, 0.134488, begin, end)); // zoomed in mandelbrot section
	}

	// join threads
	for (thread* t : threads)
	{		
		t->join();
		delete t;
	}
	progress.join();


	steady_clock::time_point stop = steady_clock::now(); // stop timing 
	
	std::cout << "\nDone!\n";
	// compute the runtime and output result
	auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
	std::cout << "\nComputing the entire Mandelbrot set took " << time_taken << " ms.\n";

	mandelbrot->draw_mandelbrot("mandelbrot.tga"); // draw the calculated set
	delete mandelbrot; // delete mandelbrot object

	system("pause");
	return 0;
}
