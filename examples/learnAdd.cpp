/**
 * This source creates a new neural network able to sum two integer numbers,
 * using the training input set saved on "adder.xml" and saving output network
 * on "adder.net"
 *
 * by BlackLight, 2009
 */

#include <iostream>
#include <fstream>
#include <ctime>
#include <neural++.hpp>

using namespace std;
using namespace neuralpp;

double f (double x)  {
	return (x <= 0) ? 1 : 0;
}

int main()  {
	int id = 0;
	string xml;
	time_t t1, t2;
	NeuralNet net(2, 2, 1, 0.005, 1000, 0.1, f);

	NeuralNet::initXML(xml);
	xml += NeuralNet::XMLFromSet(id, "2,3;5");
	xml += NeuralNet::XMLFromSet(id, "3,2;5");
	xml += NeuralNet::XMLFromSet(id, "6,2;8");
	xml += NeuralNet::XMLFromSet(id, "2,2;4");
	xml += NeuralNet::XMLFromSet(id, "1,2;3");
	xml += NeuralNet::XMLFromSet(id, "-1,-2;-3");
	xml += NeuralNet::XMLFromSet(id, "8,9;17");
	xml += NeuralNet::XMLFromSet(id, "10,10;20");
	NeuralNet::closeXML(xml);

	ofstream out("adder.xml");
	out << xml;
	out.close();
	cout << "Training file adder.xml has been written\n";

	t1 = time(NULL);
	cout << "Training in progress - This may take a while...\n";
	net.train("adder.xml", NeuralNet::file);
	t2 = time(NULL);

	net.save("adder.net");
	cout << "Network trained in " << (t2-t1) << " seconds. You can use adder.net file now to load this network\n";
	return 0;
}

