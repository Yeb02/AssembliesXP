#pragma once

#include "AssemblyInterface.h"
#include "Random.h"

#include <vector>
#include <queue>

class VanillaNeuronGroup : public INeuronAssembly
{
public:
	VanillaNeuronGroup(int _n, int _k) : k(_k), INeuronAssembly(_n) {};
	
	void fire() override 
	{
		std::fill(firing, firing + n, 0);

		q.empty();

		for (int i = 0; i < n; ++i) {
			if (q.size() < k)
				q.push(std::pair<double, int>(preSynActivations[i], i));
			else if (q.top().first < preSynActivations[i]) {
				q.pop();
				q.push(std::pair<double, int>(preSynActivations[i], i));
			}
		}
		for (int i = 0; i < k; ++i) {
			firing[q.top().second] = 1;
			q.pop();
		}

		std::fill(preSynActivations, preSynActivations + n, 0.0f);
	}

	int k;

	// util to get the k greatest activation's indices, as per justHelloWorld's answer at:
	// https://stackoverflow.com/questions/14902876/indices-of-the-k-largest-elements-in-an-unsorted-length-n-array
	std::priority_queue< 
		std::pair<float, int>,
		std::vector< std::pair<float, int> >,
		std::greater <std::pair<float, int> > 
		> q;

};


class VanillaConnectome: public IConnectome 
{
public:
	float* weights;
	int nWPerNode;
	float beta;

	VanillaConnectome(float _beta, float _p, INeuronAssembly* _originAssembly, INeuronAssembly* _destinationAssembly) 
		: beta(_beta), IConnectome(_originAssembly, _destinationAssembly, _p)
	{
		weights = new float[nWPerONode * originAssembly->n];
		
		for (int i = 0; i < originAssembly->n; i++) {
			for (int j = 0; j < nWPerONode; j++) {
				weights[i * nWPerONode + j] = UNIFORM_01;
			}
		}

		normalize();
	};

	// normalize (sum = 1) the OUTGOING weights. TODO try normalizing the incoming ones.
	void normalize() 
	{
		for (int i = 0; i < originAssembly->n; i++) {
			float s = 0.0f;
			for (int j = 0; j < nWPerONode; j++) {
				s += weights[i * nWPerONode + j];
			}
			s = 1.0f / s;
			for (int j = 0; j < nWPerONode; j++) {
				weights[i * nWPerONode + j] *= s;
			}
		}
	}

	void transmitActivations() override 
	{
		for (int i = 0; i < originAssembly->n; i++) 
		{
			if (originAssembly->firing[i]) [[unlikely]] {
				int id = i * nWPerONode;
				for (int j = 0; j < nWPerONode; j++) {
					destinationAssembly->preSynActivations[destinationIDs[id]] += weights[id];
					id++;
				}
			}
		}
	}

	void updateParameters() override
	{
		for (int i = 0; i < originAssembly->n; i++)
		{
			if (originAssembly->firing[i]) [[unlikely]] {
				int id = i * nWPerONode;
				for (int j = 0; j < nWPerONode; j++) {
					if (destinationAssembly->firing[destinationIDs[id]]) [[unlikely]] {
						weights[id] *= beta;
					}
					id++;
				}
			}
		}
	}
};