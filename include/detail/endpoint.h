/******************************************************************************
 *
 *                          MPP: An MPI CPP Interface
 *
 *                  Copyright (C) 2011-2012  Simone Pellegrini
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 ******************************************************************************/

#pragma once 

#include "detail/decls.h"

#define OVERLOAD_SEND(name, impl) \
	template <class MsgType> \
	inline endpoint& name(msg_impl<MsgType>&& m) { \
		return name##_impl(impl, std::move(m)); \
	} \
	template <class MsgType> \
	inline endpoint& name(const msg_impl<MsgType>& m) { \
		return name(std::move(m)); \
	} \
	template <class RawType> \
	inline endpoint& name(const RawType& m) { \
		return name( std::move( msg_impl<const RawType>(m) ) ); \
	} 

namespace mpi {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// endpoint: represent the src or dest of an MPI channel. Provides streaming
// operations to send/recv messages (msg<T>) both in a synchronous or asynch
// ronous way.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class endpoint {

	const int 		m_rank;	 // The rank of this endpoint
	const comm& 	m_comm;  // The MPI communicator this endpoing
							 // belongs to

	typedef int (*send_ptr)(void*,int,MPI_Datatype,int,int,MPI_Comm);

	// Make this class non-copyable 
	endpoint(const endpoint& other) = delete;
	endpoint& operator=(const endpoint& other) = delete;

public:
	endpoint(const int& rank, const comm& com):
		m_rank(rank), m_comm(com) { }

	endpoint(endpoint&& other) :
		m_rank(other.m_rank), 
		m_comm(std::move(other.m_comm)) { }

	// Send a generic message to this endpoint (synchronously)
	template <class MsgType> 
	inline endpoint& send_impl(const send_ptr& func, msg_impl<MsgType>&& m);

	// MPI_Send wrappers 
	OVERLOAD_SEND(send, MPI_Send)

	// MPI_Ssend wrappers 
	OVERLOAD_SEND(ssend, MPI_Ssend)

	// MPI_Rsend wrappers 
	OVERLOAD_SEND(rsend, MPI_Rsend)

	// Send a generic message to this endpoint (asynchronously)
	template <class MsgType> 
	inline request<MsgType> isend(msg_impl<MsgType>&& m);

	// Send a generic message to this endpoint (asynchronously)
	template <class MsgType> 
	inline request<MsgType> isend(const msg_impl<MsgType>& m) {
		return isend(std::move(m));
	}

	// Send a generic message to this endpoint (asynchronously)
	template <class RawMsg> 
	inline request<const RawMsg> isend(const RawMsg& m) {
		return isend( std::move( msg_impl<const RawMsg>(m) ) );
	}


	template <class MsgType>
	inline endpoint& operator<<(msg_impl<MsgType>&& m) { 
		return send(std::move(m)); 
	}

	template <class MsgType>
	inline endpoint& operator<<(const msg_impl<MsgType>& m) {
		return send(std::move(m));
	}

	template <class RawType>
	inline endpoint& operator<<(const RawType& m) {
		return send( std::move( msg_impl<const RawType>(m) ) );
	}




	// Receive from this endpoint (synchronously)
	template <class RawType>
	inline status operator>>(RawType& m);

	template <class MsgType>
	inline status operator>>(msg_impl<MsgType>&& m);

	// Receive from this endpoing (asynchronously)
	template <class MsgType>
	inline request<MsgType> operator>(msg_impl<MsgType>&& m);

	// Receive from this endpoing (asynchronously
	template <class RawType>
	inline request<RawType> operator>(RawType& m) {
		return operator>( std::move(msg_impl<RawType>(m)) );
	}

	// Returns the rank of this endpoit
	inline const int& rank() const { return m_rank; }
};

} // end mpi namespace 

#include "detail/comm.h"

namespace mpi {

// Send a generic message to this endpoint (synchronously)
template <class MsgType>
inline endpoint& endpoint::send_impl(const send_ptr& func, msg_impl<MsgType>&& m) {
	MPI_Datatype&& dt = m.type();
	if ( func(const_cast<void*>(static_cast<const void*>(m.addr())), 
			  static_cast<int>(m.size()), dt,
			  m_rank, 
			  m.tag(), 
			  m_comm.mpi_comm()
			) == MPI_SUCCESS ) {
		return *this;
	}
	std::ostringstream ss;
	ss << "ERROR in MPI rank '" << comm::world.rank()
	   << "': Failed to send message to destination rank '"
	   << m_rank << "'";
	throw comm_error( ss.str() );
}

// Send a generic message to this endpoint (asynchronously)
template <class MsgType>
inline request<MsgType> endpoint::isend(msg_impl<MsgType>&& m) {
	MPI_Datatype&& dt = m.type();
	MPI_Request req;
	if ( MPI_Isend( const_cast<void*>(static_cast<const void*>(m.addr())), 
			  		static_cast<int>(m.size()), dt,
			  		m_rank, 
			  		m.tag(), 
			  		m_comm.mpi_comm(),
			  		&req
				) != MPI_SUCCESS ) 
	{
		std::ostringstream ss;
		ss << "ERROR in MPI rank '" << comm::world.rank()
		   << "': Failed to send message to destination rank '"
		   << m_rank << "'";

		throw comm_error( ss.str() );
	}
	return request<MsgType>(m_comm, req, std::move(m));
}

// Receive from this endpoing (asynchronously)
template <class MsgType>
inline request<MsgType> endpoint::operator>(msg_impl<MsgType>&& m) {
	MPI_Request req;
	if( MPI_Irecv( static_cast<void*>(m.addr()), 
				   static_cast<int>(m.size()), 
				   m.type(),
				   m_rank, 
				   m.tag(), 
				   m_comm.mpi_comm(), 
				   &req
				 ) != MPI_SUCCESS ) {
		std::ostringstream ss;
		ss << "ERROR in MPI rank '" << comm::world.rank()
		   << "': Failed to receive message from destination rank '"
		   << m_rank << "'";
		throw comm_error( ss.str() );
	}
	return request<MsgType>(m_comm, req, std::move(m));
}

} // end mpi namespace 


#include "detail/status.h"

namespace mpi {

template <class RawType>
inline status endpoint::operator>>(RawType& m) {
	return operator>>( msg_impl<RawType>( m ) );
}

template <class MsgType>
inline status endpoint::operator>>(msg_impl<MsgType>&& m) {
	status::mpi_status_ptr stat( new MPI_Status );
	MPI_Datatype dt = m.type();
	if(MPI_Recv( const_cast<void*>(static_cast<const void*>(m.addr())), 
				 static_cast<int>(m.size()), dt,
				 m_rank, 
				 m.tag(), 
				 m_comm.mpi_comm(), 
				 stat.get()
			   ) == MPI_SUCCESS ) {
		return status(m_comm, std::move(stat), dt);
	}
	std::ostringstream ss;
	ss << "ERROR in MPI rank '" << comm::world.rank()
	   << "': Failed to receive message from destination rank '"
	   << m_rank << "'";
	throw comm_error( ss.str() );
}

} // end mpi namespace 
