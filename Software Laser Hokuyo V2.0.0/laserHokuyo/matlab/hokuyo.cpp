#include "mex.h"
#include "hokuyo.h"

/* Input Arguments */

#define	Y_IN	prhs[0]

/* output Arguments*/

#define	YP_OUT	plhs[0]

#if !defined(max)
#define	max(A, B)	((A) > (B) ? (A) : (B))
#endif

#if !defined(min)
#define	min(A, B)	((A) < (B) ? (A) : (B))
#endif

////FUNCION LLAMADA POR MATLAB (en este caso "hokuyo")/////
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int16_T   *yp;
    const int	m = NUMREADINGS;
    unsigned int	m1,n1;
    
    /* Check for proper number of arguments */
    if ((nrhs!= 0) || (nlhs > 1))
    {
        mexErrMsgTxt("No se requiere argumento de entrada y si uno de salida, ejemplo: distancia_mm=hokuyo");
        return;
    }
    else
    {
        /* Create a matrix for the return argument */
        YP_OUT = mxCreateNumericMatrix(m, 1, mxINT16_CLASS, mxREAL);
        /* Assign pointers to the various parameters */

        yp = (int16_T *)mxGetData(YP_OUT);

        CSharedStruct<LaserData>m_SharedData_Laser(M_COMPARTIDA_HOKUYO);

        m_SharedData_Laser.Acquire();
        /* Do the computations*/
        for(int j=0; j < NUMREADINGS; j++)
        {
            if(m_SharedData_Laser->distance_mm[j] < minDistance)
                yp[j] = maxDistance;
            else if(m_SharedData_Laser->distance_mm[j] > maxDistance)
                yp[j] = maxDistance;
            else
                yp[j] = (int16_T)(m_SharedData_Laser->distance_mm[j]);
        }
        m_SharedData_Laser.Release();
    }
    return;
}
////FUNCION LLAMADA POR MATLAB (en este caso "hokuyo")/////
