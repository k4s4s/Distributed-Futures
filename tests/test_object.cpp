#include <iostream>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/mpi.hpp>
#include <map>
#include <string>

#define MASTER 0

class func {
private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int /* file_version */) {};
public:
	func() {};
	~func() {};
	int operator()() {
		std::cout << "Hello World!" << std::endl;	
	};
};

namespace mpi = boost::mpi;

int main(int argc, char* argv[]) {
#if 0
  mpi::environment env(argc, argv);
  mpi::communicator world;

  if (world.rank() == 0) {
		_stub *pack = new _async_stub<func>();
    world.send(1, 0, pack);
  } else {
   	_stub *pack;
    world.recv(0, 0, pack);
		pack->run();
  }
#endif
}

