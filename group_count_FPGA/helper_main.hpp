#ifndef HELPER_MAIN_HPP
#define HELPER_MAIN_HPP

using namespace std;

void initializeHashMap(uint32_t* hashVec, uint32_t* countVec, uint64_t HSIZE);
void validate(uint64_t dataSize, uint32_t* hashVec, uint32_t* countVec, uint64_t HSIZE);
void validate_element(uint32_t *data, uint64_t dataSize, uint32_t*hashVec, uint32_t* countVec, uint64_t HSIZE);

/**
  *  Generate a data array with random values between 1 and #distinctValues
  *  The array is dynamically sized. The number of elements corresponds to the value in dataSize.
  * @todo : change data generation function to a function with real random values
  */
template <typename T>
void generateData(T* arr, uint64_t distinctValues, uint64_t dataSize) {
    int i;    
    for(i=0;i<dataSize;i+=1){
        arr[i] = 1+ (rand() % distinctValues);
    }
} 

#endif  // HELPER_MAIN_HPP