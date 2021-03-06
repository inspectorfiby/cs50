/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Resizes a BMP file.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize n infile outfile\n");
        return 1;
    }

    // remember filenames and resize factor
    char* infile = argv[2];
    char* outfile = argv[3];
    int n = atoi(argv[1]);
    if (n < 1 || n > 100)
    {
        printf("n must be between 1 and 100\n");
        return 1;
    }
    
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf, r_bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    r_bf = bf;

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi, r_bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    r_bi = bi;

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determine new dimensions
    r_bi.biWidth = n * bi.biWidth;
    r_bi.biHeight = n * bi.biHeight;
    
    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int r_padding = (4 - (r_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // determine new size
    r_bi.biSizeImage = (r_bi.biWidth * sizeof(RGBTRIPLE) + r_padding) * abs(r_bi.biHeight);
    r_bf.bfSize = bf.bfSize - bi.biSizeImage + r_bi.biSizeImage;
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&r_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&r_bi, sizeof(BITMAPINFOHEADER), 1, outptr);



    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight ; i++)
    {
        // iterate each row for n times
        for (int l = 0; l < n; l++)
        {
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
        
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            
                for (int m = 0; m < n; m++)
                {
                // write RGB triple to outfile for n times
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
                
                // add padding
                for (int a = 0; a < r_padding; a++)
                    fputc(0x00, outptr);
                    
                if (l <  n - 1)
                    fseek(inptr, -bi.biWidth * sizeof(RGBTRIPLE), SEEK_CUR);
        }
            fseek(inptr, padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
