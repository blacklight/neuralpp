/**
 * This source makes sums between two numbers using pre-trained neural network
 * saved on "adder.net"
 *
 * by BlackLight, 2009
 */

#include <iostream>
#include <neural++.hpp>
using namespace neuralpp;

#define	NETFILE	"adder.net"

int main()  {
	float a,b;
	NeuralNet *net = NULL;

	try  {
		net = new NeuralNet(NETFILE);
	}

	catch (NetworkFileNotFoundException e)  {
		cerr << "Fatal error while opening " << NETFILE << ": " << e.what();
		return 1;
	}

	cout << "First number to add: ";
	cin >> a;

	cout << "Second number to add: ";
	cin >> b;

	vector<float> v;
	v.push_back(a);
	v.push_back(b);

	net->setInput(v);
	net->propagate();
	cout << "Neural net output: " << net->getOutput() << endl;

	return 0;
}

