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

namespace neuralpp {
	Layer::Layer(size_t sz, double (*a) (double)) {
		for (size_t i = 0; i < sz; i++) {
			Neuron n(a);
			 elements.push_back(n);
		}
		
		actv_f = a;
	}

	Layer::Layer(vector < Neuron > &el, double (*a) (double)) {
		elements = el;
		actv_f = a;
	}

	size_t Layer::size() const  {
		return elements.size();
	}

	Neuron & Layer::operator[](size_t i) throw(NetworkIndexOutOfBoundsException)  {
		if (i > size())
			throw NetworkIndexOutOfBoundsException();

		return elements[i];
	}

	void Layer::link(Layer & l) {
		srand((unsigned) time(NULL));

		for (size_t i = 0; i < l.size(); i++) {
			Neuron *n1 = &(l.elements[i]);

			for (size_t j = 0; j < size(); j++) {
				Neuron *n2 = &(elements[j]);
				Synapsis s(n1, n2, RAND, actv_f);

				n1->push_out(s);
				n2->push_in(s);
			}
		}
	}

	void Layer::setProp(vector < double >&v) {
		for (size_t i = 0; i < size(); i++)
			elements[i].setProp(v[i]);
	}

	void Layer::setActv(vector < double >&v) {
		for (size_t i = 0; i < size(); i++)
			elements[i].setActv(v[i]);
	}

	void Layer::propagate() {
		for (size_t i = 0; i < size(); i++) {
			Neuron *n = &(elements[i]);

			n->setProp(n->propagate());
			n->setActv(actv_f(n->getProp()));
		}
	}
}

