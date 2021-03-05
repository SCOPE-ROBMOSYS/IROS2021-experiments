/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2007 Freyr Magnusson
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "DummyConnector2.h"
#include <yarp/os/StringInputStream.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>


using namespace yarp::os::impl;
using namespace yarp::os;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
class DummyConnector2Reader :
        public StreamConnectionReader
{
public:
    ConnectionWriter* altWriter;
    bool tmode;

    ConnectionWriter* getWriter() override
    {
//         altWriter->reset(tmode);
        return altWriter;
    }
};

class DummyConnector2::Private
{
private:
    BufferedConnectionWriter writer{false};
    DummyConnector2Reader reader;
    StringInputStream sis;
    bool textMode{false};

public:
    Private()
    {
        reader.altWriter = &writer;
        reader.tmode = textMode;
    }

    void setTextMode(bool textmode)
    {
        textMode = textmode;
        writer.reset(textMode);
        reader.tmode = textMode;
    }

    ConnectionWriter& getCleanWriter()
    {
        writer.reset(textMode);
        return writer;
    }

    ConnectionWriter& getWriter()
    {
        return writer;
    }

    ConnectionReader& getReader(ConnectionWriter* replywriter)
    {
        writer.stopWrite();
        std::string s = writer.toString();
        sis.reset();
        sis.add(s);
        Route r;
        reader.reset(sis, nullptr, r, s.length(), textMode);
        reader.altWriter = ((replywriter != nullptr) ? replywriter : &writer);
        return reader;
    }

    void setReplyWriter(ConnectionWriter* writer)
    {
        reader.altWriter = writer;
    }

    void reset()
    {
        writer.reset(textMode);
    }
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

DummyConnector2::DummyConnector2() :
        mPriv(new Private())
{
}

DummyConnector2::~DummyConnector2()
{
    delete mPriv;
}

void DummyConnector2::setTextMode(bool textmode)
{
    mPriv->setTextMode(textmode);
}

ConnectionWriter& DummyConnector2::getCleanWriter()
{
    return mPriv->getCleanWriter();
}

ConnectionWriter& DummyConnector2::getWriter()
{
    return mPriv->getWriter();
}

ConnectionReader& DummyConnector2::getReader(ConnectionWriter* replywriter)
{
    return mPriv->getReader(replywriter);
}

void DummyConnector2::reset()
{
    mPriv->reset();
}
