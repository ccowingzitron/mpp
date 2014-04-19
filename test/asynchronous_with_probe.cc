#include <mpp.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <chrono>



int main(int argc, char** argv) {

	mpp::init(argc, argv);

 	if ( mpp::comm::world.rank() == 0 ) {
 		std::vector< int > veci {1,2,3,4};
 		std::vector< char > vecc {'a','b','c','d','e'};
 		std::stringstream ss;
 		ss << "process 0: ";
 		for (int i = 0 ; i != 4 ; ++i) {
 			ss << veci[i] << " " << vecc[i] << ",";
 		}
 		ss << vecc[4] << "\n";
 		std::cout << ss.str();
 		auto reqi = mpp::comm::world(1).isend(mpp::msg(veci,0));
 		auto reqc = mpp::comm::world(1).isend(mpp::msg(vecc,5));
 		std::cout << "process 0 waiting\n";
 		bool i = true, c = true;
 		for ( ; ; ) {
 			if (i) { if (reqi.test()) { veci.resize(0) ; i = false ; } }
 			if (c) { if (reqc.test()) { vecc.resize(0) ; c = false ; } }
 			if ((!i) && (!c)) { break ; }
 		}
 		std::cout << "process 0 done\n";
 	} else  {
 		std::vector< int > veci;
 		std::vector< char > vecc;
 		//std::cout << "process 1 requesting\n";
 		//auto reqc = mpp::comm::world(0) > mpp::msg(vecc,5);
 		//auto reqi = mpp::comm::world(0) > mpp::msg(veci,0);
 		std::cout << "process 1 waiting\n";
 		std::stringstream ss;
 		bool i = true, c = true;
 		for ( ; ; ) {
 			if (c) {
 				auto m = mpp::msg(vecc,5);
 				auto stat = mpp::comm::world(1).probe(std::move(m));
 				if (m.ready()) {
 					if (c) {
 						std::cout << "process 1: first c true, size = ";
 					}
 					vecc.resize(stat->count());
 					if (c) {
 						std::cout << vecc.size() << "\n";
 					}
 					mpp::comm::world(0) >> std::move(m);
 					c = false;
 				}
 			}
 			if (i) {
 				auto m = mpp::msg(veci,0);
 				auto stat = mpp::comm::world(1).probe(std::move(m));
 				if (m.ready()) {
 					if (i) {
 						std::cout << "process 1: first i true, size = ";
 					}
 					veci.resize(stat->count());
 					if (i) {
 						std::cout << veci.size() << "\n";
 					}
 					mpp::comm::world(0) >> std::move(m);
 					i = false;
 				}
 			}
 			if ((!i) && (!c)) { break ; }
 		}
 		ss << "process 1: ";
 		for (int i = 0 ; i != 4 ; ++i) {
 			ss << veci[i] << " " << vecc[i] << ",";
 		}
 		ss << vecc[4] << "\n";
 		std::cout << ss.str();
 	}

	mpp::finalize();
	return 0;
}
