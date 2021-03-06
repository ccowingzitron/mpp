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


#include "mpp/comm.h"
#include "mpp/endpoint.h"
#include "mpp/message.h"
#include "mpp/status.h"
#include "mpp/request.h"

#include <exception>

namespace mpp {

/**
 * Exception type used whenever the required thread level does not match
 * the one provided by the underlying MPI library.
 */
struct ThreadLevelException : public std::exception {
	const int required, provided;

	ThreadLevelException(int required, int provided):
		required(required), provided(provided) { }

	const char* what() const noexcept {
		return "Requested thread level does not match the one provided by the library.";
	}
};

enum thread_level { NO_THREAD,
					THREAD_SINGLE,
					THREAD_FUNNELED,
					THREAD_SERIALIZED,
					THREAD_MULTIPLE
				  };

inline void init(int argc = 0, char* argv[] = NULL, const thread_level required = NO_THREAD) {
	if(required == NO_THREAD) {
		MPI_Init(&argc, &argv);
		return;
	}

	int provided;
	// try initialize with the provided thread level
	MPI_Init_thread(&argc, &argv, required, &provided);

	if (provided < required)
		throw ThreadLevelException(required, provided);
}

inline void finalize(){ MPI_Finalize(); }

} // end mpi namespace

