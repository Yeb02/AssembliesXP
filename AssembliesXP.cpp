
#include <iostream>

#include "Vanilla.h"


int main()
{
    int n = 100;
    int k = 10;
    float beta = 1.2f;
    float p = .4f;


    VanillaNeuronGroup va(n, k);
    VanillaConnectome c(beta, p, (INeuronAssembly*) &va, (INeuronAssembly*) &va);

    for (int i = 0; i < k; i++) {
        va.firing[INT_0X(n)] = 1;
    }

    for (int i = 0; i < 10; i++) 
    {
        for (int j = 0; j < n; j++) {
            if (va.firing[j]) std::cout << j << " ";
        }
        std::cout << std::endl;

        c.transmitActivations();
        va.fire();
        c.updateParameters();
    }
}