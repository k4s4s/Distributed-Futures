
#ifndef Future_H
#define Future_H

#include <iostream>
#include <cassert>
#include "futures_enviroment.hpp"
#include "communication/communication.hpp"

//There is an issue if no BOOST_CLASS_EXPORT is present with linking boost::mpi::exception
class linker_fix {
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
	linker_fix() {};
	~linker_fix() {};
};
BOOST_CLASS_EXPORT(linker_fix);

namespace futures {

template <class T, class DataT>
class AsyncTask {
public:
	virtual ~AsyncTask() {};
	virtual T operator()(DataT args) = 0; 
};

template <class T>
class Future {
private:
    bool ready_status;
    int src_id, dst_id;
		communication::SharedDataManager *sharedData;
public:
		Future(int _src_id, int _dst_id, 
					communication::SharedDataManager *_sharedData);
    ~Future();
    bool is_ready();
    T get();
};

template<typename T, typename F, typename ... Args>
Future<T> *async(int src_id, int dst_id,
                 unsigned int data_size, unsigned int type_size,
                 F& f, Args ...args);

namespace details {

template<typename TX>
struct _get_data {
    TX operator()(communication::SharedDataManager* sharedDataManager, boost::mpl::true_) {
				TX value;		
				sharedDataManager->get_data(&value);
        return value;
    };
    TX operator()(communication::SharedDataManager* sharedDataManager, boost::mpl::false_) {
				TX value;
  			boost::mpi::packed_iarchive ia(sharedDataManager->get_comm());
				sharedDataManager->get_data(ia);
  			// Deserialize the data in the message
  			ia >> value;		
        return value;
    };
};

template<typename TX>
struct _get_data<TX*> {
    TX* operator()(communication::SharedDataManager* sharedDataManager, boost::mpl::true_) {
				TX* value = new TX[sharedDataManager->get_dataSize()];			
				sharedDataManager->get_data(value);
        return value;
    };
    TX* operator()(communication::SharedDataManager* sharedDataManager, boost::mpl::false_) {
				TX* value = new TX[sharedDataManager->get_dataSize()];	
				sharedDataManager->get_data(value);
        return value;
    };
};

template<typename TX>
struct _set_data {
    void operator()(communication::SharedDataManager* sharedDataManager,
										TX value, boost::mpl::true_) {
    	sharedDataManager->set_data(&value);
    };
    void operator()(communication::SharedDataManager* sharedDataManager,
										TX value, boost::mpl::false_) {
  		boost::mpi::packed_oarchive oa(sharedDataManager->get_comm());
  		oa << value;
    	sharedDataManager->set_data(oa);
    };
};

template<typename TX>
struct _set_data<TX*> {
    void operator()(communication::SharedDataManager* sharedDataManager, 
										TX* value, boost::mpl::true_) {
    	sharedDataManager->set_data(value);
    };
    void operator()(communication::SharedDataManager* sharedDataManager, 
										TX* value, boost::mpl::false_) {
    	sharedDataManager->set_data(value);
    };
};

template<typename TX>
struct _sizeof {
	int operator()() {
		return sizeof(TX);
	};
};

template<typename TX>
struct _sizeof<TX*> {
	int operator()() {
		return sizeof(TX);
	};
};

template<typename TX>
struct _get_mpi_datatype {
	MPI_Datatype operator()(boost::mpl::true_) {
		return boost::mpi::get_mpi_datatype<TX>(TX());
	};
	MPI_Datatype operator()(boost::mpl::false_) {
		return MPI_DATATYPE_NULL;
	};
};

template<typename TX>
struct	_get_mpi_datatype<TX*> {
	MPI_Datatype operator()(boost::mpl::true_) {
		return boost::mpi::get_mpi_datatype<TX>(TX());
	};
	MPI_Datatype operator()(boost::mpl::false_) {
		return MPI_DATATYPE_NULL;
	};
};

template<typename TX>
struct _is_mpi_datatype
 : public boost::mpi::is_mpi_datatype<TX>
{};

template<typename TX>
struct _is_mpi_datatype<TX*>
 : public boost::mpi::is_mpi_datatype<TX>
{};

}//end of namespace details

/** Implementation of async function **/
template<typename T, typename F, typename ... Args>
Future<T> *async_impl(int src_id, int dst_id,
                 			unsigned int data_size, unsigned int type_size,
                 			F& f, Args ...args) {
    Futures_Enviroment *env = Futures_Enviroment::Instance();
    int id = env->get_procId();
		if(id == src_id || id == dst_id) {
			communication::SharedDataManager *sharedData;
			sharedData = env->new_SharedDataManager(src_id, dst_id, data_size, type_size,
																							details::_get_mpi_datatype<T>()(
																								details::_is_mpi_datatype<T>()));
			if(id == dst_id) {
				Future<T> *future = new Future<T>(src_id, dst_id, sharedData);
				return future;
			}
			else if(id == src_id) {
        T retval = f(args...);
        details::_set_data<T>()(sharedData, retval, details::_is_mpi_datatype<T>());
				int ready_status = 1;
        sharedData->set_status(&ready_status);
				delete sharedData;
			}
		}
    //Master and the rest should skip here directly
    return NULL;//the rest procs should move on... //FIXME: may return NULL if proc_rank != future_rank
};

template<typename T, typename F, typename ... Args>
Future<T> *async(unsigned int data_size, F& f, Args ...args) {
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	int type_size = details::_sizeof<T>()();
	int src_id = env->get_avaibleWorker();
	return async_impl<T>(1, 0, data_size, type_size, f, args...);
}

template<typename T, typename F, typename ... Args>
Future<T> *async(F& f, Args ...args) {
	//FIXME: assert if T is pointer
	Futures_Enviroment *env = Futures_Enviroment::Instance();
	int type_size = details::_sizeof<T>()();
	int dst_id = env->get_avaibleWorker();
	return async_impl<T>(1, 0, 1, type_size, f, args...);
}

template <class T> Future<T>::Future(int _src_id, int _dst_id, 
																		communication::SharedDataManager *_sharedData) {
    ready_status = 0;
    src_id = _src_id;
		dst_id = _dst_id;
		sharedData = _sharedData;
};

template <class T> Future<T>::~Future() {
    Futures_Enviroment *env = Futures_Enviroment::Instance();
		int id = env->get_procId();
    if(id == dst_id) {
			delete sharedData;
		}
};

template <class T> bool Future<T>::is_ready() {
    Futures_Enviroment* env = Futures_Enviroment::Instance();
		int id = env->get_procId();
		assert(id == dst_id);
    int ready_status;
    return sharedData->get_status(&ready_status);
};

template <class T> T Future<T>::get() {
    Futures_Enviroment* env = Futures_Enviroment::Instance();
		int id = env->get_procId();
		assert(id == dst_id);
    int ready_status;
    while (1) {
        sharedData->get_status(&ready_status);
        if (ready_status) break;
    }
    return	details::_get_data<T>()(sharedData, details::_is_mpi_datatype<T>());
};

}//end of futures namespace

#endif

