#include "C_mandelbrot.h"

// constructor
Mandelbrot::Mandelbrot(int input) {
	// initialise values
	num_threads = input;
	threads_left = num_threads;
	ready = false;
}

// destructor
Mandelbrot::~Mandelbrot() {};


// producer thread function that computes horizontal fragments of mandelbrot

/*
Adapted from :
	Auth: Adam Sampson
	Year: Unknown [Accessed 2022]
	Name: Mandelbrot set example [C++]
	Desc: Render the Mandelbrot set into the image array.
		  The parameters specify the region on the complex plane to plot.
	Available: Provided by Abertay University 
*/

void Mandelbrot::compute_mandelbrot(double left, double right, double top, double bottom, int begin, int end)
{
	for (int y = begin; y < end; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			// work out the point in the complex plane that corresponds to this pixel in the output image
			complex<double> c(left + (x * (right - left) / WIDTH),
				top + (y * (bottom - top) / HEIGHT));

			// start off z at (0, 0)
			complex<double> z(0.0, 0.0);

			// iterate z = z^2 + c until z moves more than 2 units away from (0, 0), or we've iterated too many times
			int iterations = 0;
			while (abs(z) < 2.0 && iterations < MAX_ITERATIONS)
			{
				z = (z * z) + c;

				++iterations;
			}

			// colour the pixel
			if (iterations == MAX_ITERATIONS)
			{
				// z didn't escape from the circle. this point is in the Mandelbrot set
				image[y][x] = 0x000000; // black
			}
			else
			{
				// z escaped within less than MAX_ITERATIONS iterations. this point isn't in the set
				int i = iterations;
				if (i < 3)
					image[y][x] = 0x6d5269;
				else if (i < 5)
					image[y][x] = 0x916d8c;
				else if (i < 10)
					image[y][x] = 0xB689B0;
				else if (i < 15)
					image[y][x] = 0xD5A4CF;
				else if (i < 25)
					image[y][x] = 0xF9BCDD;
				else if (i < 50)
					image[y][x] = 0xFCD9C2;
				else if (i < 500)
					image[y][x] = 0xFFF1AC;
			}

			// image[end - 1][x] = 0xFF0000; // draws a border after each thread's calculated fragment for visualisation
		}
	}

	// calculation and colouring is done, update progress
	update_progress();
}

// extension of the producer function that updates progress
void Mandelbrot::update_progress() {
	std::unique_lock<std::mutex> lock(mtx); // lock the ready flag
	--threads_left; // decrease the amount of unfinished threads
	ready = true; // thread has finished producing so set ready to true
	lock.unlock();
	cv.notify_one(); // notify the waiting consumer thread
}

// consumer thread function that outputs the progress of threads that are drawing the mandelbrot set
void Mandelbrot::show_progress() {

	// algorithm:
	// while all threads haven't finished (threads_left > 0)
	// conditional variable waits for the flag
	// gets the progress
	// outputs the progress

	while (threads_left > 0) {
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [&] {return ready; });
		double progress = ((static_cast<double>(num_threads) - threads_left) / num_threads);

		// output a progress bar

		/*
		Adapted from :
			Auth: leemes
			Year : 2013
			Name : Progress indicator [C++]
			Desc : Fixed width progress bar output
			Available : https://stackoverflow.com/a/14539953
		*/

		if (progress < 1.0) {
			int bar_width = 50; 
			std::cout << "[";
			int pos = bar_width * progress;
			for (int i = 0; i < bar_width; ++i) {
				if (i < pos) std::cout << "=";
				else if (i == pos) std::cout << ">";
				else std::cout << " ";
			}
			std::cout << "] " << int(progress * 100.0) << " %\n";
			std::cout.flush();
		}

		ready = false;
	}
}

// write mandelbrot to a .tga file

/*
Adapted from :
	Auth: Adam Sampson
	Year: Unknown [Accessed 2022]
	Name: Mandelbrot set example [C++]
	Desc: Write the image to a TGA file with a given name.
     	  Format specification: http://www.gamers.org/dEngine/quake3/TGA.txt
	Available: Provided by Abertay University
*/

void Mandelbrot::draw_mandelbrot(const char* filename)
//const int& height, const int& width, uint32_t** image
{
	std::ofstream outfile(filename, std::ofstream::binary);

	uint8_t header[18] = {
		0, // no image ID
		0, // no colour map
		2, // uncompressed 24-bit image
		0, 0, 0, 0, 0, // empty colour map specification
		0, 0, // X origin
		0, 0, // Y origin
		WIDTH & 0xFF, (WIDTH >> 8) & 0xFF, // width
		HEIGHT & 0xFF, (HEIGHT >> 8) & 0xFF, // height
		24, // bits per pixel
		0, // image descriptor
	};
	outfile.write((const char*)header, 18);

	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			uint8_t pixel[3] = {
				image[y][x] & 0xFF, // blue channel
				(image[y][x] >> 8) & 0xFF, // green channel
				(image[y][x] >> 16) & 0xFF, // red channel
			};
			outfile.write((const char*)pixel, 3);
		}
	}

	outfile.close();
	if (!outfile)
	{
		// an error has occurred at some point since opening the file
		std::cout << "Error writing to " << filename << std::endl;
		exit(1);
	}
}

// getter functions
int Mandelbrot::get_num_threads() {
	return num_threads;
}
