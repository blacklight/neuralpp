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

#include <fstream>
#include <sstream>

#include "neural++.hpp"
#include "Markup.h"

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::stringstream;

namespace neuralpp {
	double __actv(double prop)  {
		return prop;
	}

	double df (double (*f)(double), double x)   {
		double h = 0.000001;
		return (f(x+h) - f(x)) / h;
	}

	NeuralNet::NeuralNet(size_t in_size, size_t hidden_size,
			     size_t out_size, double l, int e, double th, double (*a)(double)) {

		epochs = e;
		ref_epochs = epochs;
		l_rate = l;
		actv_f = a;
		threshold = th;

		input = new Layer(in_size, __actv, th);
		hidden = new Layer(hidden_size, __actv, th);
		output = new Layer(out_size, __actv, th);
		link();
	}

	/*NeuralNet::NeuralNet(size_t in_size, size_t hidden_size,
			     size_t out_size, double (*a) (double),
			     double l, int e, double th) {

		epochs = e;
		ref_epochs = epochs;
		l_rate = l;
		actv_f = a;
		threshold = th;

		input = new Layer(in_size, a, th);
		hidden = new Layer(hidden_size, a, th);
		output = new Layer(out_size, a, th);
		link();
	}*/

	double NeuralNet::getOutput() const  {
		return (*output)[0].getActv();
	}

	vector<double> NeuralNet::getOutputs() {
		vector<double> v;

		for (size_t i = 0; i < output->size(); i++)
			v.push_back((*output)[i].getActv());
		return v;
	}

	double NeuralNet::error(double expected)  {
		double err = 0.0;
		vector<double> out = getOutputs();

		for (size_t i=0; i < output->size(); i++)
			err += 0.5*(out[i] - expect[i]) * (out[i] - expect[i]);

		return err;
	}

	void NeuralNet::propagate() {
		hidden->propagate();
		output->propagate();
	}

	void NeuralNet::setInput(vector<double> v) {
		input->setInput(v);
	}

	void NeuralNet::link() {
		hidden->link(*input);
		output->link(*hidden);
	}

	void NeuralNet::setExpected(double e) {
		expect.clear();
		expect.push_back(e);
	}

	void NeuralNet::setExpected(vector<double> e) {
		expect.clear();
		expect.assign(e.begin(), e.end());
	}

	double NeuralNet::expected() const  {
		return expect[0];
	}

	vector<double> NeuralNet::getExpected() const  {
		return expect;
	}

	void NeuralNet::updateWeights() {
		double Dk = 0.0;
		size_t k = output->size();

		for (size_t i = 0; i < k; i++) {
			Neuron *n = &(*output)[i];
			double out_delta = 0.0,
				  z = n->getActv(),
				  d = expect[i],
				  f = df(actv_f, n->getProp());
	
			for (size_t j = 0; j < n->nIn(); j++) {
				Synapsis *s = &(n->synIn(j));
				double y = s->getIn()->getActv(),
					  beta = s->momentum(ref_epochs, ref_epochs - epochs);

				if (ref_epochs - epochs > 0)
					out_delta =
					    (-l_rate) * (z-d) * f * y +
					    beta * s->getPrevDelta();
				else
					out_delta =
					    (-l_rate) * (z-d) * f * y;

				Dk += ( (z-d) * f * s->getWeight() );
				s->setDelta(out_delta);
			}
		}

		for (size_t i = 0; i < hidden->size(); i++) {
			Neuron *n = &(*hidden)[i];
			double hidden_delta = 0.0,
				  d = df(actv_f, n->getProp()) * Dk;

			for (size_t j = 0; j < n->nIn(); j++) {
				Synapsis *s = &(n->synIn(j));
				double x = s->getIn()->getActv(),
					  beta = s->momentum(ref_epochs, ref_epochs - epochs);

				if (ref_epochs - epochs > 0)
					hidden_delta =
						(-l_rate) * d * x +
						beta * s->getPrevDelta();
				else
					hidden_delta =
						(-l_rate) * d * x;

				s->setDelta(hidden_delta);
			}
		}
	}

	void NeuralNet::commitChanges(Layer& l) {
		for (size_t i = 0; i < l.size(); i++) {
			Neuron *n = &(l[i]);

			for (size_t j = 0; j < n->nIn(); j++) {
				Synapsis *s = &(n->synIn(j));
				s->setWeight(s->getWeight() +
					     s->getDelta());
				s->setDelta(0.0);
			}
		}
	}

	void NeuralNet::update() {
		epochs = ref_epochs;

		while ((epochs--) > 0) {
			updateWeights();
			commitChanges(*output);
			commitChanges(*hidden);
			propagate();
		}
	}

	void NeuralNet::save (const char *fname) throw(NetworkFileWriteException)  {
		struct netrecord record;
		ofstream out(fname);

		if (!out)
			throw NetworkFileWriteException();

		record.input_size = input->size();
		record.hidden_size = hidden->size();
		record.output_size = output->size();

		record.epochs = ref_epochs;
		record.l_rate = l_rate;
		record.ex = expect[0];

		if (out.write((char*) &record, sizeof(struct netrecord)) <= 0)
			throw NetworkFileWriteException();

		// Saving neurons' state
		for (unsigned int i = 0; i < input->size(); i++) {
			struct neuronrecord r;
			r.prop = (*input)[i].getProp();
			r.actv = (*input)[i].getActv();

			if (out.write((char*) &r, sizeof(struct neuronrecord)) <= 0)
				throw NetworkFileWriteException();
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			struct neuronrecord r;
			r.prop = (*hidden)[i].getProp();
			r.actv = (*hidden)[i].getActv();
			
			if (out.write((char*) &r, sizeof(struct neuronrecord)) <= 0)
				throw NetworkFileWriteException();
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			struct neuronrecord r;
			r.prop = (*output)[i].getProp();
			r.actv = (*output)[i].getActv();

			if (out.write((char*) &r, sizeof(struct neuronrecord)) <= 0)
				throw NetworkFileWriteException();
		}

		// Saving synapsis' state
		for (unsigned int i = 0; i < input->size(); i++) {
			int nout = (*input)[i].nOut();

			if (out.write((char*) &nout, sizeof(int)) <= 0)
				throw NetworkFileWriteException();

			for (int j = 0; j < nout; j++) {
				struct synrecord r;
				r.w = (*input)[i].synOut(j).getWeight();
				r.d = (*input)[i].synOut(j).getDelta();

				if (out.write((char*) &r, sizeof(struct synrecord)) <= 0)
					throw NetworkFileWriteException();
			}
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			int nin = (*output)[i].nIn();

			if (out.write((char*) &nin, sizeof(int)) <= 0)
				throw NetworkFileWriteException();
			
			for (int j = 0; j < nin; j++) {
				struct synrecord r;
				r.w = (*output)[i].synIn(j).getWeight();
				r.d = (*output)[i].synIn(j).getDelta();

				if (out.write((char*) &r, sizeof(struct synrecord)) <= 0)
					throw NetworkFileWriteException();
			}
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nin = (*hidden)[i].nIn();

			if (out.write((char*) &nin, sizeof(int)) <= 0)
				throw NetworkFileWriteException();

			for (int j = 0; j < nin; j++) {
				struct synrecord r;
				r.w = (*hidden)[i].synIn(j).getWeight();
				r.d = (*hidden)[i].synIn(j).getDelta();

				if (out.write((char*) &r, sizeof(struct synrecord)) <= 0)
					throw NetworkFileWriteException();
			}
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nout = (*hidden)[i].nOut();

			if (out.write((char*) &nout, sizeof(int)) <= 0)
				throw NetworkFileWriteException();

			for (int j = 0; j < nout; j++) {
				struct synrecord r;
				r.w = (*hidden)[i].synOut(j).getWeight();
				r.d = (*hidden)[i].synOut(j).getDelta();

				if (out.write((char*) &r, sizeof(struct synrecord)) <= 0)
					throw NetworkFileWriteException();
			}
		}

		out.close();
	}

	NeuralNet::NeuralNet(const string fname) throw(NetworkFileNotFoundException) {
		struct netrecord record;
		ifstream in(fname.c_str());

		if (!in)
			throw NetworkFileNotFoundException();

		if (in.read((char*) &record, sizeof(struct netrecord)) <= 0)
			throw NetworkFileNotFoundException();

		*this =
		    NeuralNet(record.input_size, record.hidden_size,
			      record.output_size, record.l_rate,
			      record.epochs);

		// Restore neurons
		for (unsigned int i = 0; i < input->size(); i++) {
			struct neuronrecord r;

			if (in.read((char*) &r, sizeof(struct neuronrecord)) <= 0)
				throw NetworkFileNotFoundException();

			(*input)[i].setProp(r.prop);
			(*input)[i].setActv(r.actv);
			(*input)[i].synClear();
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			struct neuronrecord r;
			
			if (in.read((char*) &r, sizeof(struct neuronrecord)) <= 0)
				throw NetworkFileNotFoundException();

			(*hidden)[i].setProp(r.prop);
			(*hidden)[i].setActv(r.actv);
			(*hidden)[i].synClear();
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			struct neuronrecord r;
			
			if (in.read((char*) &r, sizeof(struct neuronrecord)) <= 0)
				throw NetworkFileNotFoundException();

			(*output)[i].setProp(r.prop);
			(*output)[i].setActv(r.actv);
			(*output)[i].synClear();
		}

		for (unsigned int i = 0; i < input->size(); i++)
			(*input)[i].synClear();

		for (unsigned int i = 0; i < hidden->size(); i++)
			(*hidden)[i].synClear();

		for (unsigned int i = 0; i < output->size(); i++)
			(*output)[i].synClear();

		hidden->link(*input);
		output->link(*hidden);

		// Restore synapsis
		for (unsigned int i = 0; i < input->size(); i++) {
			int nout;

			if (in.read((char*) &nout, sizeof(int)) <= 0 )
				throw NetworkFileNotFoundException();
			
			for (int j = 0; j < nout; j++) {
				struct synrecord r;

				if (in.read((char*) &r, sizeof(struct synrecord)) <= 0)
					throw NetworkFileNotFoundException();
				
				(*input)[i].synOut(j).setWeight(r.w);
				(*input)[i].synOut(j).setDelta(r.d);
			}
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			int nin;

			if (in.read((char*) &nin, sizeof(int)) <= 0)
				throw NetworkFileNotFoundException();

			for (int j = 0; j < nin; j++) {
				struct synrecord r;
				
				if (in.read((char*) &r, sizeof(struct synrecord)) <= 0)
					throw NetworkFileNotFoundException();

				(*output)[i].synIn(j).setWeight(r.w);
				(*output)[i].synIn(j).setDelta(r.d);
			}
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nin;
			
			if (in.read((char*) &nin, sizeof(int)) <= 0)
				throw NetworkFileNotFoundException();

			for (int j = 0; j < nin; j++) {
				struct synrecord r;
				
				if (in.read((char*) &r, sizeof(struct synrecord)) <= 0)
					throw NetworkFileNotFoundException();

				(*hidden)[i].synIn(j).setWeight(r.w);
				(*hidden)[i].synIn(j).setDelta(r.d);
			}
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nout;
			
			if (in.read((char*) &nout, sizeof(int)) <= 0)
				throw NetworkFileNotFoundException();

			for (int j = 0; j < nout; j++) {
				struct synrecord r;
				
				if (in.read((char*) &r, sizeof(struct synrecord)) <= 0)
					throw NetworkFileNotFoundException();

				(*hidden)[i].synOut(j).setWeight(r.w);
				(*hidden)[i].synOut(j).setDelta(r.d);
			}
		}

		in.close();
	}

	void NeuralNet::train(string xmlsrc, NeuralNet::source src =
			      file) throw(InvalidXMLException) {
		CMarkup xml;

		if (src == file)
			xml.Load(xmlsrc.c_str());
		else
			xml.SetDoc(xmlsrc.c_str());

		if (!xml.IsWellFormed()) {
			throw InvalidXMLException();
			return;
		}

		if (xml.FindElem("NETWORK")) {
			while (xml.FindChildElem("TRAINING")) {
				vector<double> input;
				vector<double> output;
				xml.IntoElem();

				while (xml.FindChildElem("INPUT")) {
					xml.IntoElem();
					input.push_back(atof(
								xml.GetData().c_str()));

					xml.OutOfElem();
				}
				
				while (xml.FindChildElem("OUTPUT")) {
					xml.IntoElem();
					output.push_back( atof(xml.GetData().c_str()) );
					xml.OutOfElem();
				}

				xml.OutOfElem();

				setInput(input);
				setExpected(output);
				propagate();
				update();
			}
		}
	}

	void NeuralNet::initXML(string& xml) {
		xml.append
		    ("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
		     "<!DOCTYPE NETWORK SYSTEM \"http://blacklight.gotdns.org/prog/neuralpp/trainer.dtd\">\n"
		     "<!-- Automatically generated by Neural++ library - by BlackLight -->\n\n"
		     "<NETWORK>\n");
	}

	vector <double> NeuralNet::split(char delim, string str) {
		char tmp[1024];
		vector <double> v;
		memset(tmp, 0x0, sizeof(tmp));

		for (unsigned int i = 0, j = 0; i <= str.length(); i++) {
			if (str[i] == delim || i == str.length()) {
				v.push_back(atof(tmp));
				memset(tmp, 0x0, sizeof(tmp));
				j = 0;
			} else
				tmp[j++] = str[i];
		}

		return v;
	}

	string NeuralNet::XMLFromSet (int& id, string set) {
		string xml;
		vector<double> in, out;
		stringstream ss (stringstream::in | stringstream::out);

		unsigned int delimPos = -1;
		char delim = ';';

		for (delimPos = 0;
		     delimPos < set.length() && set[delimPos] != delim;
		     delimPos++);

		if (delimPos == set.length())
			return xml;

		string inStr = set.substr(0, delimPos);
		string outStr = set.substr(delimPos + 1, set.length());

		in = split(',', inStr);
		out = split(',', outStr);

		ss << (id++);
		xml += "\t<TRAINING ID=\"" + ss.str() + "\">\n";

		for (unsigned int i = 0; i < in.size(); i++, id++) {
			ss.str(string());
			ss << id;
			xml += "\t\t<INPUT ID=\"" + ss.str() + "\">";

			ss.str(string());
			ss << in[i];
			xml += ss.str() + "</INPUT>\n";
		}

		for (unsigned int i = 0; i < out.size(); i++, id++) {
			ss.str(string());
			ss << id;
			xml += "\t\t<OUTPUT ID=\"" + ss.str() + "\">";

			ss.str(string());
			ss << out[i];
			xml += ss.str() + "</OUTPUT>\n";
		}

		xml += "\t</TRAINING>\n\n";
		return xml;
	}

	void NeuralNet::closeXML(string & xml) {
		xml.append("</NETWORK>\n\n");
	}
}

