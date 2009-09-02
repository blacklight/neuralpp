/**
 * Show how to train a network that performs sums between
 * two real numbers. The training XML is built from scratch, then saved to a file, then
 * the network is initialized using that XML file, trained, and the resulting trained
 * network is saved to adder.net. Then, you should take a look at doAdd.cpp to see how
 * to use that file to use the network.
 *
 * by BlackLight, 2009
 */

#include <iostream>
#include <fstream>
#include <ctime>
#include <neural++.hpp>

using namespace std;
using namespace neuralpp;

int main()  {
	int id = 0;
	string xml;
	time_t t1, t2;

	// Create the neural network. The network is going to have
	// => 2 neurons for the input layer
	// => 2 neurons for the hidden layer
	// => 1 neuron  for the output layer
	// => a learning rate == 0.002 (just get it doing some tests until satisfied, but remember to keep its value quite low and ~ 0 to keep the network stable)
	// => 1000 learning steps (i.e. the network will be ready after 1000 training steps to adjust the synaptical weights
	// => 0.1 as neural threshold (the threshold above which a neuron activates)
	NeuralNet net(2, 2, 1, 0.002, 2000);

	// Initialize a training XML as a string in 'xml'
	NeuralNet::initXML(xml);

	// Build some training sets for the XML. The format is:
	// "input1,input2,...,inputn;output1,output2,...,outputn
	// The 'id' variable is passed as reference, starting from 0,
	// and it's used to enumerate the sets in the XML file.
	xml += NeuralNet::XMLFromSet(id, "2,3;5");
	xml += NeuralNet::XMLFromSet(id, "3,2;5");
	xml += NeuralNet::XMLFromSet(id, "6,2;8");
	xml += NeuralNet::XMLFromSet(id, "2,2;4");
	xml += NeuralNet::XMLFromSet(id, "1,2;3");
	xml += NeuralNet::XMLFromSet(id, "-1,-2;-3");
	xml += NeuralNet::XMLFromSet(id, "8,9;17");
	xml += NeuralNet::XMLFromSet(id, "10,10;20");
	xml += NeuralNet::XMLFromSet(id, "4,1;5");
	xml += NeuralNet::XMLFromSet(id, "2,6;8");
	xml += NeuralNet::XMLFromSet(id, "2,7;9");
	xml += NeuralNet::XMLFromSet(id, "8,9;17");
	NeuralNet::closeXML(xml);

	// Save the XML string just created to a file
	ofstream out("adder.xml");
	out << xml;
	out.close();
	cout << "Training file adder.xml has been written\n";

	// Start the training from the XML file
	t1 = time(NULL);
	cout << "Training in progress - This may take a while...\n";
	net.train("adder.xml", NeuralNet::file);
	t2 = time(NULL);

	// Save the trained network to a binary file, that can be reloaded from any
	// application that is going to use that network
	net.save("network.xml");
	cout << "Network trained in " << (t2-t1) << " seconds. You can use adder.net file now to load this network\n";
	return 0;
}

