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

namespace neuralpp {
	Neuron::Neuron(double (*a) (double), double (*d) (double)) {
		actv_f = a;
		deriv = d;
	}
	
	Neuron::Neuron(vector < Synapsis > i, vector < Synapsis > o,
			 double (*a) (double), double (*d) (double)) {

		in = i;
		out = o;

		actv_f = a;
		deriv = d;
	}

	Synapsis & Neuron::synIn(size_t i) {
		return in[i];
	}

	Synapsis & Neuron::synOut(size_t i) {
		return out[i];
	}

	void Neuron::push_in(Synapsis & s) {
		in.push_back(s);
	}

	void Neuron::push_out(Synapsis & s) {
		out.push_back(s);
	}

	void Neuron::setProp(double val) {
		prop_val = val;
	}

	void Neuron::setActv(double val) {
		actv_val = actv_f(val);
	}

	size_t Neuron::nIn() {
		return in.size();
	}

	size_t Neuron::nOut() {
		return out.size();
	}

	double Neuron::getProp() {
		return prop_val;
	}

	double Neuron::getActv() {
		return actv_val;
	}

	double Neuron::propagate() {
		double aux = 0;

		for (size_t i = 0; i < nIn(); i++)
			aux +=
			    (in[i].getWeight() * in[i].getIn()->actv_val);
		return aux;
	}

	void Neuron::synClear()  {
		in.clear();
		out.clear();
	}
}

