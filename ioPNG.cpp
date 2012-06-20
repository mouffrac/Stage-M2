/***************************************************************************\
 * Copyright (C) by University Paris-Est - MISS team
 * ioPPM.cpp created in 10 2008.
 * Mail : biri@univ-mlv.fr
 *
 * This file is part of the OpenKraken-image.
 *
 * The OpenKraken-image is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * The OpenKraken-image is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
\***************************************************************************/

#ifndef __APPLE__
/*
 * External Includes
 */
#include "sstream"
#include "iostream"
#include "fstream"

/*
 * Internal Includes
 */
#include "ioPNG.hpp"
#include "ImageException.hpp"

#define PNG_BYTES_TO_CHECK 4


/*
 * Namespace
 */
namespace kn{

	
	void errorOrWarningCatch(png_structp /*str_ptr*/,const char* message) {
		throw ImageException(message,"loadImagePNG");
	}

	
	/*
	 * Functions definitions
	 */
	int loadPNG(Image<unsigned char>& res,const std::string& filename) {
		FILE * infile;			/* source file */
		infile = fopen(filename.c_str(),"rb");
		unsigned char buf[PNG_BYTES_TO_CHECK];
		png_structp png_ptr;
		png_infop info_ptr;
		unsigned int sig_read = PNG_BYTES_TO_CHECK;
		png_uint_32 width, height;
		int bit_depth, color_type, interlace_type;
		unsigned char errString[100];
		int componentperpixel = 0;
		int type = NONE_PNG;

		// We check if Image does not exist already
		if (res.size()>0) {
			throw ImageException("Image already allocated. Cannot store HDR image in that image.","loadImage");
		}

		/* *************************************************************************************
			 ** INITIALISATION OF DATA STRUCTURE
			 ************************************************************************************* */

		/* Open the prospective PNG file. */
		if ((infile = fopen(filename.c_str(), "rb")) == NULL)
			throw ImageException("Error could not read file "+filename,"loadImagePNG");

		/* Read in the signature bytes */
		if (fread(buf, 1, PNG_BYTES_TO_CHECK, infile) != PNG_BYTES_TO_CHECK) {
			fclose(infile);
			throw ImageException("Could not read header of PNG file "+filename,"loadImagePNG");
		}

		/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature. */
		if(png_check_sig(buf, PNG_BYTES_TO_CHECK) == 0) {
			fclose(infile);
			throw ImageException("Image "+filename+" does not appear to be a PNG file. Wrong header","loadImagePNG");
		}

		/* Create and initialize the png_struct with the desired error handler
		* functions.  If you want to use the default stderr and longjump method,
		* you can supply NULL for the last three parameters.  We also supply the
		* the compiler header file version, so that we know if the application
		* was compiled with a compatible version of the library.  REQUIRED
		*/
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,(png_byte*)errString, errorOrWarningCatch, errorOrWarningCatch);
		if (png_ptr == NULL)
		{
			fclose(infile);
			throw ImageException("Unable to create reading structure. Memory leak ?","loadImagePNG");
		}

		/* Allocate/initialize the memory for image information.  REQUIRED. */
		info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL)
		{
			fclose(infile);
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			throw ImageException("Unable to create reading info structure. Memory leak ?","loadImagePNG");
		}
		
		/* Set error handling if you are using the setjmp/longjmp method (this is
		* the normal method of doing things with libpng).  REQUIRED unless you
		* set up your own error handlers in the png_create_read_struct() earlier.
		*/
		if (setjmp(png_ptr->jmpbuf))
		{
			/* Free all of the memory associated with the png_ptr and info_ptr */
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			fclose(infile);
			/* If we get here, we had a problem reading the file */
			throw ImageException("Wrong jumping in reading PNG File (init)","loadImagePNG");
		}

		/* Set up the input control if you are using standard C streams */
		png_init_io(png_ptr, infile);

		/* If we have already read some of the signature */
		png_set_sig_bytes(png_ptr, sig_read);


		/* *************************************************************************************
			 ** READING HEADER
			 ************************************************************************************* */

		/* The call to png_read_info() gives us all of the information from the
		* PNG file before the first IDAT (image data chunk).  REQUIRED
		*/
		png_read_info(png_ptr, info_ptr);

		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,&interlace_type, NULL, NULL);

		//std::cerr<<"We got an image of"<<width<<" / "<<height<<" and bit_depth "<<bit_depth<<" and color_type "<<color_type<<std::endl;
		/*
		 color_type = 0 : Greyscale image
		 color_type = 2 : color image
		 color_type = 3 : color image with a palette
		 color_type = 6 : color image with alpha channel
		 color_type = 4 : greyscale image with alpha channel (juste alpha channel ?)
		*/
		switch(color_type) {
			case (PNG_COLOR_TYPE_GRAY) :
				type = GRAYSCALE_PNG;
				componentperpixel = 1;
				break;
			case (PNG_COLOR_TYPE_RGB) :
				type = COLOR_RGB_PNG;
				componentperpixel = 3;
				break;
			case (PNG_COLOR_TYPE_RGBA) :
				type = COLOR_RGBA_PNG;
				componentperpixel = 4;
				break;
			default:
				throw ImageException("We handle only greyscale, RGB and RGBA image. Color type is "+componentperpixel,"loadImagePNG");
		}

		/* HERE WE DO SOME SETTING OVER THE PNG STRUCT TO HANDLE SEVERAL CONFIGURATION OF READING */
		
		/*
		 * expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
		 * TO BE REMOVED if we want to handle pure 1,2 or 4 bit/pixel images
		 */
		if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			png_set_expand(png_ptr);

		/* expand paletted or RGB images with transparency to full alpha channels
		 * so the data will be available as RGBA quartets */
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);

		
		/* IF NECESSARY (don't think so) we can flip the RGB pixels to BGR (or RGBA to BGRA) with the following */
		/*png_set_bgr(png_ptr);*/
		/* IF NECESSARY (don't think so) swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR) */
		/*png_set_swap_alpha(png_ptr);*/

		/* IF NECESSARY Add filler (or alpha) byte (before/after each RGB triplet) */
		/*png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);*/

		/* *************************************************************************************
			 ** READING IMAGE
			 ************************************************************************************* */
		/* the easiest way to read the image */
		unsigned char** row_pointers = new unsigned char*[height];

		// Remap on a single chunk of bytes
		unsigned char* thebuffer = new unsigned char[height*png_get_rowbytes(png_ptr, info_ptr)];
		
		for (unsigned int row = 0; row < height; row++)
		{
			row_pointers[row] = thebuffer + row*png_get_rowbytes(png_ptr, info_ptr);
		}

		// Reading the whole image...
		png_read_image(png_ptr, row_pointers);

		/* *************************************************************************************
			 ** BUILDING IMAGE
			 ************************************************************************************* */
		try {
			res.buildImageFromBuffer(width,height,componentperpixel,thebuffer);
		}
		catch (ImageException &e) {
			// Should absolutely not happens
			throw ImageException("Exception rising when building image from buffer : "+e.errorString(),"loadFilePPM");
		}
		if (thebuffer != NULL) delete[](thebuffer);
		if (row_pointers != NULL) delete[](row_pointers);

		
		/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
		png_read_end(png_ptr, info_ptr);

		/* clean up after the read, and free any memory allocated - REQUIRED */
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

		/* close the file */
		fclose(infile);

		/* that's it */
		return type;
	}

	/*
	bool savePPM(Image<unsigned char>& res,const std::string& filename) {

		size_t componentperpixel = res.nbChannel();
		size_t width = res.width();
		size_t height = res.height();
		size_t size = res.size();

		if ( (componentperpixel == 0) || (width == 0) || (height == 0) || (size == 0)) {
			throw ImageException("No data in Image or data mismatch. No export in PPM file possible","exportFile");
		}
		if (size != width*height*componentperpixel*sizeof(unsigned char)) {
			throw ImageException("Something weird on data or not a UNSIGNED CHAR image","exportFile");
		}
		if ((componentperpixel!=1) && (componentperpixel!=3)) {
			std::stringstream serr;
			serr << "Component per pixel is "<<componentperpixel;
			serr << ". Unable to make a PPM with this image nbChannel"<<std::endl;
			throw ImageException(serr.str(),"exportFile");
		}

		std::ofstream os(filename.c_str(),std::ofstream::binary);
		if (!os.is_open()) {
			throw ImageException("Unable to open "+filename+" for writing","exportFile");
		}
		if (componentperpixel == 1) {
			os << "P5" <<std::endl;
		}
		else {
			os << "P6" <<std::endl;
		}
		os<<"# PPM Image generated by OpenKraken info(biri@univ-mlv.fr)"<<std::endl;
		os<<width<<" "<<height<<std::endl;

		os<<"255"<<std::endl;

		os.write((char*)res.begin(),size);

		os.close();
		// POUR ACCORD (?)
		*/
		/*
		return (1*//*os.rdstate() != std::ios_base::badbit*//*);
	}
	*/



	/*
	 * End of Namespace
	 */
}
#endif
