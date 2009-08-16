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
	NeuralNet net(3, 3, 1, 0.005, 1000);
	string xml;
	double tmp;

	// XML initialization. Then, I say XML that 2+3=5, 3+3=6, 5+4=9
	// Strings' format is "input1,input2,...,inputn;output1,output2,...,outputm
	NeuralNet::initXML(xml);
	xml += NeuralNet::XMLFromSet(0, "2,3,4;9");
	xml += NeuralNet::XMLFromSet(1, "3,3,1;7");
	xml += NeuralNet::XMLFromSet(2, "5,4,2;11");
	NeuralNet::closeXML(xml);
	
	net.train(xml, NeuralNet::str);
	vector<double> v;
	cout << "Network status: trained\n\n";

	cout << "First number to add: ";
	cin >> tmp;
	v.push_back(tmp);

	cout << "Second number to add: ";
	cin >> tmp;
	v.push_back(tmp);

	cout << "Third number to add: ";
	cin >> tmp;
	v.push_back(tmp);

	net.setInput(v);
	net.propagate();
	cout << "Output: " << net.getOutput() << endl;
	return 0;
}

