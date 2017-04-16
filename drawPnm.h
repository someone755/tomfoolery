void drawPnm(int x_res, int y_res, unsigned char arr[x_res][y_res],char fname[],unsigned char mode){
    FILE *f = fopen(fname,"wb");
    //FILE is an object type
    //*f points to the newly created "fname" in "wb" (binary write/overwrite) mode
    if(f==NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    switch(mode){
        case 0: //mode 0 for grayscale
            fprintf(f,"P5\n%d %d\n255\n",x_res,y_res); //pbm type 5 (8-bit grayscale), resolution (hotizontal, vertical), color step (255)
            printf("hello");
            fwrite(arr,1,x_res*y_res,f);
            break;
        case 1: //mode 1 for color
            fprintf(f,"P6\n%d %d\n255\n",x_res,y_res); //pbm type 6 (24-bit color), resolution (hotizontal, vertical), color step (255)
            for(int i=0;i<y_res;i++){
                for(int j=0;j<x_res;j++){
                    fprintf(f,"%c%c%c",(arr[i][j]*3)%256,\
                                       (arr[i][j]*2)%256,\
                                        arr[i][j]); //RGB
                }
            }
    }
    fclose(f);
}
