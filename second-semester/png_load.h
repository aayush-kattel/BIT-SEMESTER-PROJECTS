#ifndef COURSEWORK_PNGLOAD_H

int png_load(const char* file_name, 
		     int* width, 
			 int* height, 
			 char** image_data_ptr)
{
    //verify img is png or not
    png_byte header[8];

    FILE* fp = fopen(file_name, "rb");
    if (fp == 0)
    {
        fprintf(stderr, "erro: could not open PNG file %s\n", file_name);
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // Stores PNG info width, height, color type 
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // holds end metadata
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // error in libpng
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    png_init_io(png_ptr, fp);// reading png data

    png_set_sig_bytes(png_ptr, 8);// let libpng know you already read the first 8 bytes

    png_read_info(png_ptr, info_ptr);// read all the info of png

    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get png info
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (width) { *width = temp_width; }
    if (height){ *height = temp_height; }

    png_read_update_info(png_ptr, info_ptr);   // Update the png info struct.

    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    rowbytes += 3 - ((rowbytes-1) % 4); // glTexImage2d requires rows to be 4-byte aligned

    // Allocate the big chunk of memory for image data
    png_byte* image_data;
    image_data = (png_byte*)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep* row_pointers = (png_bytep*)malloc(temp_height * sizeof(png_bytep));
    if (row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    int i;
    for (i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    png_read_image(png_ptr, row_pointers);// read the png into image_data through row_pointers

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

	*image_data_ptr = (char*)image_data; // pass back the image data

    free(row_pointers);
    fclose(fp);

	return 1;
}

#endif 
