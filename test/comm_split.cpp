#include <mpp.h>
#include <iostream>


int main(int argc, char * argv []) {

mpi::init(argc,argv);
mpi::comm worldcomm = mpi::comm::world;
int rank,size,color,key;
rank = worldcomm.rank();
size = worldcomm.size();
color = rank / 2;
key = rank % 2;
mpi::comm splitcomm = worldcomm.split(color,key);
int splitrank = splitcomm.rank();
int splitsize = splitcomm.size();
std::cout << "size = " << size << ", rank = " << rank << ", splitsize = " << splitsize << ", and splitrank = " << splitrank << "\n";
mpi::finalize();

return 0;

}
