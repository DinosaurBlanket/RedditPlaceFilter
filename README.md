
Fetch snapshots with

```wget -r "http://spacescience.tech/place/img/" -A "149123*"```

You can play with that last number to control how many images get downloaded, or you could just download the whole directory, though keep in mind the program can't process more than 256 files at a time.

You'll need to convert the PNGs to BMPs for this to work, I recomend mogrify.

```mogrify -format bmp *.png```

Compile with ```gcc RedditPlaceFilter.c -o RedditPlaceFilter -lSDL2```.
