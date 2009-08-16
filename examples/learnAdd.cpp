/**
 * This source creates a new neural network able to sum two integer numbers,
 * using the training input set saved on "adder.xml" and saving output network
 * on "adder.net"
 *
 * by BlackLight, 2009
 */

#include <iostream>
#include <neural++.hpp>

using namespace std;
using namespace neuralpp;

int main()  {
	NeuralNet net(2, 2, 1, 0.005, 1000);

	cout << "Training in progress - This may take a while...\n";
	net.train("adder.xml", NeuralNet::file);

	net.save("adder.net");
	cout << "Network trained. You can use adder.net file now to load this network\n";
	return 0;
}

