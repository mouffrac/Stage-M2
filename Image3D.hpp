/***************************************************************************\
 * Copyright (C) by University Paris-Est - MISS team   
 * Image.hpp created in 09 2008.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.	 If not, see <http://www.gnu.org/licenses/>.
 *
\***************************************************************************/

/*
 * Anti-doublon
 */
#ifndef __OPENKN_IMAGE__IMAGE3D_HPP__
#define __OPENKN_IMAGE__IMAGE3D_HPP__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma warning(disable:4996)
#endif

/*
 * External Includes
 */
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <algorithm>

/*
 * Internal Includes
 */
#include "ImageException.hpp"
#include "Image.hpp"
/*
 * Namespace
 */
namespace kn{
	
		/*
		 * Class definition
		 */

		/** \brief Container for images 3D
		 *	This class represent any generic image 3D for any size, and basic type and any nb of channels.
		 *	The data of the image 3D is represented as a unidimensionnal array of the basic typename.
		 *	Moreover, we store the position of any row and any frame allowing a fast acces to any voxel of the image.
		 *	The first row is the top row and each row is stored from left to right.
		 */
		template<typename T>
		class Image3D {

			/*
			 * Constructor & destructors
			 */
		public:
			/** \brief Constructor to build a 0 size image 3D
			 */
			Image3D();

			/** \brief Constructor to build an allocated empty image 3D or an image 3D from a raw data buffer
			 *	\param width width of the image 3D
			 *	\param height height of the image 3D
			 *   	\param depth depth of the image 3D
			 *	\param nbChannel nb of channel of the image 3D
			 *	\param buffer optional data buffer WHICH IS NOT COPYIED
			 */
			Image3D(const size_t width, const size_t height, const size_t depth, const size_t nbChannel, T* buffer=0);

			/** \brief Copy Constructor
			 * \param i Image 3D to clone
			 */
			Image3D(const Image3D<T> & i);

			/** \brief dealoccates image 3D data
			 */
			virtual ~Image3D();

           
		protected :
			/** \brief raw data of the image 3D 
			 */
			T * data;
			
		
			/** \brief pointers on each frame of the image 3D on the raw date
			 */
			T ** frames;

			/** \brief pointers on each row of the image 3D on the raw date
			 */
			T *** rows;
			
			
			/** \brief allocate and initialize the rows array
			 */
			void initRowsAndFrames();

			/** \brief pointer on the begining of raw data
			 */
			T * begin_;

			/** \brief pointer on the end of raw data
			 */
			T * end_;

		public :
			/** \brief getting the raw data of the image
			 * \return raw data of the image
			 */
			inline virtual T  * begin() const {return begin_;}

			/** \brief getting the end of raw data of the image
			 * \return end of raw data of the image
			 */
			inline virtual T  * end() const {return end_;}
			
		protected :
			/** \brief width of the image
			 */
			size_t imageWidth;

			/** \brief height of the image
			 */
			size_t imageHeight;

			/** \brief depth of the image
			 */
			size_t imageDepth;
			
			/** \brief nb of channel of the image
			 */
			size_t imageNbChannel;

			/** \brief Total size of the image (w*h*d)
			 */
			size_t imageSize;
		public:
			/** \brief getting the width of the image
			 * \return the width of the image
			 */
			inline virtual size_t width() const {return imageWidth; }

			/** \brief getting the height of the image
			 * \return the height of the image
			 */
			inline virtual size_t height() const {return imageHeight; }

			/** \brief getting the depth of the image
			 * \return the depth of the image
			 */
			inline virtual size_t depth() const {return imageDepth; }

			/** \brief getting the nb of channel of the image
			 * \return the nb of channel of the image
			 */
			inline virtual size_t nbChannel() const {return imageNbChannel; }

			/** \brief getting the total size of the image
			 * \return the total of the image
			 */
			inline virtual size_t size() const {return imageSize; }
			
			/** \brief get pointer on the asked pixel
			 * \param x width position of the pixel
			 * \param y height position of the pixel
			 * \param z depth position of the pixel
			 * \return a buffer with the pixel
			 */
			inline virtual T * operator()(const unsigned int x, const unsigned int y, const unsigned int z) const {
				return (rows[y][z]+x*imageNbChannel);
			}

			/** \brief get pointer on the asked pixel (with bounds checking).
			 * This function is the same than the operator() but checks if indices are valid.
			 * \param x width position of the pixel
			 * \param y height position of the pixel
			 * \param z depth position of the pixel
			 * \return a buffer with the pixel
			 */
			inline virtual T * at(const unsigned int x, const unsigned int y, const unsigned int z) const {
				if (x>=imageWidth || y>=imageHeight || z>=imageDepth ) {
					std::ostringstream o("Operator("); o << x << "," << y << "," << z << ") " ;
					throw ImageException(o.str(), "Invalid range");
				}
				return (*this)(x,y,z);//(rows[y]+x*nbChannel());
			}

			/** \brief get component value of the asked pixel
			 *	\param x width position of the pixel
			 *	\param y height position of the pixel
			 *	\param z depth position of the pixel
			 *	\param channel component number
			 *	\return a value of the component
			 */
			inline virtual T & operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int channel) {
				return (*this)(x,y,z)[channel];
			}

			/** \brief get component value of the asked pixel (with bounds checking).
			 *	\param x width position of the pixel
			 *	\param y height position of the pixel
			 *	\param z depth position of the pixel
			 *	\param channel component number
			 *	\return a value of the component
			 */
			inline virtual T & at(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int channel) {
				if (channel>=imageNbChannel) {
					std::ostringstream o("Operator("); o << x << "," << y << "," << z << "," << channel <<") " ;
					throw ImageException(o.str(), "Invalid range");
				}
				return (at(x,y,z)[channel]);
			}

			/** \brief get component value of the asked pixel
			 * \param x width position of the pixel
			 * \param y height position of the pixel
			 * \param z depth position of the pixel
			 * \param channel component number
			 * \return a value of the component
			 */
			inline virtual const T & operator()(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int channel)const {
				return (*this)(x,y,z)[channel];
			}

			/** \brief get component value of the asked pixel (with bounds checking)
			 * This function behaves the same than the operator() but checks if indices are valid respect to
			 * the size of
			 * \param x width position of the pixel
			 * \param y height position of the pixel
			 * \param z depth position of the pixel
			 * \param channel component number
			 * \return a value of the component
			 */
			inline virtual const T & at(const unsigned int x, const unsigned int y, const unsigned int z, const unsigned int channel)const {
				if (channel>=imageNbChannel) {std::ostringstream o("Operator("); o << x << "," << y << "," << z << "," <<channel<< ") " ; throw ImageException(o.str(), "Invalid range"); }
				return (*this)(x,y,z)[channel];
			}
			
			/** \brief get component value of the asked pixel (with bounds checking)
			 * This function behaves the same than the operator() but checks if indices are valid respect to
			 * the size of
			 * \param x width position of the pixel
			 * \param y height position of the pixel
			 * \param z depth position of the pixel
			 * \param color : array containing the resulting color, should have a nbchannel size.
			 */
			void atBilinear(const double x, const double y, const double z, T *color)const;


			/** \brief copy on this image the content of another if they have the same resolution and nb channel; if the calling image have width = height = depth = 0, this function create the adequat memory, else throws an exception.
			 * \param i : source image
			 * \throw ImageException : incompatible image size
			 * \return a reference to the destination image
			 */
			virtual Image3D<T> & operator=(const Image3D &i) ;


			/** \brief fill all the voxels component to the parameter value
			 *  \param value : value used to fill the image
			 */
			inline void fill(const T &value) {
			  std::fill(begin_,end_,value);
			}

			/** \brief fill all the voxels of a given frame with the values of a given image
			 *  \param index : index of the frame
			 *  \param frame : image which will be used for filling
			 *  \throw ImageException : invalid frame dimensions
			 *  \throw ImageException : index out of bound
			 */
			inline void setFrame(unsigned int index, Image<T>& frame){
				if(frame.width() != imageWidth || frame.height() != imageHeight || frame.nbChannel() != imageNbChannel)
					throw ImageException("invalid frame dimensions", "Image3D::setFrame");
				if(index >= imageDepth)
					throw ImageException("index out of bound", "Image3D::setFrame");
				std::copy(frame.begin(), frame.end(), frames[index]);
			} 
			
			/** \brief fill an image with the values of a given frame
			 *  \param index : index of the frame
			 *  \param frame : image which will be filled
			 *  \throw ImageException : invalid frame dimensions
			 *  \throw ImageException : index out of bound
			 */
			inline void getFrame(unsigned int index, Image<T>& frame){
				if(frame.width() != imageWidth || frame.height() != imageHeight || frame.nbChannel() != imageNbChannel)
					throw ImageException("Invalid frame dimensions", "Image3D::setFrame");
				if(index >= imageDepth)
					throw ImageException("index out of bound", "Image3D::setFrame");
				std::copy(frames[index], frames[index]+frame.size(), frame.begin());
			} 

			/*
			 * Building and erasing images
			 */
		private:
			/** \brief This function is used ONLY ON 0 SIZED Images to build an image from a buffer which can optionnaly be NOT copied
			 * \param width width of the image
			 * \param height height of the image
			 * \param depth depth of the image
			 * \param nbChannel number of channels of the image
			 * \param buffer data buffer
			 * \param copy flag to control wether the buffer is copied or not
			 */
			void buildImageFromBuffer(const size_t width, const size_t height, const size_t depth, const size_t nbChannel, T* buffer, const bool copy=true);

			/** \brief This function is used ONLY ON 0 SIZED Images to build an image from another Image which can optionnaly be NOT copied
			 * \param i the source image
			 * \param copy flag to control wether the buffer is copied or not
			 */
			void buildImageFromImage(const Image3D<T> & i, const bool copy=true);

		private:
			/** \brief This function is used internaly ONLY ON 0 SIZED Images to build an empty image
			 * \param width width of the image
			 * \param height height of the image
			 * \param depth depth of the image
			 * \param nbChannel number of channels of the image
			 */
			void buildEmptyImage(const size_t width, const size_t height, const size_t depth, const size_t nbChannel);

			/** \brief This function allows to empty/erase the data of an image
			 */
			void erase();

		};

		/*
		 * Templates definitions
		 */

		/*
		 * Functions definitions
		 */
		template<typename T>
		Image3D<T>::Image3D() {
			imageWidth = 0;
			imageHeight = 0;
			imageDepth = 0;
			imageNbChannel = 0;
			imageSize = 0;
			frames = 0;
			rows = 0;
			data = 0;
			begin_ = 0;
			end_ = 0;
			//std::cerr<<"CONSTRUCT VIDE"<<std::endl;
		}

		template<typename T>
		Image3D<T>::Image3D(const size_t width, const size_t height, const size_t depth, const size_t nbChannel, T* buffer) {
			imageSize = 0;
			imageWidth = 0;
			imageHeight = 0;
			imageDepth = 0;
			imageNbChannel = 0;
			imageSize = 0;
			
			rows=0;
			frames = 0;
			data=0;
			begin_ = 0;
			end_ = 0;

			if (buffer) {
				buildImageFromBuffer(width,height,depth,nbChannel,buffer);
			}
			else {
				buildEmptyImage(width,height,depth,nbChannel);
			}
		}

		template<typename T>
		Image3D<T>::Image3D(const Image3D<T> & i) {
			imageSize = 0;
			imageSize = 0;
			imageWidth = 0;
			imageHeight = 0;
			imageDepth = 0;
			imageNbChannel = 0;
			imageSize = 0;
			
			rows=0;
			frames=0;
			data=0;
			begin_ = 0;
			end_ = 0;

			buildImageFromImage(i,true);
		}

		template<typename T>
		Image3D<T>::~Image3D() {
			erase();
		}

		template<typename T>
		void Image3D<T>::initRowsAndFrames() {
			unsigned int i,j;
			if(rows) {
				delete[] rows;
				rows = 0;
			}
			if(frames) {
				delete[] frames;
				frames = 0;
			}
			
			frames = new T*[imageDepth*imageHeight];
			rows = new T**[imageHeight];
			
			unsigned int frameDec = imageHeight*imageWidth*imageNbChannel;
			unsigned int rowDec = imageWidth*imageNbChannel;
			
			for (i =0; i <imageHeight; i++) {
				rows[i] = frames+i*imageDepth;
				for (j =0; j <imageDepth; j++) {
					frames[i*imageDepth+j] = data+j*frameDec+i*rowDec;
				}
			}
		}
		
		template<typename T>
		void Image3D<T>::buildImageFromBuffer(const size_t width, const size_t height, const size_t depth, const size_t nbChannel, T* buffer, const bool copy) {
			if (size()) {
				throw ImageException("buildImageFromImage","Image already allocated");
			}
			imageWidth = width;
			imageHeight = height;
			imageDepth = depth;
			imageNbChannel = nbChannel;
			imageSize = imageWidth*imageHeight*imageDepth*imageNbChannel;
			if (copy) {
				if(data){
					delete[] data;
					data = 0;
				}
				data = new T[size()*sizeof(T)];
				std::copy(buffer,buffer+size(),data);
			}
			else {
				if(data){
					delete[] data;
				}
				data = buffer;
			}
			initRowsAndFrames();

			begin_ = data;
			end_ = data+imageSize;
		}
		
		template<typename T>
		void Image3D<T>::buildImageFromImage(const Image3D<T> & i, const bool copy) {
			buildImageFromBuffer(i.imageWidth,i.imageHeight,i.imageDepth,i.imageNbChannel,i.begin_,copy);
		}

		template<typename T>
		void Image3D<T>::buildEmptyImage(const size_t width, const size_t height, const size_t depth, const size_t nbChannel) {
			if(imageSize) {
				throw ImageException("buildEmptyImage","Image already allocated");			   
			}			
			imageWidth = width;
			imageHeight = height;
			imageDepth = depth;
			imageNbChannel = nbChannel;
			imageSize = width * height * depth * nbChannel;
            if(data){
                delete[] data;
                data = 0;
            }
			data = new T[imageSize * sizeof(T)];
			initRowsAndFrames();

			begin_ = data;
			end_ = data+imageSize;
		}
		
		
		template<typename T>
		void Image3D<T>::erase() {
			imageWidth = 0;
			imageHeight = 0;
			imageDepth = 0;
			imageNbChannel = 0;
			imageSize = 0;
			if(data) delete[] data;
			if(rows) delete[] rows;
			if(frames) delete[] frames;
			data = 0;
			rows = 0;
			frames = 0;
			begin_ = 0;
			end_ = 0;
		}


		template<typename T>
		Image3D<T> & Image3D<T>::operator=(const Image3D &i) {
			// precaution
			if(&i == this) return *this;

			// 0 size image
			if(this->imageWidth  == 0 && this->imageHeight ==0 && this->imageDepth ==0){
			    buildImageFromImage(i);
			    return *this;
			}

			// standard copy
			if(this->imageWidth  == i.imageWidth  && 
			   this->imageHeight == i.imageHeight &&
			   this->imageDepth == i.imageDepth &&
			   this->imageNbChannel == i.imageNbChannel)
			std::copy(i.begin_,i.end_,this->begin_);
			else throw ImageException("operator=","incompatible image size");

			return *this;
		}
		
		template<typename T>
		void Image3D<T>::atBilinear(const double x, const double y, const double z, T *color)const{
			// 4 concerned pixels on floor(z)
			double x1 = floor(x);
			double y1 = floor(y);
			int    x2 = std::min((int)x1+1, (int)imageWidth-1);
			int    y2 = std::min((int)y1+1, (int)imageHeight-1);
			double z1 = floor(z);

			// default : border is black
			std::vector<T> black(imageNbChannel);
			std::fill(black.begin(),black.end(),T(0));
			T *color1  = &(black[0]);
			T *color2  = &(black[0]);
			T *color3  = &(black[0]);
			T *color4  = &(black[0]);
			
			// colors
			color1 = this->at((int)x1,(int)y1,(int)z1);
			color2 = this->at((int)x1,y2,(int)z1);
			color3 = this->at(x2,y2,(int)z1);
			color4 = this->at(x2,(int)y1,(int)z1);
			  
			// first component
			std::vector<T> firstComponent(imageNbChannel);
			for(unsigned int c=0; c<imageNbChannel; ++c)
			    firstComponent[c] = (T)(((x-x1)*color4[c] + (1.0-(x-x1))*color1[c]) * (1.0-(y-y1)) +
			                            ((x-x1)*color3[c] + (1.0-(x-x1))*color2[c]) * (y-y1));
			                            			                   	                   
			// 4 concerned pixels on floor(z)+1
			int z2 = std::min((int)z1+1, (int)imageDepth-1);
			color1  = &(black[0]);
			color2  = &(black[0]);
			color3  = &(black[0]);
			color4  = &(black[0]);			                  
			
			// colors
			color1 = this->at((int)x1,(int)y1,(int)z2);
			color2 = this->at((int)x1,y2,(int)z2);
			color3 = this->at(x2,y2,(int)z2);
			color4 = this->at(x2,(int)y1,(int)z2);			
			
    		// second component
	    	std::vector<T> secondComponent(imageNbChannel);
			for(unsigned int c=0; c<imageNbChannel; ++c)
			    secondComponent[c] = (T)(((x-x1)*color4[c] + (1.0-(x-x1))*color1[c]) * (1.0-(y-y1)) +
			                             ((x-x1)*color3[c] + (1.0-(x-x1))*color2[c]) * (y-y1));
			
			// final color 
		    for(unsigned int c=0; c<imageNbChannel; ++c)
			    color[c] = (1-(z-z1))*firstComponent[c] + (z-z1)*secondComponent[c];			
		}
		
		
		
		
		

		/*
		 * End of Namespace
		 */
	}

/*
 * End of Anti-doublon
 */
#endif
