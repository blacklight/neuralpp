/**
 * This source creates a new neural network able to sum two integer numbers,
 * creating the XML containing the training input set on the fly.
 *
 * by BlackLight, 2009
 */

#include <iostream>
#include <neural++.hpp>

using namespace std;
using namespace neuralpp;

int main()  {
	NeuralNet net(2, 2, 2, 0.005, 100);
	string xml;
	double tmp;
	int id = 0;

	// XML initialization. Then, I say XML that 2+3=5, 3+3=6, 5+4=9
	// Strings' format is "input1,input2,...,inputn;output1,output2,...,outputm
	NeuralNet::initXML(xml);
	xml += NeuralNet::XMLFromSet(id, "3,2;5,1");
	xml += NeuralNet::XMLFromSet(id, "4,2;6,2");
	xml += NeuralNet::XMLFromSet(id, "6,3;9,3");
	NeuralNet::closeXML(xml);
	cout << xml << endl;
	
	net.train(xml, NeuralNet::str);
	vector<double> v;
	cout << "Network status: trained\n\n";

	cout << "First number to add: ";
	cin >> tmp;
	v.push_back(tmp);

	cout << "Second number to add: ";
	cin >> tmp;
	v.push_back(tmp);

	net.setInput(v);
	net.propagate();
	cout << "Output: " << net.getOutputs()[0] << "; " << net.getOutputs()[1] << endl;
	return 0;
}

