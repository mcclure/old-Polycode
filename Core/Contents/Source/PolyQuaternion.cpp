/*
 *  PolyQuaternion.cpp
 *  TAU
 *
 *  Created by Ivan Safrin on 3/26/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "PolyQuaternion.h"

using namespace Polycode;

Quaternion::Quaternion() : x(0), y(0), z(0), w(1.0f) {

}

Quaternion::~Quaternion() {

}

void Quaternion::createFromAxisAngle(float ax, float ay, float az, float degrees)
{
	float angle = float((degrees / 180.0f) * PI);
	float result = (float)sin( angle / 2.0f );
	w = (float)cos( angle / 2.0f );
	x = float(ax * result);
	y = float(ay * result);
	z = float(az * result);
}

Matrix4 Quaternion::createMatrix()
{
	Matrix4 m;
        float fTx  = 2.0*x;
        float fTy  = 2.0*y;
        float fTz  = 2.0*z;
        float fTwx = fTx*w;
        float fTwy = fTy*w;
        float fTwz = fTz*w;
        float fTxx = fTx*x;
        float fTxy = fTy*x;
        float fTxz = fTz*x;
        float fTyy = fTy*y;
        float fTyz = fTz*y;
        float fTzz = fTz*z;

        m[0][0] = 1.0-(fTyy+fTzz);
        m[1][0] = fTxy-fTwz;
        m[2][0] = fTxz+fTwy;
        m[0][1] = fTxy+fTwz;
        m[1][1] = 1.0-(fTxx+fTzz);
        m[2][1] = fTyz-fTwx;
        m[0][2] = fTxz-fTwy;
        m[1][2] = fTyz+fTwx;
        m[2][2] = 1.0-(fTxx+fTyy);	
	return m;
}

    float Quaternion::Dot (const Quaternion& rkQ) const
    {
        return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
    }

    Quaternion Quaternion::operator* (const Quaternion& rkQ) const
    {
        return Quaternion
        (
            w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
            w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
            w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
            w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
        );
    }

    Quaternion Quaternion::operator* (float fScalar) const
    {
        return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
    }

    Quaternion operator* (float fScalar, const Quaternion& rkQ)
    {
        return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
            fScalar*rkQ.z);
    }
	
    Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
    {
        return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
    }	

    Quaternion Quaternion::Exp () const
    {
        // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
        // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

        float fAngle ( sqrtf(x*x+y*y+z*z) );
        float fSin = sinf(fAngle);

        Quaternion kResult;
        kResult.w = cosf(fAngle);

        if ( fabs(fSin) >= 1e-03 )
        {
            float fCoeff = fSin/fAngle;
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
        }
        else
        {
            kResult.x = x;
            kResult.y = y;
            kResult.z = z;
        }

        return kResult;
    }
	
	Quaternion Quaternion::Inverse () const
    {
        float fNorm = w*w+x*x+y*y+z*z;
        if ( fNorm > 0.0 )
        {
            float fInvNorm = 1.0/fNorm;
            return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
        }
        else
        {
            // return an invalid result to flag the error
            return Quaternion(0,0,0,0);
        }
    }


    Quaternion Quaternion::Log () const
    {
        Quaternion kResult;
        kResult.w = 0.0;

        if ( fabs(w) < 1.0 )
        {
            float fAngle ( acosf(w) );
            float fSin = sinf(fAngle);
            if ( fabs(fSin) >= 1e-03 )
            {
                float fCoeff = fAngle/fSin;
                kResult.x = fCoeff*x;
                kResult.y = fCoeff*y;
                kResult.z = fCoeff*z;
                return kResult;
            }
        }

        kResult.x = x;
        kResult.y = y;
        kResult.z = z;

        return kResult;
    }

    float Quaternion::Norm () const
    {
        return w*w+x*x+y*y+z*z;
    }

    float Quaternion::normalise(void)
    {
        float len = Norm();
        float factor = 1.0f / sqrtf(len);
        *this = *this * factor;
        return len;
    }

    Quaternion Quaternion::Slerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath)
    {
        float fCos = rkP.Dot(rkQ);
        Quaternion rkT;

        // Do we need to invert rotation?
        if (fCos < 0.0f && shortestPath)
        {
            fCos = -fCos;
            rkT = -rkQ;
        }
        else
        {
            rkT = rkQ;
        }

        if (fabs(fCos) < 1 - 1e-03)
        {
            // Standard case (slerp)
            float fSin = sqrtf(1 - (fCos*fCos));
            float fAngle = atan2f(fSin, fCos);
            float fInvSin = 1.0f / fSin;
            float fCoeff0 = sinf((1.0f - fT) * fAngle) * fInvSin;
            float fCoeff1 = sinf(fT * fAngle) * fInvSin;
            return fCoeff0 * rkP + fCoeff1 * rkT;
        }
        else
        {
            // There are two situations:
            // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
            //    interpolation safely.
            // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
            //    are an infinite number of possibilities interpolation. but we haven't
            //    have method to fix this case, so just use linear interpolation here.
            Quaternion t = (1.0f - fT) * rkP + fT * rkT;
            // taking the complement requires renormalisation
            t.normalise();
            return t;
        }
    }
	
    Quaternion Quaternion::Squad (float fT,
        const Quaternion& rkP, const Quaternion& rkA,
        const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath)
    {
        float fSlerpT = 2.0*fT*(1.0-fT);
        Quaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
        Quaternion kSlerpQ = Slerp(fT, rkA, rkB);
        return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
    }	

Quaternion Quaternion::operator *(Quaternion q)
{
	Quaternion r;

	r.w = w*q.w - x*q.x - y*q.y - z*q.z;
	r.x = w*q.x + x*q.w + y*q.z - z*q.y;
	r.y = w*q.y + y*q.w + z*q.x - x*q.z;
	r.z = w*q.z + z*q.w + x*q.y - y*q.x;

	return r;
}