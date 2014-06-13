// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Starka
// Copyright (c) 2009-2014 Illumina, Inc.
//
// This software is provided under the terms and conditions of the
// Illumina Open Source Software License 1.
//
// You should have received a copy of the Illumina Open Source
// Software License 1 along with this program. If not, see
// <https://github.com/sequencing/licenses/>
//

/// \file
///
/// \author Chris Saunders
///

#pragma once

struct prog_info
{
    virtual
    const char* name() const = 0;

    virtual
    const char* version() const = 0;

    virtual
    void usage(const char* xmessage = 0) const = 0;

    virtual
    void doc() const = 0;

protected:
    prog_info() {}
    virtual ~prog_info() {}
};
