
#ifndef _MPI_DETAILS_H
#define _MPI_DETAILS_H

#include <mpi.h>
//#include <boost/mpi.hpp>
#include <boost/mpi/datatype.hpp>

#include "cereal/cereal.hpp"
#include "cereal/archives/raw.hpp"

#define AR_SIZE_OFFSET 0
#define DATA_OFFSET sizeof(int)

namespace futures {
namespace communication {
namespace mpi_details {

static void MPI_Exclusive_get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                         int target_rank, MPI_Aint target_disp, int target_count,
                         MPI_Datatype target_datatype, MPI_Win win);

static void MPI_Exclusive_put(void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                         int target_rank, MPI_Aint target_disp, int target_count,
                         MPI_Datatype target_datatype, MPI_Win win);

static void MPI_Group_create_comm(MPI_Group group, MPI_Comm comm, MPI_Comm *comm_new, int tag);

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
struct _put {
    int operator()(TX& data,
										int dst_id,
										int count,
										int offset,
										MPI_Win data_win, 
										boost::mpl::true_) {
        MPI_Datatype datatype = _get_mpi_datatype<TX>()(boost::mpi::is_mpi_datatype<TX>());
    		MPI_Exclusive_put(&data, count, datatype, dst_id, 
													offset, count, datatype, data_win);
				return count*sizeof(TX);
    };
    int operator()(TX& data,
										int dst_id,
										int count,
										int offset,
										MPI_Win data_win, 
										boost::mpl::false_) {
        cereal::RawOutputArchive oa;
        oa(data);
				int size = oa.size();
    		MPI_Exclusive_put((void*)(&size), 1, MPI_INT, dst_id, 
													offset+AR_SIZE_OFFSET,
                          1, MPI_INT, data_win);
    		MPI_Exclusive_put(const_cast<void*>(oa.address()), size, 
																						MPI_PACKED, dst_id,
																						offset+DATA_OFFSET, size, 
																						MPI_PACKED, data_win);
				return size+sizeof(size); //data and size variable sizes
    };
};

template<typename TX>
struct _put<TX*> {
    int operator()(TX* data,
										int dst_id,
										int count,
										int offset,
										MPI_Win data_win, 
										boost::mpl::true_) {
        MPI_Datatype datatype = _get_mpi_datatype<TX>()(boost::mpi::is_mpi_datatype<TX>());
    		MPI_Exclusive_put(data, count, datatype, dst_id, 
													offset, count, datatype, data_win);
				return count*sizeof(TX);
    };
    int operator()(TX* data,
										int dst_id,
										int count,
										int offset,
										MPI_Win data_win, 
										boost::mpl::false_) {
				std::cout << "calling special put" << std::endl;
/*
        cereal::RawOutputArchive oa;
        oa(count, boost::serialization::make_array(data, count));
				int size = oa.size();
    		MPI_Exclusive_put((void*)(&size), 1, MPI_INT, dst_id, 
													offset+AR_SIZE_OFFSET,
                          1, MPI_INT, data_win);
    		MPI_Exclusive_put(const_cast<void*>(oa.address()), size, 
																						MPI_PACKED, dst_id,
																						offset+DATA_OFFSET, size, 
																						MPI_PACKED, data_win);
				return size+sizeof(size);
*/
				return 0;
    };
};

template<typename TX>
struct _get {
    TX operator()(int src_id,
									int count,
									int offset,
									MPI_Win data_win,
									boost::mpl::true_) {
			TX data;
 			MPI_Datatype datatype = _get_mpi_datatype<TX>()(boost::mpi::is_mpi_datatype<TX>());
    	MPI_Exclusive_get(&data, count, datatype, src_id, offset,
                        count, datatype, data_win);
			return data;
		}
    TX operator()(int src_id,
									int count,
									int offset,
									MPI_Win data_win, 
									boost::mpl::false_) {
        TX data;
				cereal::RawInputArchive ia;
    		MPI_Exclusive_get((void*)&count, 1, MPI_INT, src_id, 
													offset+AR_SIZE_OFFSET,
		                      1, MPI_INT, data_win);
				// Prepare input buffer and receive the message
				ia.resize(count);
    		MPI_Exclusive_get(const_cast<void*>(ia.address()), ia.size(), 
																						MPI_PACKED, src_id,	offset+DATA_OFFSET,
																						ia.size(), MPI_PACKED, data_win);
        // Deserialize the data in the message
        ia(data);
        return data;
    };
};

template<typename TX>
struct _get<TX*> {
    TX* operator()(int src_id,
									int size,
									int count,
									int offset,
									MPI_Win data_win,
									boost::mpl::true_) {
        TX* data = new TX[size];
 				MPI_Datatype datatype = _get_mpi_datatype<TX>()(boost::mpi::is_mpi_datatype<TX>());
    		MPI_Exclusive_get(data, count, datatype, src_id, offset,
                        	count, datatype, data_win);
        return data;
    };
    TX* operator()(int src_id,
									int size,
									int count,
									int offset,
									MPI_Win data_win, 
									boost::mpl::false_) {
				std::cout << "calling special deserialize" << std::endl;
/*
        TX* data = new TX[size];
				cereal::RawInputArchive ia;
    		MPI_Exclusive_get((void*)&count, 1, MPI_INT, src_id, 
													offset+AR_SIZE_OFFSET,
		                      1, MPI_INT, data_win);
				// Prepare input buffer and receive the message
				ia.resize(count);
    		MPI_Exclusive_get(const_cast<void*>(ia.address()), ia.size(), 
																						MPI_PACKED, src_id,	offset+DATA_OFFSET,
																						ia.size(), MPI_PACKED, data_win);
        ia(count);
        // Deserialize the data in the message
        boost::serialization::array<TX> arr(data, count);
        ia(arr);
        return data; //FIXME: need to destory array object at some point
				//or not? it's not a pointer, it should be destroyed at return
*/
			return NULL;
    };
};

/*
template<typename TX>
struct _send_data {
    void operator()(communication::CommInterface* commInterface, int dst_id, int tag,
                    TX data, boost::mpl::true_) {
        commInterface->send(dst_id, tag, 1, boost::mpi::get_mpi_datatype<TX>(TX()), &data);
    };
    void operator()(communication::CommInterface* commInterface, int dst_id, int tag,
                    TX data, boost::mpl::false_) {
        boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
        oa << data;
        commInterface->send(dst_id, tag, oa);
    };
};

template<typename TX>
struct _send_data<TX*> {
    void operator()(communication::CommInterface* commInterface, int dst_id, int tag,
                    TX* data, boost::mpl::true_) {
        commInterface->send(dst_id, tag, 1, boost::mpi::get_mpi_datatype<TX>(TX()), data);
    };
    void operator()(communication::CommInterface* commInterface, int dst_id, int tag,
                    TX* data, boost::mpl::false_) {
        int n = sizeof(TX); //FIXME: maybe add support for arrays, not necessary at the moment though
        boost::mpi::packed_oarchive oa(MPI_COMM_WORLD);
        oa << n << boost::serialization::make_array(data, n);
        commInterface->send(dst_id, tag, oa);
    };
};

template<typename TX>
struct _recv_data {
    TX operator()(communication::CommInterface* commInterface, int src_id, int tag,
                  boost::mpl::true_) {
        TX data;
        commInterface->recv(src_id, tag, 1, boost::mpi::get_mpi_datatype<TX>(TX()), &data);
        return data;
    };
    TX operator()(communication::CommInterface* commInterface, int src_id, int tag,
                  boost::mpl::false_) {
        TX data;
        boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
        commInterface->recv(src_id, tag, ia);
        ia >> data;
        return data;
    };
};

template<typename TX>
struct _recv_data<TX*> {
    TX* operator()(communication::CommInterface* commInterface, int src_id, int tag,
                   boost::mpl::true_) {
        TX* data = new TX[1];
        commInterface->recv(src_id, tag, 1, boost::mpi::get_mpi_datatype<TX>(TX()), data);
        return data;
    };
    TX* operator()(communication::CommInterface* commInterface, int src_id, int tag,
                   boost::mpl::false_) {
        TX* data = new TX[1];
        boost::mpi::packed_iarchive ia(MPI_COMM_WORLD);
        commInterface->recv(src_id, tag, ia);
        // Determine how much data we are going to receive
        int count;
        ia >> count;
        // Deserialize the data in the message
        boost::serialization::array<TX> arr(data, count);
        ia >> arr;
        return data;
    };
};
*/

}//end of details namespace
}
}

static void futures::communication::mpi_details::MPI_Exclusive_get(void *origin_addr,
        int origin_count, MPI_Datatype origin_datatype,
        int target_rank, MPI_Aint target_disp, int target_count,
        MPI_Datatype target_datatype, MPI_Win win) {
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
    MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count,
            target_datatype, win);
    MPI_Win_unlock(target_rank, win);
};

static void futures::communication::mpi_details::MPI_Exclusive_put(void *origin_addr,
        int origin_count, MPI_Datatype origin_datatype,
        int target_rank, MPI_Aint target_disp, int target_count,
        MPI_Datatype target_datatype, MPI_Win win) {
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
    MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count,
            target_datatype, win);
    MPI_Win_unlock(target_rank, win);
};

static void futures::communication::mpi_details::MPI_Group_create_comm(MPI_Group group,
        MPI_Comm comm, MPI_Comm *comm_new, int tag) {
    //REQUIRE: group is ordered by desired rank in comm and is identical on all callers
    int rank, grp_rank, grp_size;
    MPI_Group new_group;
    MPI_Comm_rank(comm, &rank);
    MPI_Group_rank(group, &grp_rank);
    MPI_Group_size(group, &grp_size);
    MPI_Comm_dup(MPI_COMM_SELF, comm_new);
    int *grp_pids = new int[grp_size];
    int *pids = new int[grp_size];
    for(int i=0; i < grp_size; i++) {
        grp_pids[i] = i;
    }
    MPI_Group parent_grp;
    MPI_Comm_group(comm, &parent_grp);
    MPI_Group_translate_ranks(group, grp_size, grp_pids, parent_grp, pids);
    MPI_Group_free(&parent_grp);

    for(int merge_sz=1; merge_sz < grp_size; merge_sz = merge_sz*2) {
        int gid = grp_rank/merge_sz;
        MPI_Comm ic, comm_old = *comm_new; //FIXME: not sure if this is ok, I think openmpi has a pointer underneath
        if(gid%2 == 0) {
            if((gid+1)*merge_sz < grp_size) {
                MPI_Intercomm_create(*comm_new, 0, comm, pids[(gid+1)*merge_sz], tag, &ic);
                MPI_Intercomm_merge(ic, 0, comm_new);
            }
        }
        else {
            MPI_Intercomm_create(*comm_new , 0, comm, pids[(gid-1)*merge_sz], tag, &ic);
            MPI_Intercomm_merge(ic, 1, comm_new);
        }
        if(*comm_new != comm_old) {
            MPI_Comm_free(&ic);
            MPI_Comm_free(&comm_old);
        }
    }
    delete pids;
    delete grp_pids;
};

#endif
