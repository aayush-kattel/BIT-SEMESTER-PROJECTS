#ifndef COURSEWORK_LOAD_AND_BIND_TEXTURE_H
#define COURSEWORK_LOAD_AND_BIND_TEXTURE_H

#include <GL/glut.h>

unsigned int load_and_bind_texture(const char* filename)
{
    char* image_buffer = NULL; 
	int width = 0;
	int height = 0;

	// Read png file
	if (png_load(filename, &width, &height, &image_buffer)==0)
    {
        fprintf(stderr, "Failed to read image texture from %s\n", filename);
        exit(1);
    }

	// request one texture handle
	unsigned int tex_handle = 0;
	glGenTextures(1, &tex_handle); 

	// Bind texture to texture handle
  	glBindTexture(GL_TEXTURE_2D, tex_handle);

	// set texture parameters 
  	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  	glTexImage2D(GL_TEXTURE_2D, 0, 
				GL_RGBA, width, height, 0,
   		 		GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image_buffer);

    glBindTexture(GL_TEXTURE_2D,0);

	free(image_buffer); // free the image buffer memory
	
	return tex_handle;
}

#endif 
