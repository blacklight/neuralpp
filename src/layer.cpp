/**************************************************************************************************
 * LibNeural++ v.0.4 - All-purpose library for managing neural networks                           *
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

using std::vector;

namespace neuralpp {
	Layer::Layer(size_t sz, double (*a) (double), double th) {
		for (size_t i = 0; i < sz; i++) {
			Neuron n(a,th);
			elements.push_back(n);
		}
		
		threshold = th;
		actv_f = a;
	}

	Layer::Layer(vector<Neuron> &el, double (*a) (double), double th) {
		elements = el;
		actv_f = a;
		threshold = th;
	}

	size_t Layer::size() const  {
		return elements.size();
	}

	Neuron & Layer::operator[](size_t i) throw(NetworkIndexOutOfBoundsException)  {
		if (i > size())
			throw NetworkIndexOutOfBoundsException();

		return elements[i];
	}

	void Layer::link(Layer& l) {
		srand((unsigned) time(NULL));
		
		for (size_t i = 0; i < l.size(); i++)
			l.elements[i].setSynOut(size());
		
		for (size_t i = 0; i < size(); i++)
			elements[i].setSynIn(l.size());

		for (size_t i = 0; i < l.size(); i++) {
			for (size_t j = 0; j < size(); j++) {
				Synapsis *s = new Synapsis( &(l.elements[i]), &(elements[i]), RAND, actv_f );
				l.elements[i].synOut(j) = *s;
				elements[j].synIn(i) = *s;
			}
		}
	}

	void Layer::setInput (vector<double> v)  {
		for (size_t i = 0; i < size(); i++)  {
			elements[i].setProp(v[i]);
			elements[i].setActv(v[i]);
		}
	}

	void Layer::propagate() {
		for (size_t i = 0; i < size(); i++)
			elements[i].propagate();
	}
}

