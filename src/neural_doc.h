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

#ifndef __NEURALPP
#define __NEURALPP

#include <vector>
#include <cmath>
#include <ctime>
using namespace std;

namespace neuralpp  {
	//! Default rand value: |sin(rand)|, always >= 0 and <= 1
	#define	RAND		( (float) abs( sinf((float) rand()) ) )
	
	class Synapsis;
	class Neuron;
	class Layer;
	class NeuralNet;
	class NetworkFileNotFoundException;
	class InvalidXMLException;

	/**
	 * @class NetworkFileNotFoundException
	 * @brief Exception thrown when doing an attempt to load a network from an invalid file
	 */
	class NetworkFileNotFoundException : public exception  {
	public:
		NetworkFileNotFoundException()  {}
		const char* what() const throw() { return strdup("Attempt to load a neural network from an invalid network file\n"); }
	};
	
	/**
	 * @class InvalidXMLException
	 * @brief Exception thrown when trying parsing an invalid XML
	 */
	class InvalidXMLException : public exception  {
	public:
		InvalidXMLException()  {}
		const char* what() const throw() { return strdup("Attempt to load an invalid XML file\n"); }
	};

	/**
	 * @class NeuralNet
	 * @brief Main project's class. Use *ONLY* this class, unless you know what you're doing
	 */
	class NeuralNet  {
		int epochs;
		float l_rate;
		float ex;

		Layer* input;
		Layer* hidden;
		Layer* output;

		void updateWeights();
		void commitChanges (Layer *l);
		float error(float);
		
		float (*actv_f)(float);
		float (*deriv)(float);

	public:
		/**
		 * @brief Enum to choose the eventual training source for our network (XML from a file or from a string)
		 */
		typedef enum  { file, str } source;

		NeuralNet (size_t, size_t, size_t, float, int);
		NeuralNet (size_t, size_t, size_t, float(*)(float), float(*)(float), float, int);
		NeuralNet (const char*) throw();

		float getOutput();
		float expected();
		
		vector<float> getVectorOutput();

		void setExpected(float);
		void update();
		void propagate();
		void setInput (vector<float>&);
		void link();
		bool save (const char*);
		void train(string, source) throw();

		static vector<float> split (char, string);
		static void initXML (string&);
		static string XMLFromSet (int, string);
		static void closeXML(string&);
	};

	/**
	 * @class Synapsis
	 * @brief Class for managing synapsis. Don't use this class directly unless you know what
	 *  you're doing, use NeuralNet instead
	 */
	class Synapsis  {
		float delta;
		float weight;

		Neuron *in;
		Neuron *out;
		
		float (*actv_f)(float);
		float (*deriv)(float);

	public:
		Synapsis (Neuron* i, Neuron* o, float(*)(float), float(*)(float));
		Synapsis (Neuron* i, Neuron* o, float w, float(*)(float), float(*)(float));
	
		Neuron* getIn();
		Neuron* getOut();

		void setWeight(float);
		void setDelta(float);

		float getWeight();
		float getDelta();
	};

	/**
	 * @class Neuron
	 * @brief Class for managing neurons. Don't use this class directly unless you know what
	 *  you're doing, use NeuralNet instead
	 */
	class Neuron  {
		float actv_val;
		float prop_val;

		vector< Synapsis > in;
		vector< Synapsis > out;

		float (*actv_f)(float);
		float (*deriv)(float);
	public:
		Neuron (float (*)(float), float(*)(float));
		Neuron (vector< Synapsis >, vector< Synapsis >, float (*)(float), float(*)(float));

		Synapsis& synIn (size_t i);
		Synapsis& synOut (size_t i);

		void push_in (Synapsis&);
		void push_out (Synapsis&);

		void setActv (float);
		void setProp (float);

		float getActv();
		float getProp();
		float propagate();

		
		size_t nIn();
		size_t nOut();
	};

	/**
	 * @class Layer
	 * @brief Class for managing layers of neurons. Don't use this class directly unless you know what
	 *  you're doing, use NeuralNet instead
	 */
	class Layer  {
		vector< Neuron > elements;
		void (*update_weights)();

		float (*actv_f)(float);
		float (*deriv)(float);
	public:
		Layer (size_t sz, float (*)(float), float(*)(float));
		Layer (vector< Neuron >&, float(*)(float), float(*)(float));

		Neuron& operator[] (size_t);

		void link (Layer&);
		void setProp (vector<float>&);
		void setActv (vector<float>&);
		void propagate();

		size_t size();
	};
}

#endif

