#include "mex.h"
#include "rplidar.h"

/* Input Arguments */

#define	Y_IN	prhs[0]

/* output Arguments*/

#define	YP_OUT1	plhs[0]
#define	YP_OUT2	plhs[1]
#define	YP_OUT3	plhs[2]
#define	YP_OUT4	plhs[3]

#if !defined(max)
#define	max(A, B)	((A) > (B) ? (A) : (B))
#endif

#if !defined(min)
#define	min(A, B)	((A) < (B) ? (A) : (B))
#endif

////FUNCION LLAMADA POR MATLAB (en este caso "rplidar")/////
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double   *yp1, *yp2;
    uint8_T  *yp3;
    uint16_T *yp4;
    const int	m = MAXREADINGS;    
    unsigned int	m1,n1;
    
    /* Check for proper number of arguments */
    if ((nrhs!= 0) || (nlhs != 4))
    {
        mexErrMsgTxt("No se requiere argumento de entrada y se requieren cuatro argumentos de salida, ejemplo: [ang_deg dist_m quality num]=rplidar");
        return;
    }
    else
    {
        /* Create a matrix for the return argument */
        YP_OUT1 = mxCreateDoubleMatrix(m, 1, mxREAL);
        YP_OUT2 = mxCreateDoubleMatrix(m, 1, mxREAL);
        YP_OUT3 = mxCreateNumericMatrix(m, 1, mxUINT8_CLASS, mxREAL);
        YP_OUT4 = mxCreateNumericMatrix(1, 1, mxUINT16_CLASS, mxREAL);
        /* Assign pointers to the various parameters */

        yp1 = mxGetDoubles(YP_OUT1);
        yp2 = mxGetDoubles(YP_OUT2);
        yp3 = mxGetUint8s(YP_OUT3);
        yp4 = mxGetUint16s(YP_OUT4);

        CSharedStruct<LaserData>m_SharedData_RPLIDAR(M_COMPARTIDA_RPLIDAR);

        m_SharedData_RPLIDAR.Acquire();
        /* Do the computations*/
        *yp4 = m_SharedData_RPLIDAR->count;
        for(int j=0; j < *yp4; j++)
        {
            yp1[j] = m_SharedData_RPLIDAR->angle_deg[j];
            yp2[j] = m_SharedData_RPLIDAR->distance_m[j];
            yp3[j] = m_SharedData_RPLIDAR->quality[j];
        }
        for(int j=*yp4; j < MAXREADINGS; j++)
        {
            yp1[j] = 0.0f;
            yp2[j] = 0.0f;
            yp3[j] = 0;
        }
        m_SharedData_RPLIDAR.Release();
    }
    return;
}
////FUNCION LLAMADA POR MATLAB (en este caso "rplidar")/////
