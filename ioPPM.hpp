/***************************************************************************\
 * Copyright (C) by University Paris-Est - MISS team
 * ioPPM.hpp created in 10 2008.
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
 * Anti-doublon
 */
#ifndef __OPENKN_IMAGE__IOPPM_HPP__
#define __OPENKN_IMAGE__IOPPM_HPP__

/*
 * External Includes
 */
#include <string>


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

	/** \brief Loader function for PPM/PGM/PBM image.
		* This is a generic PPM loading class. It handles P1 / P2 / P3 / P4 / P5 and P6 PPM format file.<br>
		* Exact format is defined here : http://local.wasp.uwa.edu.au/~pbourke/dataformats/ppm/
		* but we handle only 255 different colour values (unsigned char). Boolean image (PBM)
		* are converted to greyscale image (1 value being mapped to 0 and 0 to 255
		* (that is the tradition)<br>
		* <b>Beware</b>, P4 PBM file are not supposed to exist.
		* We handle them as Gimp does : each line is stored independently.<br>
		* \param res Image receving data from file
		* \param filename Name of the file to load
		* \return int indicating number of composant per pixel
		* \todo Must this function check that the extension of the name file is correct ? (.ppm/.PPM ...)
		*/
	int loadPPM(Image<unsigned char>& res,const std::string& filename);

	/** \brief Export function for PPM/PGM/PBM image.
		* This is a generic PPM exporting class.
		* It handles P1 / P2 / P3 / P4 / P5 and P6 PPM format file.
		*
		* Exact format is defined here : http://local.wasp.uwa.edu.au/~pbourke/dataformats/ppm/
		* We only export PPM and PGM files with raw data format so P5 and P6 file format
		* \param res Image that contents
		* \param filename Name of the file to export
		* \return Boolean indicating succes of export operation
		*/
	bool savePPM(const Image<unsigned char>& res,const std::string& filename);

	/*
	 * End of Namespace
	 */
}

/*
 * End of Anti-doublon
 */
#endif
