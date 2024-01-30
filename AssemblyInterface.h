#pragma once

#include <algorithm>
#include "Random.h"

// The set of neurons only, as we conceptually split the assembly classe in
// two : the neurons and the connections.
class INeuronAssembly
{
public :
	int n; // the number of neurons

	float* preSynActivations;
	int* firing;

	INeuronAssembly(int _n) : n(_n)
	{
		preSynActivations = new float[n];
		firing = new int[n];
		zero();
	}

	// to be overriden by the derived class. Determines which neurons should be firing.
	virtual void fire() = 0;

	virtual void zero() {
		std::fill(firing, firing + n, 0);
		std::fill(preSynActivations, preSynActivations+n, 0.0f);
	};

	~INeuronAssembly() {
		delete[] firing;
		delete[] preSynActivations;
	}
};


// The (directed) connectome between 2 neuron groups (potentially the same). We do not guarantee 
// that a neuron is connected at most one to another (i.e. parallel connexions are possible)
class IConnectome
{
public:
	float p; // The probability of a connection between a destination and an origin node. 
	
	// For simplicity, we assume each neuron in the origin assembly has exactly 
	// nWPerONode = (int)((float)destinationAssembly->n * p) outgoing connections
	int nWPerONode;

	// The IDs of the destination neurons.
	int* destinationIDs;

	INeuronAssembly* originAssembly;
	INeuronAssembly* destinationAssembly;

	IConnectome(INeuronAssembly* _originAssembly, INeuronAssembly* _destinationAssembly, float _p) :
		originAssembly(_originAssembly), destinationAssembly(_destinationAssembly), p(_p) 
	{
		nWPerONode = (int)((float)destinationAssembly->n * p);
		destinationIDs = new int[nWPerONode * originAssembly->n];
		for (int i = 0; i < originAssembly->n; i++) {
			for (int j = 0; j < nWPerONode; j++) {
				destinationIDs[i * nWPerONode + j] = INT_0X(destinationAssembly->n);
			}
		}
	};

	// to be overriden by the derived class. Accumulates the firing of the origin neurons
	// in the presynaptic activation of the destination neurons, in a fashion handled by the
	// derived class.
	virtual void transmitActivations() = 0;

	// to be overriden by the derived class. Changes the connexions parameters according to
	// hebbian rules on plasticity.
	virtual void updateParameters() = 0;

	~IConnectome() {
		delete[] destinationIDs;
	}
};
