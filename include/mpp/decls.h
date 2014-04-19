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

#include <mpi.h>

#include <stdexcept>

// the following five headers were included to ensure correct compilation, by CACZ 14-4-16
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>

/**
 * Contains the forward declaration of the base classes utilized by MPP
 */

namespace mpp {


// undefined added, and pair of const ints moved to decls.h to make them globally available, CACZ 14-4-18

const int any = MPI_ANY_SOURCE;
const int undefined = MPI_UNDEFINED;


template <class T>
class msg_impl;

class status;

// added here, CACZ 14-4-18
typedef std::shared_ptr<MPI_Status> mpi_status_ptr;

template <class T>
class request;

class endpoint;

// Expection which is thrown every time a communication fails
struct comm_error : public std::logic_error {

	comm_error(const std::string& msg) : std::logic_error(msg) { }

};

}
