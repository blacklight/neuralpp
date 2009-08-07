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

#include "neural++.hpp"
using namespace neuralpp;

/**
 * @brief Constructor
 * @param a Activation function
 * @param d Its derivate
 */
Neuron::Neuron (float (*a)(float), float (*d)(float))  {
	actv_f=a;
	deriv=d;
}

/**
 * @brief Alternative constructor, that gets also the synapsis linked to the neuron
 */
Neuron::Neuron (vector< Synapsis > i, vector< Synapsis > o, float (*a)(float), float(*d)(float))  {
	in=i;
	out=o;

	actv_f=a;
	deriv=d;
}

/**
 * @brief Gets the i-th synapsis connected on the input of the neuron
 */
Synapsis& Neuron::synIn (size_t i)  { return in[i]; }

/**
 * @brief Gets the i-th synapsis connected on the output of the neuron
 */
Synapsis& Neuron::synOut (size_t i)  { return out[i]; }

/**
 * @brief It pushes a new input synapsis
 */
void Neuron::push_in (Synapsis& s)  { in.push_back(s); }

/**
 * @brief It pushes a new output synapsis
 */
void Neuron::push_out (Synapsis& s)  { out.push_back(s); }

/**
 * @brief Change the propagation value of the neuron
 */
void Neuron::setProp (float val)  { prop_val=val; }

/**
 * @brief Change the activation value of the neuron
 */
void Neuron::setActv (float val)  { actv_val=actv_f(val); }

/**
 * @return Number of input synapsis
 */
size_t Neuron::nIn()  { return in.size(); }

/**
 * @return Number of output synapsis
 */
size_t Neuron::nOut()  { return out.size(); }

/**
 * @brief It gets the propagation value of the neuron
 */
float Neuron::getProp()  { return prop_val; }

/**
 * @brief It gets the activation value of the neuron
 */
float Neuron::getActv()  { return actv_val; }

/**
 * @brief Propagate a neuron's activation value to the connected neurons
 */
float Neuron::propagate()  {
	float aux=0;

	for (size_t i=0; i<nIn(); i++)
		aux += (in[i].getWeight() * in[i].getIn()->actv_val);
	return aux;
}

