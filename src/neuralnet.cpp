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

#include <sstream>
#include "neural++.hpp"
#include "Markup.h"

namespace neuralpp {
	double __actv(double prop) {
		return prop;
	}
	
	double __deriv(double prop) {
		return 1;
	}

	NeuralNet::NeuralNet(size_t in_size, size_t hidden_size,
			     size_t out_size, double l, int e) {

		epochs = e;
		ref_epochs = epochs;
		l_rate = l;
		actv_f = __actv;
		deriv = __deriv;

		input = new Layer(in_size, __actv, __deriv);
		hidden = new Layer(hidden_size, __actv, __deriv);
		output = new Layer(out_size, __actv, __deriv);
		link();
	}

	NeuralNet::NeuralNet(size_t in_size, size_t hidden_size,
			     size_t out_size, double (*a) (double),
			     double (*d) (double), double l, int e) {

		epochs = e;
		ref_epochs = epochs;
		l_rate = l;

		actv_f = a;
		deriv = d;

		input = new Layer(in_size, a, d);
		hidden = new Layer(hidden_size, a, d);
		output = new Layer(out_size, a, d);
		link();
	}

	double NeuralNet::getOutput() {
		return (*output)[0].getActv();
	}

	vector<double> NeuralNet::getOutputs() {
		vector<double> v;

		for (size_t i = 0; i < output->size(); i++)
			v.push_back((*output)[i].getActv());
		return v;
	}

	double NeuralNet::error(double expected) {
		return abs((getOutput() - expected *
			    deriv(getOutput())) / (abs(expected)));
	}

	void NeuralNet::propagate() {
		hidden->propagate();
		output->propagate();
	}

	void NeuralNet::setInput(vector <double>& v) {
		input->setProp(v);
		input->setActv(v);
	}

	void NeuralNet::link() {
		hidden->link(*input);
		output->link(*hidden);
	}

	void NeuralNet::setExpected(double e) {
		ex = e;
	}

	double NeuralNet::expected() {
		return ex;
	}

	void NeuralNet::updateWeights() {
		double out_delta;

		for (size_t i = 0; i < output->size(); i++) {
			Neuron *n = &(*output)[i];

			for (size_t j = 0; j < n->nIn(); j++) {
				Synapsis *s = &(n->synIn(j));

				if (ref_epochs - epochs > 0)
					out_delta =
					    s->getIn()->getActv() *
					    error(ex) * (-l_rate) +
					    s->momentum(ref_epochs,
							ref_epochs -
							epochs) *
					    s->getPrevDelta();
				else
					out_delta =
					    s->getIn()->getActv() *
					    error(ex) * (-l_rate);

				s->setDelta(out_delta);
			}
		}

		for (size_t i = 0; i < hidden->size(); i++) {
			Neuron *n = &(*hidden)[i];
			double d =
			    deriv(n->getProp()) *
			    n->synOut(0).getWeight() * out_delta;

			for (size_t j = 0; j < n->nIn(); j++) {
				Synapsis *s = &(n->synIn(j));

				if (ref_epochs - epochs > 0)
					s->setDelta((-l_rate) * d *
						    s->getIn()->getActv() +
						    s->momentum(ref_epochs,
								ref_epochs
								-
								epochs) *
						    s->getPrevDelta());
				else
					s->setDelta((-l_rate) * d *
						    s->getIn()->getActv());
			}
		}
	}

	void NeuralNet::commitChanges(Layer * l) {
		for (size_t i = 0; i < l->size(); i++) {
			Neuron *n = &(*l)[i];

			for (size_t j = 0; j < n->nIn(); j++) {
				Synapsis *s = &(n->synIn(j));
				s->setWeight(s->getWeight() +
					     s->getDelta());
				s->setDelta(0);
			}
		}
	}

	void NeuralNet::update() {
		while ((epochs--) > 0) {
			updateWeights();
			commitChanges(output);
			commitChanges(hidden);
			propagate();
		}
	}

	bool NeuralNet::save(const char *fname) {
		FILE *fp;
		struct netrecord record;

		if (!(fp = fopen(fname, "wb")))
			return false;

		record.input_size = input->size();
		record.hidden_size = hidden->size();
		record.output_size = output->size();

		record.epochs = ref_epochs;
		record.l_rate = l_rate;
		record.ex = ex;

		if (fwrite(&record, sizeof(struct netrecord), 1, fp) <= 0)
			return false;

		// Saving neurons' state
		for (unsigned int i = 0; i < input->size(); i++) {
			struct neuronrecord r;
			r.prop = (*input)[i].getProp();
			r.actv = (*input)[i].getActv();
			fwrite(&r, sizeof(struct neuronrecord), 1, fp);
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			struct neuronrecord r;
			r.prop = (*hidden)[i].getProp();
			r.actv = (*hidden)[i].getActv();
			fwrite(&r, sizeof(struct neuronrecord), 1, fp);
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			struct neuronrecord r;
			r.prop = (*output)[i].getProp();
			r.actv = (*output)[i].getActv();
			fwrite(&r, sizeof(struct neuronrecord), 1, fp);
		}

		// Saving synapsis' state
		for (unsigned int i = 0; i < input->size(); i++) {
			int nout = (*input)[i].nOut();
			fwrite(&nout, sizeof(int), 1, fp);

			for (int j = 0; j < nout; j++) {
				struct synrecord r;
				r.w = (*input)[i].synOut(j).getWeight();
				r.d = (*input)[i].synOut(j).getDelta();
				fwrite(&r, sizeof(struct synrecord), 1,
				       fp);
			}
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			int nin = (*output)[i].nIn();
			fwrite(&nin, sizeof(int), 1, fp);

			for (int j = 0; j < nin; j++) {
				struct synrecord r;
				r.w = (*output)[i].synIn(j).getWeight();
				r.d = (*output)[i].synIn(j).getDelta();
				fwrite(&r, sizeof(struct synrecord), 1,
				       fp);
			}
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nin = (*hidden)[i].nIn();
			fwrite(&nin, sizeof(int), 1, fp);

			for (int j = 0; j < nin; j++) {
				struct synrecord r;
				r.w = (*hidden)[i].synIn(j).getWeight();
				r.d = (*hidden)[i].synIn(j).getDelta();
				fwrite(&r, sizeof(struct synrecord), 1,
				       fp);
			}
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nout = (*hidden)[i].nOut();
			fwrite(&nout, sizeof(int), 1, fp);

			for (int j = 0; j < nout; j++) {
				struct synrecord r;
				r.w = (*hidden)[i].synOut(j).getWeight();
				r.d = (*hidden)[i].synOut(j).getDelta();
				fwrite(&r, sizeof(struct synrecord), 1,
				       fp);
			}
		}

		fclose(fp);
		return true;
	}

	NeuralNet::NeuralNet(const char *fname) throw(NetworkFileNotFoundException) {
		struct netrecord record;
		FILE *fp;

		if (!(fp = fopen(fname, "rb")))
			throw NetworkFileNotFoundException();

		if (fread(&record, sizeof(struct netrecord), 1, fp) <= 0)
			throw NetworkFileNotFoundException();

		*this =
		    NeuralNet(record.input_size, record.hidden_size,
			      record.output_size, record.l_rate,
			      record.epochs);

		// Restore neurons
		for (unsigned int i = 0; i < input->size(); i++) {
			struct neuronrecord r;
			fread(&r, sizeof(struct neuronrecord), 1, fp);

			(*input)[i].setProp(r.prop);
			(*input)[i].setActv(r.actv);
			(*input)[i].synClear();
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			struct neuronrecord r;
			fread(&r, sizeof(struct neuronrecord), 1, fp);

			(*hidden)[i].setProp(r.prop);
			(*hidden)[i].setActv(r.actv);
			(*hidden)[i].synClear();
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			struct neuronrecord r;
			fread(&r, sizeof(struct neuronrecord), 1, fp);

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
			fread(&nout, sizeof(int), 1, fp);

			for (int j = 0; j < nout; j++) {
				struct synrecord r;
				fread(&r, sizeof(struct synrecord), 1, fp);

				(*input)[i].synOut(j).setWeight(r.w);
				(*input)[i].synOut(j).setDelta(r.d);
			}
		}

		for (unsigned int i = 0; i < output->size(); i++) {
			int nin;
			fread(&nin, sizeof(int), 1, fp);

			for (int j = 0; j < nin; j++) {
				struct synrecord r;
				fread(&r, sizeof(struct synrecord), 1, fp);

				(*output)[i].synIn(j).setWeight(r.w);
				(*output)[i].synIn(j).setDelta(r.d);
			}
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nin;
			fread(&nin, sizeof(int), 1, fp);

			for (int j = 0; j < nin; j++) {
				struct synrecord r;
				fread(&r, sizeof(struct synrecord), 1, fp);

				(*hidden)[i].synIn(j).setWeight(r.w);
				(*hidden)[i].synIn(j).setDelta(r.d);
			}
		}

		for (unsigned int i = 0; i < hidden->size(); i++) {
			int nout;
			fread(&nout, sizeof(int), 1, fp);

			for (int j = 0; j < nout; j++) {
				struct synrecord r;
				fread(&r, sizeof(struct synrecord), 1, fp);

				(*hidden)[i].synOut(j).setWeight(r.w);
				(*hidden)[i].synOut(j).setDelta(r.d);
			}
		}

		fclose(fp);
	}

	void NeuralNet::train(string xmlsrc, NeuralNet::source src =
			      file) throw(InvalidXMLException) {
		double out;
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
				vector < double >input;
				double output;
				bool valid = false;

				xml.IntoElem();

				while (xml.FindChildElem("INPUT")) {
					xml.IntoElem();
					input.
					    push_back(atof
						      (xml.GetData().
						       c_str()));
					xml.OutOfElem();
				}

				if (xml.FindChildElem("OUTPUT")) {
					xml.IntoElem();
					output =
					    atof(xml.GetData().c_str());
					xml.OutOfElem();
				}

				xml.OutOfElem();

				while (!valid) {
					stringstream ss(stringstream::in | stringstream::out);

					setInput(input);
					propagate();
					setExpected(output);
					update();

					out = getOutput();

					ss << out;

					if (ss.str().find("inf") == string::npos)
						valid = true;
				}
			}
		}

		return;
	}

	void NeuralNet::initXML(string & xml) {
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

	string NeuralNet::XMLFromSet(int id, string set) {
		string xml;
		vector <double> in, out;
		unsigned int delimPos = -1;
		char delim = ';';
		char tmp[1024];

		for (delimPos = 0;
		     delimPos < set.length() && set[delimPos] != delim;
		     delimPos++);

		if (delimPos == set.length())
			return xml;

		string inStr = set.substr(0, delimPos);
		string outStr = set.substr(delimPos + 1, set.length());

		in = split(',', inStr);
		out = split(',', outStr);

		snprintf(tmp, sizeof(tmp), "%d", id);
		xml += "\t<TRAINING ID=\"" + string(tmp) + "\">\n";

		for (unsigned int i = 0; i < in.size(); i++) {
			memset(tmp, 0x0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", i);
			xml += "\t\t<INPUT ID=\"" + string(tmp) + "\">";

			memset(tmp, 0x0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%f", in[i]);
			xml += string(tmp) + "</INPUT>\n";
		}

		for (unsigned int i = 0; i < out.size(); i++) {
			memset(tmp, 0x0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%d", i);
			xml += "\t\t<OUTPUT ID=\"" + string(tmp) + "\">";

			memset(tmp, 0x0, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "%f", out[i]);
			xml += string(tmp) + "</OUTPUT>\n";
		}

		xml += "\t</TRAINING>\n\n";
		return xml;
	}

	void NeuralNet::closeXML(string & xml) {
		xml.append("</NETWORK>\n\n");
	}
}

