/**
 * Show how to use a network already trained and saved to a
 * binary file. In this case, a network trained to simply perform sums between two real
 * numbers, that should have already been created using learnAdd.
 *
 * by BlackLight, 2009
 */

#include <iostream>
#include <neural++.hpp>

using namespace std;
using namespace neuralpp;

#define	NETFILE	"network.xml"

int main()  {
	double a,b;
	NeuralNet net;

	// Load the pre-trained network from "adder.net" file
	try  {
		net = NeuralNet(NETFILE);
	}

	catch (NetworkFileNotFoundException e)  {
		cerr << "Fatal error while opening " << NETFILE << ": " << e.what();
		return 1;
	}

	cout << "First number to add: ";
	cin >> a;

	cout << "Second number to add: ";
	cin >> b;

	vector<double> v;
	v.push_back(a);
	v.push_back(b);

	// Set the numbers just read as input values, propagate those values, and get
	// the output
	net.setInput(v);
	net.propagate();
	cout << "Neural net output: " << net.getOutput() << endl;

	return 0;
}

