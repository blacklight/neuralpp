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
	 * @class NetworkFileWriteException
	 * @brief Exception thrown when trying to write the network's information to a file that cannot
	 * be written
	 */
	class NetworkFileWriteException : public std::exception  {
	public:
		NetworkFileWriteException()  {}
		const char* what() const throw()  { return "There was an error while writing the network file"; }
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

	/**
	 * @class NetworkIndexOutOfBoundsException
	 * @brief Exception raised when trying to access a neuron whose index is larger than the number
	 * of neurons in the network
	 */
	class NetworkIndexOutOfBoundsException : public std::exception  {
	public:
		NetworkIndexOutOfBoundsException()  {}
		const char* what() const throw()  { return "Attempt to access a non-existing neuron"; }
	};

	/**
	 * @class InvalidSynapticalWeightException
	 * @brief Exception raised when, while trying the network or directly, the weight of a synapsis is
	 * set to a value |w| > 1
	 */
	class InvalidSynapticalWeightException : public std::exception  {
	public:
		InvalidSynapticalWeightException()  {}
		const char* what() const throw()  { return "Attempt to set an invalid weight for the synapsis"; }
	};
}

#endif
#endif

