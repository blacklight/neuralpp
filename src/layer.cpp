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

#include "neural++.h"
using namespace neuralpp;

/**
 * @brief Constructor
 * @param sz Size of the layer
 * @param a Activation function
 * @param d Its derivate
 */
Layer::Layer (size_t sz, float(*a)(float), float(*d)(float))  {
	for (size_t i=0; i<sz; i++)  {
		Neuron n(a,d);
		elements.push_back(n);
	}

	actv_f=a;
	deriv=d;
}

/**
 * @brief Alternative constructor. It directly gets a vector of neurons to build
 *  the layer
 */
Layer::Layer (vector< Neuron > &el, float (*a)(float), float(*d)(float))  {
	elements=el;
	actv_f=a;
	deriv=d;
}

/**
 * @return Number of neurons in the layer
 */
size_t Layer::size()  { return elements.size(); }

/**
 * @brief Redefinition for operator []. It gets the neuron at <i>i</i>
 */
Neuron& Layer::operator[] (size_t i)  { return elements[i]; }

/**
 * @brief It links a layer to another
 * @param l Layer to connect to the current as input layer
 */
void Layer::link (Layer& l)  {
	srand ((unsigned) time(NULL));

	for (size_t i=0; i<l.size(); i++)  {
		Neuron *n1 = &(l.elements[i]);

		for (size_t j=0; j<size(); j++)  {
			Neuron *n2 = &(elements[j]);
			Synapsis s(n1,n2,RAND,actv_f,deriv);

			n1->push_out(s);
			n2->push_in(s);
		}
	}
}

/**
 * @brief It sets a vector of propagation values to all its neurons
 * @param v Vector of values to write as propagation values
 */
void Layer::setProp (vector<float> &v)  {
	for (size_t i=0; i<size(); i++)
		elements[i].setProp(v[i]);
}

/** 
 * @brief It sets a vector of activation values to all its neurons
 * @param v Vector of values to write as activation values
 */
void Layer::setActv (vector<float> &v)  {
	for (size_t i=0; i<size(); i++)
		elements[i].setActv(v[i]);
}

/**
 * @brief It propagates its activation values to the output layers
 */
void Layer::propagate()  {
	for (size_t i=0; i<size(); i++)  {
		Neuron *n = &(elements[i]);

		n->setProp(n->propagate());
		n->setActv( actv_f(n->getProp()) );
	}
}

