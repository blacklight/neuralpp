/**************************************************************************************************
 * LibNeural++ v.0.2 - All-purpose library for managing neural networks                           *
 * Copyright (C) 2009, BlackLight                                                                 *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version. This program is distributed in the hope    *
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details. You should have received a copy of the GNU General Public License along with     *
 * this program. If not, see <http://www.gnu.org/licenses/>.                                      *
 **************************************************************************************************/

#ifndef __cplusplus
#error "This is a C++ library, you know, so you'd better use a C++ compiler to compile it"
#else

#ifndef __NEURALPP_EXCEPTION
#define __NEURALPP_EXCEPTION

#include <exception>

namespace neuralpp  {
	/**
	 * @class NetworkFileNotFoundException
	 * @brief Exception thrown when doing an attempt to load a network from an invalid file
	 */
	class NetworkFileNotFoundException : public std::exception  {
	public:
		NetworkFileNotFoundException()  {}
		const char* what() const throw() { return "Attempt to load a neural network from an invalid network file"; }
	};

	/**
	 * @class InvalidXMLException
	 * @brief Exception thrown when trying parsing an invalid XML
	 */
	class InvalidXMLException : public std::exception  {
	public:
		InvalidXMLException()  {}
		const char* what() const throw() { return "Attempt to load an invalid XML file"; }
	};
}

#endif
#endif

