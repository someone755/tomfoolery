# drawPnm.h
**Disclaimer:** The PNM file type is a standard from the 1980s, with few (if any) practical real world uses. While it serves as a good introduction to writing bitmap images in C, it is considered awkward for day-to-day life -- its large files compared to other lossy or even lossless encodings, combined with barely any support from popular image viewing software (Windows Photo Viewer, for example, does not recognize the standard), make it a top contender for the "worst image format" award. With that out of the way, here is a short summary of the format as well as a description of the `drawPnm.h` library.

### Table of contents
- [x] [Introduction](#introduction)
- [x] [PNM header](#pnm-header)
- [x] [One file type, two ways to write it](#one-file-type-two-ways-to-write-it)
    * [Grayscale](#grayscale)
    * [Color](#color)
- [x] [Conclusion and further reading](#conclusion-and-further-reading)
## Introduction
Right off the bat, we have to assume three things:
- The image is stored as a 2D array, with each element representing a single pixel;
- The pixel data is limited to (char)/8 bits/1 byte, i.e. 0-255;
- The array's dimensions are known before `drawPnm()` is called.

We begin by declaring a pointer to the desired file, specified by the `char fname[]` argument. We then immediately check whether the file is writable, and exit the program if it is not. This can happen if the program does not have sufficient permissions to write to the directory, or, and this is rarely the case, if the running machine's memory is too full for proper storage of our `f` pointer.

Going forwards, we see that `mode` can hold values 0 or 1. I've decided that 0 be a grayscale output, while 1 produces a color image. Here, the function diverges into two parts, but before we dive into that, we should take a look at the file type's header.
## PNM header
The PNM header consists of 4 numbers, let's call them x, y, z, and w (the first, second, third, and fourth number):
- y and z are the horizontal and vertical dimensions of the image, respectively. Here, the values of `x_res` and `y_res` can be used directly.
- w represents the number of steps from black to white. As we are dealing with 8-bit colors, we set this to 2^8-1, or 255.
- x  is what we call a *magic number*. It tells the program reading the file (such as an image viewer), "Hey! I'm ____ type of file!" so that the program can properly decode the file's contents. PNM uses P1 and P4 for black-and-white, P2 and P5 for grayscale, or P3 and P6 for color images. A brief explanation of the grayscale and color types follows below.
It is expected that each header element is followed by a whitespace character, be it a newline `\n` or a simple space ` `. You can see I mixed and matched the two so that, when reading the file in a text editor, it is easy to distinguish what each number means, as in the following example:
``` 
P5
# Here we see the header of a P5-type (grayscale) PNM picture, with a resolution of 200×300.
200 300
255
```
You also might have guessed that line 2 represents a comment, though they are mostly redundant in this file type's context.
## One file type, two ways to write it
### Grayscale
The PGM format (the grayscale version of PNM) exists in two variants:

Type of PGM | Pixel data storage
----------- | ------------------
P2          | As a number (0-255) + whitespace character, i.e. `42 `
P5          | As a character, i.e. `.` or `g`

While a P2 file might look something like this,
```
P2
2 2
255
1 2
46 189
```
we can quickly see that this will eat up more space than that same file's P5 counterpart. In P2, each pixel uses either 2, 3, or 4 characters (i.e. `1 `, `42 `, or `255 ` -- don't forget about the whitespace!), where each character requires 1 byte/8bits of disk space, whereas a P5 file stores the pixel data as one byte per pixel. For small files like our example 2×2 image, this might not mean much, but if we're generating large images (hundreds or thousands of pixels wide and tall), a P5 file can be 2× to 4× smaller. -- **We quickly realize that while the P2 filetype is simpler for learning, troubleshooting, and other analysis, P5 is a far more practical solution.**

We continue by writing the array `arr` to the file, directly after the header. We could achieve this with nested loops, like so,
```
for(int i=0;i<y_res;i++){
  for(int j=0;j<x_res;j++){
    fprintf(f,"%c",arr[i][j]);
    }
}
```
But, because we are only dealing with writing the array's contents to the file directly, using fwrite will be faster in this case,
```
fwrite(arr,1,x_res*y_res,f);
```
### Color
The PPM format (the color version of PNM) exists in two variants as well:

Type of PPM | Pixel data storage
----------- | ------------------
P3          | As three numbers (0-255), each followed by a whitespace character, i.e. `42 42 42 `
P6          | As three characters, i.e. `...` or `ggg`

Much like with PGM, it is evident that the P6 binary variant of the format is again, much smaller, but in this case, because the files are even larger than their grayscale equivalents, this is even more obvious. Let's take a look at the following example,
```
P3
2 2
255
1 1 1 2 2 2 
46 46 46 189 189 189
```
Each pixel in the P3 format requires between 6 to 12 bytes of data, but the P6 alternative uses 3 bytes for each pixel. A grayscale P5 image of the same resolution requires 1 byte per pixel.

Now that we understand the file format, we see that directly copying our input array `arr` will not work as it did with P5, due to our assumption that each array element only stores a single byte of data. Hence, using `fwrite()` will not work in a simple or time-saving way, and we are forced to use loops. To determine the RGB color values (3 bytes) for each `arr[i][j]` (1 byte) we could use different algorithms but I used a tried and tested method to get 256 different combinations out of one number,
```
for(int i=0;i<y_res;i++){
  for(int j=0;j<x_res;j++){
    fprintf(f,"%c%c%c",(arr[i][j]*3)%256,\
                       (arr[i][j]*2)%256,\
                        arr[i][j]); //RGB
    }
}
```
And there we have our PPM file.

As a side note, it's interesting to observe that because PPM actually works with 3 bytes per pixel, not 1 byte like we assumed initially and implemented in the nested `for` loop above, the format is actually able to provide a full 24-bit color palette, which translates to around 16.8 million colors.
## Conclusion and further reading
At the end of every function that uses `fopen()`, it is necessary to use its "reverse" counterpart, `fclose()`, to avoid any problems that may arise if we're working with multiple functions that write to the same file.

For a more detailed explanation of the file format, I recommend the [official documentation](http://netpbm.sourceforge.net/doc/index.html#formats), as well as the [Wikipedia entry on the Netpbm format](https://en.wikipedia.org/wiki/Netpbm_format).
