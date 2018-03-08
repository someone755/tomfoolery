# drawBmp.h
**Disclaimer:** The BMP file type is an old but still commonly used standard. While this README serves as a good introduction to the file format, its main goal is to document the accompanying `drawBmp.h` library as part of my [cdraw project](https://github.com/someone755/tomfoolery/tree/cdraw/) on GitHub. There exist many more efficient ways of file storage and creation, but I've found the one I've chosen -- without compression or large color depths -- to be the easiest to both code as well as document and properly explain.
### Table of contents
- [x] [Introduction](#introduction)
- [x] [File header](#header)
	* [Bitmap file header](#bitmap-file-header)
	* [Bitmap information header (DIB header)](#bitmap-information-header-dib-header)
	* [Color table](#color-table)
- [x] [Writing the data](#writing-the-data)
- [x] [Conclusion and further reading](#conclusion-and-further-reading)


## Introduction
Before we begin using or writing our program, we're going to assume three things:
- The image is stored as a 2D array, where each array element represents one pixel of the image;
- The pixel data is limited to 8 bits/1 byte, meaning a single pixel can have 256 different values;
- The 2D array's dimensions are known before `drawBmp()` is called.

We'll begin our program by opening a file to write in, defined by the `char fname[]` function argument. We then immediately check whether the file is writable, and exit the program if it is not. This can happen if the program does not have sufficient permissions to write to the directory, or, and this is rarely the case, if the running machine's memory is too full for proper storage of our `f` pointer.

## Header
Type of data | Size of data
------------ | ------------
Bitmap file header | 14 bytes
Bitmap information header (DIB header) | 40 bytes
Color table | 256×4 bytes for an 8-bit (color or grayscale) image

An important fact to consider is that the integer values of the header data are stored in little endian format -- the least significant byte is written first. For example, while the decimal number `2` is written as `10`, it would be written as `01` in little endian.
### Bitmap file header
This block of bytes is located at the beginning of the file and is mainly used to identify the file. Its 14 bytes are used as follows:

Offset | Size | Data
------ | ---- | ----
0 | 2 bytes | Magic number ('BM')
2 bytes | 4 bytes | Size of BMP file in bytes
6 bytes | 2 bytes | Reserved (left blank in our case)
8 bytes | 2 bytes | Reserved (left blank in our case)
10 bytes | 4 bytes | Starting address of the pixel data array

We write the magic number literally, as 'B' and 'M';
```
arry[0]='B';
arry[1]='M';
```
Note how the array index denotes the data byte's offset from the beginning of the file. Also remember that our array `arry` is of type char.

The size of the BMP file is equal to the size of the entire header (54 bytes), color table (256×4), and the size of the pixel data array we get from the user (where we account for the 4 byte padding, more on this [below](#writing-the-data)!), combined. Accounting for the little-endian format, this forms the following code snippet:
```
arry[2]=arrySize%256;
arry[3]=(arrySize/256)%256;
arry[4]=((arrySize/256)/256)%256;
arry[5]=(((arrySize/256)/256)/256)%256;
```
where the `const signed int arrySize` denotes the final size of the BMP file.

Bytes six to nine can hold some image-editor specific data but in our case setting the values to `0` will suffice.
```
for(char i=6;i<10;i++) arry[i]=0;
```
The final bitmap file header field is the pixel array offset, which is equal to the size of the bitmap file header (14 bytes), bitmap information header (DIB, 40 bytes), and color table (256×4) combined, for a total of 1078 bytes. Converted to base 256, the output is
```
arry[10]=54;
arry[11]=4;
arry[12]=0;
arry[13]=0;
```
### Bitmap information header (DIB header)
The DIB header this library uses is called `BITMAPINFOHEADER`. Its 11 fields are arranged as follows:

Offset | Size | Data
------ | ---- | ----
14 bytes | 4 bytes | Size of this DIB header (40 bytes)
18 bytes | 4 bytes | Image width in pixels
22 bytes | 4 bytes | Image height in pixels
26 bytes | 2 bytes | Number of color planes (always 1)
28 bytes | 2 bytes | Bits per pixel (8 in this library)
30 bytes | 4 bytes | Compression method (0 denotes no compression is being used)
34 bytes | 4 bytes | Raw bitmap data size (can be 0 if no compression is applied)
38 bytes | 4 bytes | Horizontal image resolution (pixels per meter, 0 in this library)
42 bytes | 4 bytes | Vertical image resolution (pixels per meter, 0 in this library)
46 bytes | 4 bytes | Number of colors in the color palette (256×3 for 8-bit color depth)
50 bytes | 4 bytes | Number of important colors, 0 if all colors are equally important

Using the above table, as well as the comments within the library, the code in this section should be pretty self-explanatory. Another reminder is in place here that numbers are converted to base 256, and that the little endian format is used by the BMP file format.
### Color table
Because we're making an 8-bit color depth image with BMP, we need to provide our own color table within each file (images with higher color depths do not require this). The BMP file format specification requires the color palette to be 4 bytes per entry (i.e. 4 bytes per color). But because there are only three base colors (red, green and blue), the fourth byte is empty data. Given this, understanding the code section is again child's play.

## Writing the data
We'll begin our file with the header. This can be achieved using a loop statement, or by using `fwrite`. In the library, I opted for the former, but either can be used:
```
for(int i=0;i<54+256*4;i++) fprintf(f,"%c",arry[i]);
```
We continue by writing the pixel data array. Here, two things must be considered: First, each horizontal pixel row must be padded to have a size which is a multiple of the number 4. The following code snippet is scattered throughout the library to allow for better performance, but for easier analysis we will write it as if it were one chunk of code:
```
if(x_res%4==0) endPad=0;
else endPad=4-(x_res%4);
const char padding[]={0,0,0};
```
The above code snippet determines whether row padding is necessary. If it is, the number of padding bytes is calculated, and an array with three zeroes is declared. The two variables are going to be used as the array and array length parameters when we call the `fwrite` function each time after drawing one horizontal line of our pixel data array:
```
  fwrite(&pic[i*y_res],1,x_res,f);
  fwrite(padding,1,endPad,f);
```
Secondly, it must be taken into account that, if we were to simply write our pixel data array as we are used (i.e. left-to-right, and top-to-bottom), the image our program would output would be flipped horizontally. To avoid this, we must adapt the program as follows; Note the for loop's init, condition, and increment statements:
```
for(int i=y_res-1;i>=0;i--){
  fwrite(&pic[i*y_res],1,x_res,f);
  fwrite(padding,1,endPad,f);
}
```
Doing this, we write each line of the image separately, and account for the necessary padding.

At the end of every function that uses `fopen()`, it is necessary to use its "reverse" counterpart, `fclose()`, to avoid any problems that may arise if we're working with multiple functions that write to the same file.
## Conclusion and further reading
For a more detailed explanation of the file format, I strongly recommend the [Wikipedia entry on the BMP format](https://en.wikipedia.org/wiki/BMP_file_format), as well as searching through [StackOverflow](http://stackoverflow.com/). These resources also cover the use of different DIB headers, compression, larger color depths etc.
