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

#ifdef __cplusplus

#ifndef __NEURALPP
#define __NEURALPP

#include <vector>
#include <string>
#include <exception>

#include <cmath>
#include <ctime>
#include <cstdio>
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
		int ref_epochs;
		float l_rate;
		float ex;

		Layer* input;
		Layer* hidden;
		Layer* output;

		/**
		 * @brief It updates the weights of the net's synapsis through back-propagation.
		 *   In-class use only
		 */
		void updateWeights();

		/**
		 * @brief It commits the changes made by updateWeights() to the layer l.
		 *   In-class use only
		 * @param l Layer to commit the changes
		 */
		void commitChanges (Layer *l);

		/**
		 * @brief It get the error made on the expected result as |v-v'|/v
		 * @param Expected value
		 * @return Mean error
		 */
		float error(float);
		
		float (*actv_f)(float);
		float (*deriv)(float);

	public:
		/**
		 * @brief Enum to choose the eventual training source for our network (XML from a file or from a string)
		 */
		typedef enum  { file, str } source;

		NeuralNet()  {}

		/**
		 * @brief Constructor
		 * @param in_size Size of the input layer
		 * @param hidden_size Size of the hidden layer
		 * @param out_size Size of the output layer
		 * @param l learn rate (get it after doing some experiments, but generally try to
		 *   keep its value quite low to be more accurate)
		 * @param e Epochs (cycles) to execute (the most you execute, the most the network
		 *   can be accurate for its purpose)
		 */
		NeuralNet (size_t, size_t, size_t, float, int);

		/**
		 * @brief Constructor
		 * @param file Binary file containing a neural network previously saved by save() method
		 * @throw NetworkFileNotFoundException
		 */
		NeuralNet (const char*) throw();
		
		
		/**
		 * @brief Constructor
		 * @param in_size Size of the input layer
		 * @param hidden_size Size of the hidden layer
		 * @param out_size Size of the output layer
		 * @param actv Activation function to use (default: f(x)=x)
		 * @param deriv Derivate for the activation function to use (default: f'(x)=1)
		 * @param l learn rate (get it after doing some experiments, but generally try to
		 *   keep its value quite low to be more accurate)
		 * @param e Epochs (cycles) to execute (the most you execute, the most the network
		 *   can be accurate for its purpose)
		 */
		NeuralNet (size_t, size_t, size_t, float(*)(float), float(*)(float), float, int);

		/**
		 * @brief It gets the output of the network (note: the layer output should contain
		 *   an only neuron)
		 */
		float getOutput();

		/**
		 * @brief It gets the output of the network in case the output layer contains more neurons
		 */
		vector<float> getVectorOutput();

		/**
		 * @brief It gets the value expected. Of course you should specify this when you
		 *   build your network by using setExpected.
		 */
		float expected();

		/**
		 * @brief It sets the value you expect from your network
		 */
		void setExpected(float);

		/**
		 * @brief It updates through back-propagation the weights of the synapsis and
		 *  computes again the output value for <i>epochs</i> times, calling back
		 *  updateWeights and commitChanges functions
		 */
		void update();

		/**
		 * @brief It propagates values through the network. Use this when you want to give
		 *  an already trained network some new values the get to the output
		 */
		void propagate();

		/**
		 * @brief It sets the input for the network
		 * @param v Vector of floats, containing the values to give to your network
		 */
		void setInput (vector<float>&);

		/**
		 * @brief It links the layers of the network (input, hidden, output). Don't use unless
		 *  you exactly know what you're doing, it is already called by the constructor
		 */
		void link();

		/**
		 * @brief Save a trained neural network to a binary file
		 */
		bool save(const char*);

		/**
		 * @brief Train a network using a training set loaded from an XML file. A sample XML file
		 *   is available in examples/adder.xml
		 * @param xml XML file containing our training set
		 * @param src Source type from which the XML will be loaded (from a file [default] or from a string)
		 * @throw InvalidXMLException
		 */
		void train(string, source) throw();

		/**
		 * @brief Initialize the training XML for the neural network
		 * @param xml String that will contain the XML
		 */
		static void initXML (string&);

		/**
		 * @brief Splits a string into a vector of floats, given a delimitator
		 * @param delim Delimitator
		 * @param str String to be splitted
		 * @return Vector of floats containing splitted values
		 */
		static vector<float> split (char, string);

		/**
		 * @brief Get a training set from a string and copies it to an XML
		 *   For example, these strings could be training sets for making sums:
		 *   "2,3;5" - "5,6;11" - "2,2;4" - "4,5:9"
		 *   This method called on the first string will return an XML such this:
		 *   '&lt;training id="0"&gt;&lt;input id="0"&gt;2&lt;/input&gt;&lt;input id="1"&gt;3&lt;/input&gt;&lt;output id="0"&gt;5&lt;/output&gt;
		 *   &lt/training&gt;'
		 *
		 * @param id ID for the given training set (0,1,..,n)
		 * @param set String containing input values and expected outputs
		 * @return XML string
		 */
		static string XMLFromSet (int, string);

		/**
		 * @brief Closes an open XML document generated by "initXML" and "XMLFromSet"
		 * @param XML string to be closed
		 */
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
		Synapsis(Neuron* i, Neuron* o, float w, float d)  {
			in=i; out=o;
			weight=w; delta=d;
		}

		/**
		 * @brief Constructor
		 * @param i Input neuron
		 * @param o Output neuron
		 * @param a Activation function
		 * @param d Derivate for activation function
		 */
		Synapsis (Neuron* i, Neuron* o, float(*)(float), float(*)(float));
		
		/**
		 * @brief Constructor
		 * @param i Input neuron
		 * @param o Output neuron
		 * @param w Weight for the synapsis (default: random)
		 * @param a Activation function
		 * @param d Derivate for activation function
		 */
		Synapsis (Neuron* i, Neuron* o, float w, float(*)(float), float(*)(float));
	
		/**
		 * @return Reference to input neuron of the synapsis
		 */
		Neuron* getIn();
		
		/**
		 * @return Reference to output neuron of the synapsis
		 */
		Neuron* getOut();

		/**
		 * @brief It sets the weight of the synapsis
		 */
		void setWeight(float);
		
		/**
		 * @brief It sets the delta (how much to change the weight after an update) 
		 *  of the synapsis
		 */
		void setDelta(float);

		/**
		 * @return Weight of the synapsis
		 */
		float getWeight();
		
		/**
		 * @return Delta of the synapsis
		 */
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
		/**
		 * @brief Constructor
		 * @param a Activation function
		 * @param d Its derivate
		 */
		Neuron (float (*)(float), float(*)(float));

		/**
		 * @brief Alternative constructor, that gets also the synapsis linked to the neuron
		 */
		Neuron (vector< Synapsis >, vector< Synapsis >, float (*)(float), float(*)(float));

		/**
		 * @brief Gets the i-th synapsis connected on the input of the neuron
		 */
		Synapsis& synIn (size_t i);
		
		/**
		 * @brief Gets the i-th synapsis connected on the output of the neuron
		 */
		Synapsis& synOut (size_t i);

		/**
		 * @brief It pushes a new input synapsis
		 */
		void push_in (Synapsis&);
		
		/**
		 * @brief It pushes a new output synapsis
		 */
		void push_out (Synapsis&);

		/**
		 * @brief Change the activation value of the neuron
		 */
		void setActv (float);

		/**
		 * @brief Change the propagation value of the neuron
		 */
		void setProp (float);

		/**
		 * @brief It gets the activation value of the neuron
		 */
		float getActv();
		
		/**
		 * @brief It gets the propagation value of the neuron
		 */
		float getProp();

		/**
		 * @brief It propagates its activation value to the connected neurons
		 */
		float propagate();

		/**
		 * @return Number of input synapsis
		 */
		size_t nIn();
		
		/**
		 * @return Number of output synapsis
		 */
		size_t nOut();

		/**
		 * @brief Remove input and output synapsis from a neuron
		 */
		void synClear()  { in.clear(); out.clear(); }
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
		/**
		 * @brief Constructor
		 * @param sz Size of the layer
		 * @param a Activation function
		 * @param d Its derivate
		 */
		Layer (size_t sz, float (*)(float), float(*)(float));

		/**
		 * @brief Alternative constructor. It directly gets a vector of neurons to build
		 *  the layer
		 */
		Layer (vector< Neuron >&, float(*)(float), float(*)(float));

		/**
		 * @brief Redefinition for operator []. It gets the neuron at <i>i</i>
		 */
		Neuron& operator[] (size_t);

		/**
		 * @brief It links a layer to another
		 * @param l Layer to connect to the current as input layer
		 */
		void link (Layer&);

		/**
		 * @brief It sets a vector of propagation values to all its neurons
		 * @param v Vector of values to write as propagation values
		 */
		void setProp (vector<float>&);
		 
		/** 
		 * @brief It sets a vector of activation values to all its neurons
		 * @param v Vector of values to write as activation values
		 */
		void setActv (vector<float>&);

		/**
		 * @brief It propagates its activation values to the output layers
		 */
		void propagate();

		/**
		 * @return Number of neurons in the layer
		 */
		size_t size();
	};

	struct netrecord  {
		int input_size;
		int hidden_size;
		int output_size;

		int epochs;
		float l_rate;
		float ex;
	};

	struct neuronrecord  {
		float prop;
		float actv;
	};

	struct synrecord  {
		float w;
		float d;
	};
}

#endif
#endif

