#include <string.h>
#include <dirent.h>
#include <SDL2/SDL_surface.h>

#define LOG_SDL_ERRORS_TO stderr
#ifdef LOG_SDL_ERRORS_TO
	#include <SDL2/SDL_error.h>
	void sdlec(int line, const char *file) {
	  const char *error = SDL_GetError();
	  if (!error || !error[0]) return;
		fprintf(LOG_SDL_ERRORS_TO,
			"SDL error at line %i in %s :\n%s\n",
			line, file, error
		);
	  SDL_ClearError();
	  exit(-10);
	}
  #define _sdlec sdlec(__LINE__, __FILE__);
#else
	#define _sdlec
#endif
#define _SHOULD_NOT_BE_HERE_ printf("!! SHOULD NOT BE HERE: line %i of %s\n", __LINE__, __FILE__)
#define fr(i, bound) for (int i = 0; i < (bound); i++)

#define colorCount 16
const SDL_Color placeColors[colorCount] = {
	{  0,   0, 234, 255},
	{  0, 211, 221, 255},
	{  0, 131, 199, 255},
	{  2, 190,   1, 255},
	{ 34,  34,  34, 255},
	{130,   0, 128, 255},
	{136, 136, 136, 255},
	{148, 224,  68, 255},
	{160, 106,  66, 255},
	{207, 110, 228, 255},
	{228, 228, 228, 255},
	{229,   0,   0, 255},
	{229, 149,   0, 255},
	{229, 217,   0, 255},
	{255, 167, 209, 255},
	{255, 255, 255, 255}
};
#define placeBBP 8
#define placeW 1000
#define placeH 1000
const int pixelCount = placeW * placeH;
uint8_t colorCounts[placeH*placeW*16];

SDL_Surface *srfc = NULL;

void processImage(const char *path) {
	printf("reading %s\n", path);
	srfc = SDL_LoadBMP(path);_sdlec
	if (srfc->w != placeW || srfc->h != placeH) {
		_SHOULD_NOT_BE_HERE_;
		exit(__LINE__);
	}
	if (srfc->format->palette->ncolors != colorCount) {
		_SHOULD_NOT_BE_HERE_;
		exit(__LINE__);
	}
	if (srfc->format->BitsPerPixel != placeBBP) {
		_SHOULD_NOT_BE_HERE_;
		exit(__LINE__);
	}
	// count the colors
	const uint8_t   *srfcPixels = (uint8_t*)srfc->pixels;
	const SDL_Color *srfcColors = srfc->format->palette->colors;
	int colorCountsPos = 0;
	fr (pix, pixelCount) {
		colorCountsPos = pix * colorCount;
		fr (pal, colorCount) {
			if (!memcmp(&srfcColors[srfcPixels[pix]], &placeColors[pal], sizeof(SDL_Color))) {
				colorCounts[colorCountsPos+pal]++;
				break;
			}
			if (pal == colorCount-1) _SHOULD_NOT_BE_HERE_;
		}
	}
}

void generateOutput(const char *path) {
	printf("writing %s\n", path);
	SDL_LockSurface(srfc);_sdlec
	uint8_t   *srfcPixels = (uint8_t*)srfc->pixels;
	SDL_Color *srfcColors = srfc->format->palette->colors;
	fr (i, colorCount) srfcColors[i] = placeColors[i];
	int colorCountsPos = 0;
	fr (pix, pixelCount) {
		colorCountsPos = pix * colorCount;
		int highColorCount = 0;
		int highColorIndex = colorCount;
		int pal = 0;
		for (; pal < colorCount; pal++) {
			if (colorCounts[colorCountsPos+pal] > highColorCount) {
				highColorCount = colorCounts[colorCountsPos+pal];
				highColorIndex = pal;
			}
		}
		if (highColorIndex == colorCount) _SHOULD_NOT_BE_HERE_;
		srfcPixels[pix] = highColorIndex;
	}
	SDL_UnlockSurface(srfc);_sdlec
	SDL_SaveBMP(srfc, path);_sdlec
}

int main(int argc, char **argv) {
	if (argc != 3) {
		puts("Usage: program_name <dir_in> <file_out>");
		return __LINE__;
	}
	DIR *dir;
	struct dirent *ent;
	// path is relative to where you run the program
	if ((dir = opendir(argv[1])) != NULL) {
		// the color counters can't go over 255
	  for (int i = 0; (ent = readdir(dir)) != NULL && i < 256; i++) {
			if (ent->d_name[0] == '.') continue;
			char path[32] = {0};
			strcpy(path, argv[1]);
			strcat(path, ent->d_name);
			processImage(path);
	  }
	  closedir(dir);
	} else {
	  printf("Could not open directory %s\n", argv[1]);
	  return __LINE__;
	}
	generateOutput(argv[2]);
	SDL_FreeSurface(srfc);_sdlec
	return 0;
}
