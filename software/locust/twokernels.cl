#if defined(cl_amd_fp64) || defined(cl_khr_fp64)
	#ifdef cl_amd_fp64
		#pragma OPENCL EXTENSION cl_amd_fp64 : enable
	#elifdef cl_khr_fp64
		#pragma OPENCL EXTENSION cl_khr_fp64 : enable
	#endif
#else
	#define float double
#endif

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics: enable
#pragma OPENCL EXTENSION cl_khr_int64_extended_atomics: enable

#pragma OPENCL EXTENSION cl_khr_fp64 : enable

inline int CmGet(
	const unsigned int i,
	const unsigned int j,
	__global unsigned int *N)
	{
		return (i * (*N)) + j;
    }

inline unsigned int ReduceRows(
		const unsigned int original,
		const unsigned int offender,
		const unsigned int function,
		__global double* dataMatrix,
		__global double* dataRhs,
		__global unsigned int *N)
	{	
	double multiplier = dataMatrix[CmGet(original, function, N)] / dataMatrix[CmGet(offender, function, N)];
	multiplier *= -1;
	
	unsigned int flops = 2;
	
	for(unsigned int i = function; i < *N; i++)
	{
		if(i==function) 
			dataMatrix[CmGet(original, i, N)] = 0;
		else 
		{
			int origPos = CmGet(original, i, N);
			int offPos = CmGet(offender, i, N);
		
			dataMatrix[origPos] += (dataMatrix[offPos] * multiplier);
		}
		flops += 2;
	}

	dataRhs[original] += dataRhs[offender] * multiplier;
	
	flops += 2;
	return flops;
}

inline double ___abs(double val) 
{ 
	if(val < 0) 
		return val*-1.0f; 
	return val; 
}

inline unsigned int RowFunction(
		unsigned int row,
		__global double* dataMatrix,
		__global unsigned int *N)
		{		
			for(unsigned int ix = 0; ix < *N; ix++)
			{
				unsigned int mtxpos = CmGet(row, ix, N);
				
				// Sprawdzenie stabilnosci
				if(___abs(dataMatrix[mtxpos]) <= 0.0000000000001) 
					dataMatrix[mtxpos] = 0;
					
				// Sprawdzenie NaNa
				if(isnan(dataMatrix[mtxpos]))
					dataMatrix[mtxpos] = 0;
					
				if(dataMatrix[mtxpos] != 0) 
					return ix;
			}		
			
		return (*N);
}

__kernel void Slicer(
	__global unsigned int* N,
	__global unsigned int* M,
	__global double* dataMatrix,
	__global double* dataRhs, 
	__global int* map,
	__local int* localMap,
	__global unsigned int* flops)
{		
	int threadID = get_local_id(0);
	int blockID = get_group_id(0);
	int param_block_size = get_local_size(0);
	
	unsigned int lflops = 0;
	
	if(0 == threadID) 
	{
		for(int i=0; i<(*N); i++) 
				localMap[i] = -1;
	}
	
	// numer wiersza lokalny do slice'a
	int rnumber = blockID * param_block_size + threadID;
	
	barrier(CLK_LOCAL_MEM_FENCE);
	
	if(rnumber < (*M))
	{
		while(true){
			int function = RowFunction(rnumber, dataMatrix, N);
			
			if(function == *N) 
				break;
			
			int offender = atomic_cmpxchg(&(localMap[function]), -1, rnumber);
			
			if(offender != -1)
				lflops += ReduceRows(rnumber, offender, function, dataMatrix, dataRhs, N);
			else 
				break;
		}
	}
		
	barrier(CLK_LOCAL_MEM_FENCE);
		
	if(threadID == 0)
	{
		for(int i=0; i<(*N); i++)
		{
			map[blockID*(*N)+i] = localMap[i];
		}
	}
	
	atomic_add(flops, lflops);
}

__kernel void Resolver(
		__global unsigned int *N,
		__global double* dataMatrix,
		__global double* dataRhs,
		__global int* map,
		__global unsigned int *BLOX,
		__global unsigned int *ops,
		__global unsigned int *slices_size,
		__global unsigned int *flops)
	{
		int global_size = get_global_size(0);
		
		unsigned int ile_na_watek = *slices_size / global_size;

		unsigned int lops = 0;

		unsigned int lflops = 0;
	
		for(unsigned int cur = 0; cur < ile_na_watek; cur++)
		{
			int row = get_global_id(0) * ile_na_watek + cur;
		
			if(row < *N)
			{
				int first = -1;
				int function = -1;

				for(int block=0; block<(*BLOX); block++)
				{
					if(map[block*(*N)+row] != -1)
					{
						if(first == -1){
							first = map[block*(*N)+row];
							function = row;
							continue;
						} else {
							int thisRow = map[block*(*N)+row];
							lflops += ReduceRows(thisRow, first, function, dataMatrix, dataRhs, N);
							lops++;
						}
					}
				}
			}
		}
		atomic_add(ops, lops);
		atomic_add(flops, lflops);
	}