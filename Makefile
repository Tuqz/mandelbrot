all:
	gcc -o mandelbrot mandelbrot.c -lSDL -lm -std=c99 -Ofast

clean:
	rm -f mandelbrot
