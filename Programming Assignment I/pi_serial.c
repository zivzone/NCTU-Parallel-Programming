#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    double pi_estimate, distance_squared, x, y;
    unsigned long long  number_of_cpu, number_of_tosses, number_in_circle, toss;
    if ( argc < 2) {
        exit(-1);
    }
    number_of_cpu = atoi(argv[1]);
    number_of_tosses = atoi(argv[2]);
    if (( number_of_cpu < 1) || ( number_of_tosses < 0)) {
        exit(-1);
    }

    number_in_circle = 0;
    for (toss = 0; toss < number_of_tosses; toss++) {
        // x = random double between -1 and 1;
        // y = random double between -1 and 1;
        distance_squared = x*x + y*y;
        if (distance_squared <= 1)
            number_in_circle++;
    }
    pi_estimate = 4*number_in_circle/((double) number_of_tosses);

    printf("%f\n",pi_estimate);
    return 0;
}
