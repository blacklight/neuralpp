/**
 * This source creates a new neural network able to sum two integer numbers,
 * creating the XML containing the training input set on the fly.
 *
 * by BlackLight, 2009
 */

#include <iostream>
#include <neural++.hpp>
using namespace neuralpp;

int main()  {
	NeuralNet *net = new NeuralNet(2, 2, 1, -0.005, 10000);
	string xml;
	float tmp;

	// XML initialization. Then, I say XML that 2+3=5, 3+3=6, 5+4=9
	// Strings' format is "input1,input2,...,inputn;output1,output2,...,outputm
	NeuralNet::initXML(xml);
	xml += NeuralNet::XMLFromSet(0, "2,3;5");
	xml += NeuralNet::XMLFromSet(1, "3,3;6");
	xml += NeuralNet::XMLFromSet(2, "5,4;9");
	NeuralNet::closeXML(xml);
	
	net->train(xml, NeuralNet::str);
	vector<float> v;
	cout << "Network status: trained\n\n";

	cout << "First number to add: ";
	cin >> tmp;
	v.push_back(tmp);

	cout << "Second number to add: ";
	cin >> tmp;
	v.push_back(tmp);

	net->setInput(v);
	net->propagate();
	cout << "Output: " << net->getOutput() << endl;
	return 0;
}

