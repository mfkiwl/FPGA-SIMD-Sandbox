#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <array>

/**
 * This file contains the scalar primitves of the Intel Intrinsics, which are used 
 * in the own AVX512-implementation of the hashbased group_count.
 * These functions will later be used to run the logic of the AVX512 implementation 
 * on a FPGA within the Intel DevCloud.
*/

template<typename T>
struct fpvec {
    [[intel::fpga_register]] std::array<T, 64/sizeof(T)> elements;
};

/**	#1
 * serial primitive for Intel Intrinsic:
 * _mm512_setzero_epi32
 */
template<typename T>
fpvec<T> setzero_uint32() {
	auto reg = fpvec<T>{};
	uint32_t zero = 0;
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		reg.elements[i] = zero;
	}
	return reg;
}

/**	#2
 * serial primitive for Intel Intrinsic:
 * _mm512_setr_epi32
 * 
 * function will (currently) only be working for arrys with 16 elements a 32bit integers!
 */
template<typename T>
fpvec<T> setr_16slot_uint32(uint32_t e15, uint32_t e14, uint32_t e13, uint32_t e12, uint32_t e11, uint32_t e10, uint32_t e9,
	uint32_t e8, uint32_t e7, uint32_t e6, uint32_t e5, uint32_t e4, uint32_t e3, uint32_t e2, uint32_t e1, uint32_t e0) {
	auto result = fpvec<T>{};
#pragma unroll
	result[0] = e0;
	result[1] = e1;
	result[2] = e2;
	result[3] = e3;
	result[4] = e4;
	result[5] = e5;
	result[6] = e6;
	result[7] = e7;
	result[8] = e8;
	result[9] = e9;
	result[10] = e10;
	result[11] = e11;
	result[12] = e12;
	result[13] = e13;
	result[14] = e14;
	result[15] = e15;
	return result;
}

/**	#3
* serial primitive for Intel Intrinsic:
* _mm512_set1_epi32
*/
template<typename T>
fpvec<T> set1_uint32(uint32_t value) {
	auto reg = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		reg.elements[i] = value;
	}
	return reg;
}

/**	#4
* serial primitive for Intel Intrinsic:
* _cvtu32_mask16
* original description: "Convert integer value a into an 16-bit mask, and store the result in k."*
*/
template<typename T>
fpvec<T> cvtu32_mask16_uint32(uint32_t value) {
	auto reg = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		// tbd
	}
	return reg;
}

/**	#5
* serial primitive for two Intel Intrinsics:
* _mm512_maskz_loadu_epi32	:	original description: "Load packed 32-bit integers from memory 
*								into dst using zeromask k (elements are zeroed out when the 
*								corresponding mask bit is not set). mem_addr does not need to 
*								be aligned on any particular boundary."
* _mm512_mask_loadu_epi32	:	original description: "Load packed 32-bit integers from memory 
*								into dst using writemask k (elements are copied from src when 
*								the corresponding mask bit is not set). 
*								mem_addr does not need to be aligned on any particular boundary."
*
* customized loadu-function:
* param1 writeMask : if bit is set to "1" load related item from data
* param2 data : array which contains the data which should be loaded
* param3 startIndex : first index-position of data from where the data should be loaded
* param4 HSZIZE : HSIZE that describes the size of the arrays of the Hashvector (data array)
*/
template<typename T>
fpvec<T> mask_loadu_uint32(fpvec<T> writeMask, uint32_t* data, uint32_t startIndex, uint64_t HSIZE) {
	auto result = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		if (writeMask[i] == 1) {
			result[i] = data[(startIndex+i)%HSIZE];
		}
		else {
			result[i] = 0;
		}
	}
	return result;
}

/**	#6
* serial primitive for Intel Intrinsic:
* _mm512_mask_cmpeq_epi32_mask
* original description: "Compare packed 32-bit integers in a and b for equality, and store the results in mask vector k 
* using zeromask k1 (elements are zeroed out when the corresponding mask bit is not set)."
*/
template<typename T>
fpvec<T> mask_cmpeq_epi32_mask_uint32(fpvec<T> zeroMask, fpvec<T> a, fpvec<T> b) {
	auto resultMask = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		if (zeroMask[i] == 1) {
			if (a[i] == b[i]) {
				resultMask[i] = 1;
			}	
			else {
				resultMask[i] = 0;
			}
		}	
		else {
			resultMask[i] = 0;
		}	
	}
	return resultMask;
}

/**	#7
* serial primitive for Intel Intrinsic:
* _mm512_mask_add_epi32
* original description: "Add packed 32-bit integers in a and b, and store the results in dst using writemask k 
* (elements are copied from src when the corresponding mask bit is not set)."
*/
template<typename T>
fpvec<T> mask_add_epi32_uint32(fpvec<T> src, fpvec<T> writeMask, fpvec<T> a, fpvec<T> b) {
	auto result = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		if (writeMask[i] == 1) {
			result[i] = a[i] + b[i];
		}
		else {
			result[i] = src[i];
		}
	}
	return result;
}

/**	#8
* serial primitive for Intel Intrinsic:
* _mm512_mask_storeu_epi32
* original description: "Store packed 32-bit integers from a (=data) into memory using writemask k. 
* mem_addr does not need to be aligned on any particular boundary."
*
* customized store  - function:
* param1 result : array, in which the data is stored, if related bit of writeMask is set to "1"
* param2 startIndex : first index - position of data from where the data should be stored
* param3 HSZIZE : HSIZE that describes the size of the arrays of the Hashvector (result array)
* param4 writeMask : if bit is set to "1" -> store related item from data into result array
* param4 data : register-array which contains the data that should be stored
*/
template<typename T>
void _mm512_mask_storeu_epi32_uint32(uint32_t* result, uint32_t startIndex, uint64_t HSIZE, fpvec<T> writeMask, fpvec<T> data) {
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		if (writeMask[i] == 1) {
			result[(startIndex+i)%HSIZE] = data[i];
		}
		else {
			result[(startIndex+i)%HSIZE] = result[(startIndex+i)%HSIZE];		// not necessary? do nothing?
		}
	}
}

/**	#9
* serial primitive for Intel Intrinsic:
* _mm512_mask2int
* original description: "Converts bit mask k1 into an integer value, storing the results in dst."
*/
template<typename T>
uint32_t mask2int_uint32(fpvec<T> mask) {
#pragma unroll	
	for (int i=0; i<(64/sizeof(T)); i++) {
		//tbd
	}
}

/**	#10
* serial primitive for Intel Intrinsic:
* _mm512_knot
* original description: "Compute the bitwise NOT of 16-bit mask a, and store the result in k."
*/
template<typename T>
fpvec<T> knot_uint32(fpvec<T> src) {
	auto result = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		if (src[i] == 0) {
			result[i] = 1;
		}
		else {
			result[i] = 0;
		}
	}
	return result;
}

/**	#11
* serial primitive for Built-in Function Provided by GCC:
* __builtin_clz
*/

/**	#12
* serial primitive for Intel Intrinsic:
* _mm512_load_epi32
* original description: "Load 512-bits (composed of 16 packed 32-bit integers) from memory into dst. 
* mem_addr must be aligned on a 64-byte boundary or a general-protection exception may be generated."
*
* customized load-function:
* param2 data : array which contains the data which should be loaded
* param3 startIndex : first index-position of data from where the data should be loaded
* param4 HSZIZE : HSIZE that describes the size of the arrays of the Hashvector (data array)
*/
template<typename T>
fpvec<T> load_epi32_uint32(uint32_t* data, uint32_t startIndex, uint64_t HSIZE) {
	auto result = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		result[i] = data[(startIndex+i)%HSIZE];
	}
	return result;
}

/**	#13
* serial primitive for Intel Intrinsic:
* _mm512_cmpeq_epi32_mask
* original description: "Compare packed 32-bit integers in a and b for equality, and store the results in mask vector k."
*/
template<typename T>
fpvec<T> cmpeq_epi32_mask_uint32(fpvec<T> a, fpvec<T> b) {
	auto resultMask = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		if (a[i] == b[i]) {
			resultMask[i] = 1;
		}
		else {
			resultMask[i] = 0;
		}
	}
	return resultMask;
}

/**	#14
* serial primitive for Intel Intrinsic:
* _mm512_permutexvar_epi32
* original description: "Shuffle 32-bit integers in a across lanes using the corresponding index in idx, and store the results in dst."
*/
template<typename T>
fpvec<T> permutexvar_epi32_uint32(fpvec<T> idx, fpvec<T> a) {
	auto resultMask = fpvec<T>{};
#pragma unroll
	for (int i=0; i<(64/sizeof(T)); i++) {
		// tbd
	}
	return resultMask;
}

#endif // PRIMITIVES_HPP