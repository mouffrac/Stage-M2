/***************************************************************************\
 * Copyright (C) by University Paris-Est - MISS team   
 * ioPNG.hpp created in 01 2010.
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
 * Anti-doublon
 */
#ifndef __OPENKN_IMAGE__IOPNG_HPP__
#define __OPENKN_IMAGE__IOPNG_HPP__

/*
 * External Includes
 */
#include <string>
#include <png.h>


/*
 * Internal Includes
 */
#include "Image.hpp"

/*
 * Namespace
 */
namespace kn{
	/*
	* Class definition
	*/
		/** \addtogroup Macros */
		/*@{*/
		/** \defgroup PNG_image_type Handled PNG Image Type */
		/*@{*/
		enum PNG_file_type {NONE_PNG=0, GRAYSCALE_PNG=PNG_COLOR_TYPE_GRAY, COLOR_RGB_PNG=PNG_COLOR_TYPE_RGB,
												COLOR_RGBA_PNG = PNG_COLOR_TYPE_RGBA};
		/*@}*/
		/*@}*/


	/** \brief Loader function for PNG image.
		* This is a generic PNG loading function. It handles several option of the PNG file but not all type.<br>
		* Exact format is defined here : http://www.w3.org/TR/PNG/
		* \param res Image receving data from file
		* \param filename Name of the file to load
		* \return int indicating number of composant per pixel
		* \todo Must this function check that the extension of the name file is correct ? (.png/.PNG ...)
		*/
	int loadPNG(Image<unsigned char>& res,const std::string& filename);

	/** \brief Export function for PPM/PGM/PBM image.
		* This is a generic PNG exporting function.
		* It handles classical PNG format file.
		*
		* Exact format is defined here : http://www.w3.org/TR/PNG/
		* We only export PPM and PGM files with raw data format so P5 and P6 file format
		* \param res Image that contents
		* \param filename Name of the file to export
		* \return Boolean indicating succes of export operation
		* \TODO Function undone yet
		* bool savePPM(Image<unsigned char>& res,const std::string& filename);
		*/

	/*
	 * End of Namespace
	 */
}

/*
 * End of Anti-doublon
 */
#endif
#endif