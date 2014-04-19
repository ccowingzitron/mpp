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

#include "mpp/decls.h"

namespace mpp {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// comm: is the abstraction of the MPI_Comm class
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class comm {

	MPI_Comm m_comm;
	bool 	 m_initialized;
	int 	 m_comm_size;
	int 	 m_rank;

public: // Constructor and check_init made public to facilitate splitting of communicators, by CACZ 14-4-16
	comm(MPI_Comm comm):
		m_comm(comm),
		m_initialized(false),
		m_comm_size(-1),
		m_rank(-1) { }

	// Check whether MPI_Init has been called
	inline void check_init() {

		if (m_initialized) { return; }

		int flag;
		MPI_Initialized(&flag);
		assert(flag != 0 &&
			"FATAL: MPI environment not initialized (MPI_Init not called)");

		m_initialized = true;
		MPI_Comm_size(m_comm, &m_comm_size);
		MPI_Comm_rank(m_comm, &m_rank);
	}

public:
	// MPI_COMM_WORLD
	static comm world;

	inline int rank() {
		check_init();
		return m_rank;
	}

	inline int rank() const {
		assert(m_initialized && "MPI communicator not initialized");
		return m_rank;
	}

	inline int size() {
		check_init();
		return m_comm_size;
	}

	inline int size() const {
		assert(m_initialized && "MPI communicator not initialized");
		return m_comm_size;
	}

	const MPI_Comm& mpi_comm() const {
		return m_comm;
	}

	inline endpoint operator()( const int& rank_id ) const;

// method split added to implement MPI_Comm_split, by CACZ 14-4-16
	inline comm split( const int& color, const int&key ) const {

		MPI_Comm m_split_comm;
		assert( MPI_Comm_split(m_comm,color,key,&m_split_comm) == MPI_SUCCESS );
		comm m_split(m_split_comm);
		m_split.check_init();
		return m_split;

	}

// method barrier added to implement MPI_Comm_split, by CACZ 14-4-16
	inline void barrier() const {
		MPI_Barrier(m_comm);
	}

};

comm comm::world = comm(MPI_COMM_WORLD);

} // end mpi namespace

#include "mpp/endpoint.h"

namespace mpp {

inline endpoint comm::operator()(const int& rank_id) const {
	return endpoint(rank_id, *this);
}

} // end mpi namespace
