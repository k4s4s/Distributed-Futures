#include <iostream>
#include <vector>
#include "futures.hpp"

using namespace std;
using namespace futures; 

#define DEFAULT_SIZE 1000
#define DEFAULT_MIN_WORK_UNIT 1

////////////////////////////////////////////////////////////
// Miscialenous functions
////////////////////////////////////////////////////////////
 
/** Swap to value */
template <class NumType>
inline void Swap(NumType& value, NumType& other){
    NumType temp = value;
    value = other;
    other = temp;
}

template <class NumType>
inline void Copy(vector<NumType>& outputArray, vector<NumType>& inputArray, 
								int startIndex, int endIndex) {
	for(int i=0; i < endIndex-startIndex; i++) {
		outputArray[i] = inputArray[i+startIndex];
	}
}

template <class NumType> 
inline void Merge(vector<NumType>& outputArray, 
									vector<NumType>& arrayA, vector<NumType>& arrayB) {
	for(int i=0; i < arrayA.size(); i++) {
		outputArray[i] = arrayA[i];
	}
	for(int i=0; i < arrayB.size(); i++) {
		outputArray[i+arrayA.size()+1] = arrayB[i];
	}
}

template <class SortedType>
bool isSorted(vector<SortedType> array, const long size){
    for(int idx = 1; idx < size ; ++idx){
        if(array[idx-1] > array[idx]){
            return false;
        }
    }
    return true;
}
 
template <class SortedType>
void print(vector<SortedType> array, const int size){
    for(int idx = 0 ;idx < size; ++idx){
        std::cout << array[idx] << "\t";
    }
    std::cout << "\n";
}

////////////////////////////////////////////////////////////
// Quick sort
////////////////////////////////////////////////////////////
 
/* use in the sequential qs */
template <class SortType>
long QsPartition(vector<SortType>& outputArray, long left, long right){
    const long part = right;
    Swap(outputArray[part],outputArray[left + (right - left ) / 2]);
    const SortType partValue = outputArray[part];
    --right;
 
    while(true){
        while(outputArray[left] < partValue){
            ++left;
        }
        while(right >= left && partValue <= outputArray[right]){
            --right;
        }
        if(right < left) break;
 
        Swap(outputArray[left],outputArray[right]);
        ++left;
        --right;
    }
 
    Swap(outputArray[part],outputArray[left]);
 
    return left;
}
 
/* a sequential qs */
template <class SortType>
void QsSequential(vector<SortType>& array, const long left, const long right){
    if(left < right){
        const long part = QsPartition(array, left, right);
        QsSequential(array,part + 1,right);
        QsSequential(array,left,part - 1);
    }
}
 
/** A task dispatcher */
class quicksort {
private:
	friend class boost::serialization::access;
	template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
      ar & min_work_unit;
	}
	long int min_work_unit;
public:
	quicksort() {};
	quicksort(long int _min_work_unit): min_work_unit(_min_work_unit) {};
	~quicksort() {};
	template <class SortType>
	vector<SortType> operator()(vector<SortType> array, const int deep) {
		const int left = 0;
		const int right = array.size()-1;
    if(left < right){
        if(array.size() > min_work_unit) {
            const long part = QsPartition(array, left, right);
 						vector<SortType> subarrA((right)-(part+1)+1), subarrB(part-1-left+1);
						Copy(subarrA, array, part+1, right+1);
						Copy(subarrB, array, left, part);
						quicksort qsort(min_work_unit);
						future<vector<SortType> > res1, res2;
						res1 = async2(subarrA.size(), qsort, subarrA, deep-1);
						res2 = async2(subarrB.size(), qsort, subarrB, deep-1);
            subarrA = res1.get();
            subarrB = res2.get();
						Merge(array, subarrB, subarrA);
        }
        else {
            const long part = QsPartition(array, left, right);
            QsSequential(array,part + 1,right);
            QsSequential(array,left,part - 1);
        }
    }
		return array;
	}
};

FUTURES_EXPORT_FUNCTOR((async_function<quicksort, vector<long>, int>));

////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////
 
int main(int argc, char** argv) {
 
	Futures_Initialize(argc, argv);
	long Size = DEFAULT_SIZE;
	long int min_work_unit = DEFAULT_MIN_WORK_UNIT;
	char c;

	while ((c = getopt(argc, argv, "n:w:")) != -1)
	switch (c)	{
		case 'n':
			Size = atoi(optarg);	 
		 	break;
		case 'w':
			min_work_unit = atoi(optarg);	 
		 	break;
	 	default:
			break;		
	}

  vector<long> array(Size);
   // Create array
  srand(0);
  for(long idx = 0 ; idx < Size ; ++idx){
      array[idx] = int(Size*(float(rand())/RAND_MAX));
  }
	//print(array,Size);
  printf("Sorting %ld elements\n", Size);
	REGISTER_TIMER("total time");
	START_TIMER("total time");
  // Start sorting
	quicksort qsort(min_work_unit);
  array = qsort(array, 10);
	STOP_TIMER("total time");
	PRINT_TIMER("total time");
	Futures_Finalize();
  // Test result
  if(isSorted(array,Size)){
      printf("Is sorted\n");
  }
  else{
      printf("Error array is not sorted!\n");
      if( Size <= 20) print(array,Size);
      return -1;
  }

  return 0;
}

