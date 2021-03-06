/**********************************************************************
 * DESCRIPTION:
 *   Cuda Concurrent Wave Equation - Cuda C Version
 *   This program implements the concurrent wave equation
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <cuda.h>

#define MAXPOINTS 1000000
#define MAXSTEPS 1000000
#define MINPOINTS 20
#define PI 3.14159265

void check_param(void);
void init_line(void);
void update (void);
void printfinal (void);

int nsteps,                 	/* number of time steps */
    tpoints, 	     		/* total points along string */
    rcode;                  	/* generic return code */
float  values[MAXPOINTS+2], 	/* values at time t */
       oldval[MAXPOINTS+2], 	/* values at time (t-dt) */
       newval[MAXPOINTS+2]; 	/* values at time (t+dt) */

// define the value on devices
// use pointer to point them
float *device_values;


/**********************************************************************
 *	Checks input values from parameters
 *********************************************************************/
void check_param(void)
{
   char tchar[20];

   /* check number of points, number of iterations */
   while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
      printf("Enter number of points along vibrating string [%d-%d]: "
           ,MINPOINTS, MAXPOINTS);
      scanf("%s", tchar);
      tpoints = atoi(tchar);
      if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
         printf("Invalid. Please enter value between %d and %d\n", 
                 MINPOINTS, MAXPOINTS);
   }
   while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
      printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
      scanf("%s", tchar);
      nsteps = atoi(tchar);
      if ((nsteps < 1) || (nsteps > MAXSTEPS))
         printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
   }

   printf("Using points = %d, steps = %d\n", tpoints, nsteps);

}

/**********************************************************************
 *     Initialize points on line
 *********************************************************************/
void init_line(void)
{
   int i, j;
   float x, fac, k, tmp;

   /* Calculate initial values based on sine curve */
   fac = 2.0 * PI;
   k = 0.0; 
   tmp = tpoints - 1;
   for (j = 1; j <= tpoints; j++) {
      x = k/tmp;
      values[j] = sin (fac * x);
      k = k + 1.0;
   } 

   /* Initialize old values array */
   for (i = 1; i <= tpoints; i++) 
      oldval[i] = values[i];
}

/**********************************************************************
 *      Calculate new values using wave equation
 *********************************************************************/
//float total_oldval;
//float total_values;
/*void do_math(int i)
{
   float dtime, c, dx, tau, sqtau;

   dtime = 0.3;
   c = 1.0;
   dx = 1.0;
   tau = (c * dtime / dx);
   sqtau = tau * tau;
   newval[i] = (2.0 * values[i]) - oldval[i] + (sqtau *  (-2.0)*values[i]);
}*/
__device__ float do_math(float total_oldval, float total_values)
	{
	   float dtime, c, dx, tau, sqtau;
	
	   dtime = 0.3;
	   c = 1.0;
	   dx = 1.0;
	   tau = (c * dtime / dx);
      sqtau = tau * tau;
      // modefy original one into the version of :
      // given total oldval to do math with total current value to get total new value
		float total_newval;
		total_newval = (2.0 * total_values) - total_oldval + (sqtau *  (-2.0)*total_values);
		return total_newval;
	}
	

/**********************************************************************
 *     Update all values along line a specified number of times
 *********************************************************************/
/* void update()
{
   int i, j;

   // Update values for each time step 
   for (i = 1; i<= nsteps; i++) {
      // Update points along line for this time step 
      for (j = 1; j <= tpoints; j++) {
         // global endpoints 
         if ((j == 1) || (j  == tpoints))
            newval[j] = 0.0;
         else
            do_math(j);
      }

      // Update old values with new values 
      for (j = 1; j <= tpoints; j++) {
         oldval[j] = values[j];
         values[j] = newval[j];
      }
   }
}*/
__global__ void update(float *device_values, int tpoints, int nsteps)
{
	int i, j;
	j = (1+threadIdx.x) + blockIdx.x*32;
	if( j <= tpoints ){
		float total_values = device_values[j];
		float total_oldval = total_values;
		float total_newval;
		for(i=1; i<=nsteps; i++){
         // Update points along line for this time step 
         if((j==1) || (j==tpoints))
         // global endpoints 
				total_newval = 0.0;
			else
            total_newval = do_math(total_oldval, total_values);
         // Update old values with new values 
			total_oldval = total_values;
			total_values = total_newval;
      }
      // put current value on device into device_values array 
		device_values[j] = total_values;
	}
}
	

/**********************************************************************
 *     Print final results
 *********************************************************************/
void printfinal()
{
   int i;

   for (i = 1; i <= tpoints; i++) {
      printf("%6.4f ", values[i]);
      if (i%10 == 0)
         printf("\n");
   }
}

/**********************************************************************
 *	Main program
 *********************************************************************/
int main(int argc, char *argv[])
{
	sscanf(argv[1],"%d",&tpoints);
   sscanf(argv[2],"%d",&nsteps);
   // Create a space for cuda memory
   cudaMalloc(&device_values, sizeof(float)*(1+tpoints));
	check_param();
	printf("Initializing points on the line...\n");
   init_line();
   cudaMemcpy(device_values, values, sizeof(float)*(1+tpoints), cudaMemcpyHostToDevice);
	printf("Updating all points for all time steps...\n");
   //update();
   // We allocate the memroy considered block number and threadsperbllock
   // You can take the offical document as reference: 3.2.2. Device Memory
   // https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html
   int block;
   int threadsPerBlock = 256;
	if(tpoints%32){
      // the part of less than 32 still need to be allocate
		block = 1 + tpoints/32;
	}
	else{
		block = tpoints/32;
   }
   update<<<block, threadsPerBlock>>>(device_values, tpoints, nsteps);
	cudaMemcpy(values, device_values, sizeof(float)*(1+tpoints), cudaMemcpyDeviceToHost);
	printf("Printing final results...\n");
	printfinal();
   printf("\nDone.\n\n");
   cudaFree(device_values);
	
	return 0;
}
