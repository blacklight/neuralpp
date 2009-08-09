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

namespace neuralpp  {

Synapsis::Synapsis(Neuron* i, Neuron* o, double w, double d)  {
	in=i; out=o;
	weight=w;
	delta=d; prev_delta=0;
}

Synapsis::Synapsis (Neuron* i, Neuron* o, double(*a)(double), double(*d)(double))  {
	srand((unsigned) time(NULL));

	delta=0;
	prev_delta=0;
	weight=RAND;
	in=i;
	out=o;

	actv_f=a;
	deriv=d;
}

Synapsis::Synapsis (Neuron* i, Neuron* o,
		double w, double(*a)(double), double(*d)(double))  {
	delta=0;
	prev_delta=0;
	weight=w;
	in=i;
	out=o;
	
	actv_f=a;
	deriv=d;
}

Neuron* Synapsis::getIn()  { return in; }

Neuron* Synapsis::getOut()  { return out; }

double Synapsis::getWeight()  { return weight; }

double Synapsis::getDelta()  { return delta; }

double Synapsis::getPrevDelta()  { return prev_delta; }

void Synapsis::setWeight(double w)  { weight=w; }

void Synapsis::setDelta(double d)  {
	prev_delta=delta;
	delta=d;
}

double Synapsis::momentum(int N, int x)  {
	return (BETA0*N)/(20*x + N);
}
}

