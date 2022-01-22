__kernel void simpleAdd(__global float *a,
                        __global float *b,
				        __global float *c,
						const unsigned int n) {
		      int id = get_global_id(0);
			 
			  c[id] = a[id] + b[id];
}
