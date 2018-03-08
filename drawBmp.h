void drawBmp(unsigned char pic[],int x_res,int y_res,char fname[]){
    FILE *f = fopen(fname,"wb");
    if(f==NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    int endPad;
    if(x_res%4==0) endPad=0;
    else endPad=4-(x_res%4);
    static unsigned char arry[54+256*4]; // header size (54 bytes) + color table (256 colors*4)
    const unsigned int arrySize=54+256*4+x_res*y_res+y_res*endPad; //length of file in bytes: sizeof(arry) + one byte per pixel +  pad rows to 4-byte multiples
    /* BMP HEADER */
    //header field (BM), 2 bytes
    arry[0]='B';
    arry[1]='M';
    //size of file in bytes, 4 bytes
    arry[2]=arrySize%256;
    arry[3]=(arrySize/256)%256;
    arry[4]=((arrySize/256)/256)%256;
    arry[5]=(((arrySize/256)/256)/256)%256;
    //2×reserved, 2 bytes each
    for(char i=6;i<10;i++) arry[i]=0;
    //pixel offset, 4 bytes
    arry[10]=54; //assuming pixel offset 1078 bytes = sizeof(arry)
    arry[11]=4;
    arry[12]=0;
    arry[13]=0;
    /* DIB HEADER (using BITMAPINFOHEADER) */
    //size of BITMAPINFOHEADER header (40 bytes), 4 bytes
    arry[14]=40;
    for(char i=15;i<18;i++) arry[i]=0;
    //bitmap width in pixels, 4 bytes
    arry[18]=x_res%256;
    arry[19]=(x_res/256)%256;
    arry[20]=((x_res/256)/256)%256;
    arry[21]=(((x_res/256)/256)/256)%256;
    //bitmap height in pixels, 4 bytes
    arry[22]=y_res%256;
    arry[23]=(y_res/256)%256;
    arry[24]=((y_res/256)/256)%256;
    arry[25]=(((y_res/256)/256)/256)%256;
    //number of color planes, 2 bytes
    arry[26]=1;
    arry[27]=0;
    //bits per pixel, 2 bytes
    arry[28]=8;
    arry[29]=0;
    //compression method, 4 bytes
    for(char i=30;i<34;i++) arry[i]=0; //assuming 0 compression
    //size of raw image data, 4 bytes
    for(char i=34;i<38;i++) arry[i]=0; //dummy size of 0 for uncompressed BMPs
    //horizontal resolution (pixels per meter), 4 bytes
    for(char i=38;i<42;i++) arry[i]=0;
    //vertical resolution (pixels per meter), 4 bytes
    for(char i=42;i<46;i++) arry[i]=0;
    //number of colors in color palette, 4 bytes
    arry[46]=0;
    arry[47]=1; //assuming 256 colors
    for(char i=48;i<50;i++) arry[i]=0;
    //number of important colors, 4 bytes
    for(char i=50;i<54;i++) arry[i]=0; //generally ignored
    /* COLOR TABLE */
    //color palette, 4 bytes per entry
    for(int i=54;i<256*4+54;i+=4){
        arry[i]=(i-54)/4;           // This produces a color image with
        arry[i+1]=2*((i-54)/4)%256; // 256 colors -- using the same amount
        arry[i+2]=3*((i-54)/4)%256; // of storage as 8-bit grayscale.
        arry[i+3]=0;
    }
    /* WRITE TO FILE */
    //write header to file
    for(int i=0;i<54+256*4;i++){
        fprintf(f,"%c",arry[i]);
    }
    //write pixel data to file
    const char padding[]={0,0,0}; //at most 3 padding bytes are needed
    for(int i=y_res-1;i>=0;i--){
        fwrite(&pic[i*y_res],1,x_res,f);
        fwrite(padding,1,endPad,f); //pad rows to 4-byte multiples
    }
    fclose(f);
}
