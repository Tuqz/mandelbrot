#include <math.h>
#include <SDL/SDL.h>
#include <stdbool.h>
#include <stdint.h>

struct complex {
	double real;
	double imag;
};
typedef struct complex complex_t;

#define complex_add(a, b, result)\
	do{\
		result.real = a.real + b.real;\
		result.imag = a.imag + b.imag;\
	} while(0)
#define complex_square(a) {(a.real*a.real)-(a.imag*a.imag), 2*a.real*a.imag}
#define MAX_LOOP 200
#define PIXARRAY_WIDTH 800 
#define PIXARRAY_LENGTH 600 
#define PI 3.141592635358
#define SCALE(i, scale, centre, size) ((i*scale)/(size/4.0))-(2.0*scale)+centre

double complex_magnitude(complex_t *z) {
	double mag_sq = (z->real*z->real)+(z->imag*z->imag);
	return sqrt(mag_sq);
}	

double in_set(complex_t *c) {
	complex_t z = {0, 0};
	short i;
	for(i = 0; i < MAX_LOOP; ++i) {
		complex_t z_sq = complex_square(z);
		complex_add(z_sq, (*c), z);
		if(complex_magnitude(&z) >= MAX_LOOP)
			break;
	}
	return ((double)i)/((double)MAX_LOOP);
}

void mandel_gen(uint32_t *pixels, double scale, double centre_x, double centre_y) {
	#pragma omp parallel
	{
	for(int i = 0; i < PIXARRAY_LENGTH; ++i) {
		for(int j = 0; j < PIXARRAY_WIDTH; ++j) {
			complex_t scaled = {SCALE(j, scale, centre_x, PIXARRAY_WIDTH), SCALE(i, scale, centre_y, PIXARRAY_LENGTH)};
			double runs = in_set(&scaled);
			double sine_runs = sin(runs*PI);
			short value = (short)(sine_runs * 255.0);
			uint32_t pixel = 0 | value << 16 | value << 8 | value;
			pixels[(i * PIXARRAY_WIDTH) + j] = pixel;
		}
	}
	}
}

int main() {
	bool running = true;
	double zoom = 2, centre_x = 0, centre_y = 0;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface *screen = SDL_SetVideoMode(PIXARRAY_WIDTH, PIXARRAY_LENGTH, 32, SDL_HWSURFACE);
	SDL_WM_SetCaption("Mandelbrot renderer", NULL);

	SDL_Event ev;
	mandel_gen(screen->pixels, zoom, centre_x, centre_y);
	SDL_Flip(screen);
	while(running) {
		while(SDL_PollEvent(&ev)) {
			if(ev.type == SDL_QUIT)
				running = false;
			if(ev.type == SDL_MOUSEBUTTONDOWN) {
				centre_x = SCALE(ev.button.x, zoom, centre_x, PIXARRAY_WIDTH);
				centre_y = SCALE(ev.button.y, zoom, centre_y, PIXARRAY_LENGTH);
				if(ev.button.button == SDL_BUTTON_LEFT) {
					zoom /= 2;
				}
				else {
					zoom *= 2;
				}
				SDL_LockSurface(screen);
				printf("Started render...");
				mandel_gen(screen->pixels, zoom, centre_x, centre_y);
				printf("[OK]\n");
				SDL_UnlockSurface(screen);
				SDL_Flip(screen);
			}
		}
	}
	SDL_Quit();
	return 0;
}
