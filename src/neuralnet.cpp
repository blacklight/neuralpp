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

		input = new Layer(in_size, a, th);
		hidden = new Layer(hidden_size, a, th);
		output = new Layer(out_size, a, th);
		link();
	}

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
				(*hidden)[j].synOut(i).setDelta(out_delta);
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
				(*input)[j].synOut(i).setDelta(hidden_delta);
			}
		}

		for (size_t i = 0; i < output->size(); i++) {
			Neuron *n = &((*output)[i]);

			for (size_t j = 0; j < n->nIn(); j++) {
				Synapsis *s = &(n->synIn(j));
				s->setWeight(s->getWeight() +
					     s->getDelta());
				s->setDelta(0.0);
			}
		}

		for (size_t i = 0; i < hidden->size(); i++) {
			Neuron *n = &((*hidden)[i]);

			for (size_t j = 0; j < n->nOut(); j++) {
				Synapsis *s = &(n->synOut(j));
				s->setWeight(s->getWeight() +
					     s->getDelta());
				s->setDelta(0.0);
			}
		}

		for (size_t i = 0; i < hidden->size(); i++) {
			Neuron *n = &((*hidden)[i]);

			for (size_t j = 0; j < n->nIn(); j++) {
				Synapsis *s = &(n->synIn(j));
				s->setWeight(s->getWeight() +
					     s->getDelta());
				s->setDelta(0.0);
			}
		}

		for (size_t i = 0; i < input->size(); i++) {
			Neuron *n = &((*input)[i]);

			for (size_t j = 0; j < n->nOut(); j++) {
				Synapsis *s = &(n->synOut(j));
				s->setWeight(s->getWeight() +
					     s->getDelta());
				s->setDelta(0.0);
			}
		}
	}

	void NeuralNet::update() {
		epochs = ref_epochs;

		while ((epochs--) > 0) {
			propagate();
			updateWeights();
		}
	}

	void NeuralNet::save (const char *fname) throw(NetworkFileWriteException)  {
		ofstream out(fname);
		stringstream xml(stringstream::in | stringstream::out);

		if (!out)
			throw NetworkFileWriteException();

		xml << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
			<< "<!DOCTYPE NETWORK SYSTEM \"http://blacklight.gotdns.org/prog/neuralpp/network.dtd\">\n"
			<< "<!-- Automatically generated by BlackLight's Neural++ module -->\n\n"
			<< "<network name=\"Put here the name for this neural network\" epochs=\"" << ref_epochs << "\" "
			<< "learning_rate=\"" << l_rate << "\" threshold=\"" << threshold << "\">\n"
			<< "\t<layer class=\"input\"  size=\"" << input->size()  << "\"></layer>\n"
			<< "\t<layer class=\"hidden\" size=\"" << hidden->size() << "\"></layer>\n"
			<< "\t<layer class=\"output\" size=\"" << output->size() << "\"></layer>\n\n";

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nin = (*hidden)[i].nIn();

			for (int j = 0; j < nin; j++)
				xml << "\t<synapsis class=\"inhid\" input=\"" << j << "\" output=\"" << i << "\" "
					<< "weight=\"" << (*hidden)[i].synIn(j).getWeight() << "\"></synapsis>\n";
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			int nin = (*output)[i].nIn();

			for (int j = 0; j < nin; j++)
				xml << "\t<synapsis class=\"hidout\" input=\"" << j << "\" output=\"" << i << "\" "
					<< "weight=\"" << (*output)[i].synIn(j).getWeight() << "\"></synapsis>\n";
		}

		xml << "</network>\n";
		out << xml.str();
	}

	NeuralNet::NeuralNet(const string fname) throw(NetworkFileNotFoundException)  {
		unsigned int in_size = 0, hid_size = 0, out_size = 0;
		vector< vector<double> > in_hid_synapses, hid_out_synapses;

		CMarkup xml;
		xml.Load(fname.c_str());

		if (!xml.IsWellFormed()) {
			throw InvalidXMLException();
			return;
		}

		if (xml.FindElem("network")) {
			if (xml.GetAttrib("epochs").empty())
				throw InvalidXMLException();
			
			if (xml.GetAttrib("learning_rate").empty())
				throw InvalidXMLException();

			epochs = atoi(xml.GetAttrib("epochs").c_str());
			l_rate = atof(xml.GetAttrib("learning_rate").c_str());
			threshold = 0.0;
			
			if (!xml.GetAttrib("threshold").empty())
				threshold = atof(xml.GetAttrib("threshold").c_str());

			while (xml.FindChildElem("layer"))  {
				if (xml.GetChildAttrib("class").empty())
					throw InvalidXMLException();
			
				if (xml.GetChildAttrib("size").empty())
					throw InvalidXMLException();

				if (!xml.GetChildAttrib("class").compare("input"))
					in_size = atoi(xml.GetChildAttrib("size").c_str());
				else if (!xml.GetChildAttrib("class").compare("hidden"))
					hid_size = atoi(xml.GetChildAttrib("size").c_str());
				else if (!xml.GetChildAttrib("class").compare("output"))
					out_size = atoi(xml.GetChildAttrib("size").c_str());
				else
					throw InvalidXMLException();
			}

			if (in_size && hid_size && out_size)  {
				in_hid_synapses = vector< vector<double> >(in_size);

				for (unsigned int i=0; i < in_size; i++)
					in_hid_synapses[i] = vector<double>(hid_size);

				hid_out_synapses = vector< vector<double> >(hid_size);

				for (unsigned int i=0; i < hid_size; i++)
					hid_out_synapses[i] = vector<double>(out_size);
			}
			
			while (xml.FindChildElem("synapsis"))  {
				if (!(in_size && hid_size && out_size))
					throw InvalidXMLException();

				if (xml.GetChildAttrib("class").empty())
					throw InvalidXMLException();
			
				if (xml.GetChildAttrib("input").empty())
					throw InvalidXMLException();
				
				if (xml.GetChildAttrib("output").empty())
					throw InvalidXMLException();
				
				if (xml.GetChildAttrib("weight").empty())
					throw InvalidXMLException();

				unsigned int in  = atoi(xml.GetChildAttrib("input").c_str());
				unsigned int out = atoi(xml.GetChildAttrib("output").c_str());

				if (xml.GetChildAttrib("class") == "inhid")  {
					if (in >= in_size || out >= hid_size)
						throw InvalidXMLException();

					in_hid_synapses[in][out] = atof(xml.GetChildAttrib("weight").c_str());
				}

				if (xml.GetChildAttrib("class") == "hidout")  {
					if (in >= hid_size || out >= out_size)
						throw InvalidXMLException();

					hid_out_synapses[in][out] = atof(xml.GetChildAttrib("weight").c_str());
				}
			}
		}

		*this = NeuralNet(in_size, hid_size, out_size, l_rate, epochs, threshold);

		hidden->link(*input);
		output->link(*hidden);

		// Restore synapses
		for (unsigned int i = 0; i < input->size(); i++) {
			for (unsigned int j = 0; j < hidden->size(); j++)
				(*input)[i].synOut(j).setWeight( in_hid_synapses[i][j] );
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			for (unsigned int j = 0; j < hidden->size(); j++)
				(*output)[i].synIn(j).setWeight( (hid_out_synapses[j][i]) );
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			for (unsigned int j = 0; j < input->size(); j++)
				(*hidden)[i].synIn(j).setWeight( (in_hid_synapses[j][i]) );
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			for (unsigned int j = 0; j < output->size(); j++)
				(*hidden)[i].synOut(j).setWeight( hid_out_synapses[i][j] );
		}
	}

	void NeuralNet::saveToBinary (const char *fname) throw(NetworkFileWriteException)  {
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

	void NeuralNet::loadFromBinary (const string fname) throw(NetworkFileNotFoundException) {
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

