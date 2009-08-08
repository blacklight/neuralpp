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

#include <cstdlib>
#include "neural++.hpp"
using namespace neuralpp;

/**
 * @brief Constructor
 * @param i Input neuron
 * @param o Output neuron
 * @param a Activation function
 * @param d Derivate for activation function
 */
Synapsis::Synapsis (Neuron* i, Neuron* o, double(*a)(double), double(*d)(double))  {
	srand((unsigned) time(NULL));

	delta=0;
	weight=RAND;
	in=i;
	out=o;

	actv_f=a;
	deriv=d;
}

/**
 * @brief Constructor
 * @param i Input neuron
 * @param o Output neuron
 * @param w Weight for the synapsis (default: random)
 * @param a Activation function
 * @param d Derivate for activation function
 */
Synapsis::Synapsis (Neuron* i, Neuron* o, double w, double(*a)(double), double(*d)(double))  {
	delta=0;
	weight=w;
	in=i;
	out=o;
	
	actv_f=a;
	deriv=d;
}

/**
 * @return Reference to input neuron of the synapsis
 */
Neuron* Synapsis::getIn()  { return in; }

/**
 * @return Reference to output neuron of the synapsis
 */
Neuron* Synapsis::getOut()  { return out; }

/**
 * @return Weight of the synapsis
 */
double Synapsis::getWeight()  { return weight; }

/**
 * @return Delta of the synapsis
 */
double Synapsis::getDelta()  { return delta; }

/**
 * @brief It sets the weight of the synapsis
 */
void Synapsis::setWeight(double w)  { weight=w; }

/**
 * @brief It sets the delta (how much to change the weight after an update) 
 *  of the synapsis
 */
void Synapsis::setDelta(double d)  { delta=d; }

