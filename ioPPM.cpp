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


/*
 * External Includes
 */
#include "sstream"
#include "iostream"
#include "fstream"

/*
 * Internal Includes
 */
#include "ioPPM.hpp"
#include "ImageException.hpp"

/*
 * Namespace
 */
namespace kn{
	// Skip extra comment starting with '#'
	char skipComment(std::istream& is) {
		char c = is.get();
		//std::cerr << "skipComment:caractere("<<c<<")"<<std::endl;
		//if (c == EOF) cerr<<"Yeppeeee !"<<endl;
		while ((c == ' ') ||(c == '\n') || (c == '#'))
		{
			//std::cerr << "skipComment:caractere("<<c<<")"<<std::endl;
			if (c == '#') do { c = is.get(); /*std::cerr <<c;*/ } while (c != '\n');
			c = is.get();
		}
		is.putback(c);
		return c;
	}

	// Skip extra comment starting with '#'
	char skipSpace(std::istream& is) {
		char c = is.get();
		//std::cerr << "skipComment:caractere("<<c<<")"<<std::endl;
		//if (c == EOF) cerr<<"Yeppeeee !"<<endl;
		while ((c == ' ') || (c == '\n') || (c == '\t')) {
			c = is.get();
		}
		is.putback(c);
		return c;
	}

	/*
	 * Functions definitions
	 */
	int loadPPM(Image<unsigned char>& res,const std::string& filename) {
		std::fstream is;
		unsigned int componentperpixel = 1;
		bool isascii = false;
		bool isonebitperpixel = false;
		size_t width,height,size=0;

		//std::cout<<"READING "<<filename<<std::endl;
		// STEP 0 : WE CHECK THAT THE IMAGE IS NOT ALREADY ALLOCATED
		if (res.size()>0) {
			throw ImageException("Image already allocated. Cannot store PPM in that image.","loadFile");
		}

		// STEP 1 : READING HEADER
		is.open(filename.c_str(),std::ios::in);
		if (!is.is_open()) {
			throw ImageException("File " + filename + " not found or unreadable","loadFile");
		}
		std::string content;
		is >> content;
		if ((content.size()<2) || (content[0] != 'P') ||
				((int)(content[1]-'0')<1) || ((int)(content[1]-'0')>6) ) {
			throw ImageException("File format error : Wrong header ("+content+")","loadFile");
		}
		int code = (content[1]-'0');
		//std::cerr<<"CODER :"<<code<<":"<<std::endl;
		switch (code) {
			case (1) :
				isascii = true;
			case (4) :
				isonebitperpixel = true;
				break;
			case (2) :
				isascii = true;
			case (5) :
				break;
			case (3) :
				isascii = true;
			case (6) :
				componentperpixel = 3;
				break;
			default:
				throw ImageException("File format error : Wrong header ("+content+")","loadFile");
				break;
		}

		// Reading of width and height
		char c = skipComment(is);
		//std::cerr<<"Skip ("<<c<<")"<<std::endl;
		is >> width;
		c = skipComment(is);
		//std::cerr<<"Skip ("<<c<<")"<<std::endl;
		is >> height;
		c = skipComment(is);
		//std::cerr<<"Skip ("<<c<<")"<<std::endl;
		if (!isonebitperpixel) {
			is >> size;
			c = is.get();
			//c = (unsigned char)skipComment(is);
		}
		if ((width<=0) || (height<=0)) {
			std::stringstream serr;
			serr << "File format error : width or size negative or null (w=";
			serr << width << "/h=" << height <<")";
			throw ImageException(serr.str(),"loadFile");
		}
		if ((!isonebitperpixel) && (size != 255)) {
			throw ImageException("File format problem : We deal only with 255 level of color. This image is "+size,"loadFile");
		}

		// STEP 2 : READING THE DATA
		unsigned char* thebuffer = NULL;
		char pixchar;
		int pixel;
		thebuffer = new unsigned char[componentperpixel*width*height];
		// Reading ascii file (erk)
		if (isascii) {
			for(size_t i=0;i<(unsigned int)(width*height*componentperpixel);i++) {
				// In case of P1 file, we handle both packed 0 and 1 and spaced 0 and 1...
				if (isonebitperpixel) {
					pixchar = skipSpace(is);
					pixchar = is.get();
				}
				else{
					is >> pixel;
				}
				if (!isonebitperpixel && ((pixel<0) || (pixel>255)))
					throw ImageException("File format problem : We deal only with 255 level of color. One pixel is "+pixel,"loadFile");
				if (isonebitperpixel){
					thebuffer[i]=((int)(pixchar-'0')==0)?255:0;
					//std::cerr<<"I read and store "<<(int)thebuffer[i]<<"("<<i<<")"<<std::endl;
				}
				else {
					thebuffer[i]=(unsigned char)pixel;
					//std::cerr<<"I read and store "<<(int)thebuffer[i]<<"("<<i<<")"<<std::endl;
				}
			}
		}
		// Reading binary file
		else {
			if (isonebitperpixel) {
				// WARNING This file format doesn't belong to the standard : We choose
				//		to follow the Gimp way of dealing this. Each line is treated
				//		separatedly and packed.
				unsigned int sizebitwidth = (width%8 == 0)?(width/8):(width/8)+1;
				unsigned int sizebitbuffer = sizebitwidth*height;
				char* firstbuffer = new char[sizebitbuffer];
				unsigned int index = 0;
				unsigned int decal = 0;
				unsigned char masq = 0x00;
				//std::cerr<<"TEST : "<<(0x01<<7)<<"/"<<(int)0x80<<std::endl;
				is.read(firstbuffer,sizebitbuffer);
				for(size_t i=0;i<height;i++) {
					// Decomposition elements
					for(size_t j=0;j<width;j++) {
						masq = 0x01;
						index = j/8;
						decal = j%8;
						masq = masq << (7-decal);
						thebuffer[i*width+j] = ((int)(masq & firstbuffer[i*sizebitwidth+index])>0)?0:255;
					}
				}
			}
			else {
				is.read((char*)thebuffer,width*height*componentperpixel*sizeof(unsigned char));
				//std::cout << thebuffer[0] << std::endl;
			}
		}
		is.close();

		// STEP 2 bis : CONVERTING DATA
		if ((res.nbChannel()>0) && (componentperpixel != res.nbChannel())) {
			unsigned char* buffer_bis = new unsigned char[res.nbChannel()*width*height];
			if ((componentperpixel == 3) && (res.nbChannel()==1)) {
				// CONVERTIR IMAGE RGB en GREYSCALE...
				for(unsigned int i=0;i<width*height;i++) {
					buffer_bis[i] = (unsigned char)((int)(thebuffer[3*i]+thebuffer[3*i+1]+thebuffer[3*i+2])/3);
				}
			}
			else if ((componentperpixel == 1) && (res.nbChannel()==3)) {
				//std::cerr<<"Conversion between 1 => 3 "<<std::endl;
				// CONVERTIR IMAGE GREYSCALE en RGB...
				for(unsigned int i=0;i<width*height;i++) {
					buffer_bis[3*i  ] = (unsigned char)(thebuffer[i]);
					buffer_bis[3*i+1] = (unsigned char)(thebuffer[i]);
					buffer_bis[3*i+2] = (unsigned char)(thebuffer[i]);
				}
			}
			else {
				std::stringstream serr;
				serr << "When loading a PPM file with nbChannel "<<componentperpixel<<", conversion impossible for image nbChannel "<<res.nbChannel();
				throw ImageException(serr.str(),"loadFile");
			}
			delete[](thebuffer);
			thebuffer = buffer_bis;
		}

		// STEP 3 : MAKING THE IMAGE
		try {
			res.buildImageFromBuffer(width,height,componentperpixel,thebuffer);
		}
		catch (ImageException &e) {
			// Should absolutely not happens
			throw ImageException("Exception rising when building image from buffer : "+e.errorString(),"loadFilePPM");
		}
		if (thebuffer != NULL) delete[](thebuffer);
		return componentperpixel;
	}


	bool savePPM(const Image<unsigned char>& res,const std::string& filename) {

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
		return (1/*os.rdstate() != std::ios_base::badbit*/);
	}



	/*
	 * End of Namespace
	 */
}
