/*
 *  DataTargetQuaternion.h
 *  GaitSymODE
 *
 *  Created by Bill Sellers on Tue July 14 2009.
 *  Copyright (c) 1009 Bill Sellers. All rights reserved.
 *
 */

#ifndef DATATARGETQUATERNION_H
#define DATATARGETQUATERNION_H

#include "DataTarget.h"
#include "PGDMath.h"

class DataTargetQuaternion : public DataTarget
{
public:
    DataTargetQuaternion();
    ~DataTargetQuaternion();

    virtual void SetTargetValues(int size, double *values);
    void SetTargetValues(const char *buf);

    virtual double GetError(int valueListIndex);

    virtual void Dump();

#ifdef USE_OPENGL
    virtual void Draw() {};
#endif

protected:

    pgd::Quaternion *m_ValueList;
    int m_ValueListLength;

};

#endif // DATATARGETQUATERNION_H
