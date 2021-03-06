//
//  File: DKMatrix2.cpp
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2016 Hongtae Kim. All rights reserved.
//

#include "DKMath.h"
#include "DKMatrix2.h"
#include "DKVector2.h"

using namespace DKFramework;

const DKMatrix2 DKMatrix2::identity = DKMatrix2().SetIdentity();

DKMatrix2::DKMatrix2()
	: _11(1), _12(0)
	, _21(0), _22(1)
{
}

DKMatrix2::DKMatrix2(const DKVector2& row1, const DKVector2& row2)
	: _11(row1.x), _12(row1.y)
	, _21(row2.x), _22(row2.y)
{
}

DKMatrix2::DKMatrix2(float e11, float e12, float e21, float e22)
	: _11(e11), _12(e12)
	, _21(e21), _22(e22)
{
}

DKMatrix2& DKMatrix2::SetIdentity()
{
	m[0][0] = m[1][1] = 1.0f;
	m[0][1] = m[1][0] = 0.0f;
	return *this;
}

bool DKMatrix2::IsIdentity() const
{
	return
		m[0][0] == 1.0f && m[0][1] == 0.0f &&
		m[1][0] == 0.0f && m[1][1] == 1.0f;
}

bool DKMatrix2::IsDiagonal() const
{
	return m[0][1] == 0.0f && m[1][0] == 0.0f;
}

float DKMatrix2::Determinant() const
{
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

DKMatrix2 DKMatrix2::InverseMatrix(bool* r, float* d) const
{
    DKMatrix2 mat;
    float det = Determinant();
    bool result = false;

    if (det != 0.0f)
    {
        float detInv = 1.0f / det;

        mat.m[0][0] = m[1][1] * detInv;
        mat.m[0][1] = -m[0][1] * detInv;
        mat.m[1][0] = -m[1][0] * detInv;
        mat.m[1][1] = m[0][0] * detInv;

        result = true;
        if (d)
            *d = det;
    }
    else
        mat.SetIdentity();
    if (r)
        *r = result;
    return mat;
}

DKMatrix2 DKMatrix2::TransposeMatrix() const
{
    DKMatrix2 mat(*this);
    return mat.Transpose();
}

DKMatrix2& DKMatrix2::Inverse(bool* r)
{
    bool b;
    DKMatrix2 mat = this->InverseMatrix(&b, nullptr);
    if (b)
        *this = mat;
    if (r)
        *r = b;
    else
    {
#ifdef DKGL_DEBUG_ENABLED
        if (!b)
            DKLogE("DKMatrix2::Inverse failed");
#endif
    }
    return *this;
}

DKMatrix2& DKMatrix2::Transpose()
{
	float tmp = this->m[0][1];
	this->m[0][1] = this->m[1][0];
	this->m[1][0] = tmp;
	return *this;
}

DKMatrix2& DKMatrix2::Multiply(const DKMatrix2& m)
{
	DKMatrix2 mat(*this);
	this->m[0][0] = (mat.m[0][0] * m.m[0][0]) + (mat.m[0][1] * m.m[1][0]);
	this->m[0][1] = (mat.m[0][0] * m.m[0][1]) + (mat.m[0][1] * m.m[1][1]);
	this->m[1][0] = (mat.m[1][0] * m.m[0][0]) + (mat.m[1][1] * m.m[1][0]);
	this->m[1][1] = (mat.m[1][0] * m.m[0][1]) + (mat.m[1][1] * m.m[1][1]);
	return *this;
}

DKMatrix2 DKMatrix2::operator * (const DKMatrix2& m) const
{
	DKMatrix2 mat;
	mat.m[0][0] = (this->m[0][0] * m.m[0][0]) + (this->m[0][1] * m.m[1][0]);
	mat.m[0][1] = (this->m[0][0] * m.m[0][1]) + (this->m[0][1] * m.m[1][1]);
	mat.m[1][0] = (this->m[1][0] * m.m[0][0]) + (this->m[1][1] * m.m[1][0]);
	mat.m[1][1] = (this->m[1][0] * m.m[0][1]) + (this->m[1][1] * m.m[1][1]);
	return mat;
}

DKMatrix2 DKMatrix2::operator + (const DKMatrix2& m) const
{
	DKMatrix2 mat;
	mat.val[0] = this->val[0] + m.val[0];
	mat.val[1] = this->val[1] + m.val[1];
	mat.val[2] = this->val[2] + m.val[2];
	mat.val[3] = this->val[3] + m.val[3];
	return mat;
}

DKMatrix2 DKMatrix2::operator - (const DKMatrix2& m) const
{
	DKMatrix2 mat;
	mat.val[0] = this->val[0] - m.val[0];
	mat.val[1] = this->val[1] - m.val[1];
	mat.val[2] = this->val[2] - m.val[2];
	mat.val[3] = this->val[3] - m.val[3];
	return mat;
}

DKMatrix2 DKMatrix2::operator * (float f) const
{
	DKMatrix2 mat;
	mat.val[0] = this->val[0] * f;
	mat.val[1] = this->val[1] * f;
	mat.val[2] = this->val[2] * f;
	mat.val[3] = this->val[3] * f;
	return mat;
}

DKMatrix2 DKMatrix2::operator / (float f) const
{
	float inv = 1.0f / f;
	DKMatrix2 mat;
	mat.val[0] = this->val[0] * inv;
	mat.val[1] = this->val[1] * inv;
	mat.val[2] = this->val[2] * inv;
	mat.val[3] = this->val[3] * inv;
	return mat;
}

DKMatrix2& DKMatrix2::operator *= (const DKMatrix2& m)
{
	DKMatrix2 mat(*this);
	this->m[0][0] = (mat.m[0][0] * m.m[0][0]) + (mat.m[0][1] * m.m[1][0]);
	this->m[0][1] = (mat.m[0][0] * m.m[0][1]) + (mat.m[0][1] * m.m[1][1]);
	this->m[1][0] = (mat.m[1][0] * m.m[0][0]) + (mat.m[1][1] * m.m[1][0]);
	this->m[1][1] = (mat.m[1][0] * m.m[0][1]) + (mat.m[1][1] * m.m[1][1]);
	return *this;
}

DKMatrix2& DKMatrix2::operator += (const DKMatrix2& m)
{
	this->val[0] += m.val[0];
	this->val[1] += m.val[1];
	this->val[2] += m.val[2];
	this->val[3] += m.val[3];
	return *this;
}

DKMatrix2& DKMatrix2::operator -= (const DKMatrix2& m)
{
	this->val[0] -= m.val[0];
	this->val[1] -= m.val[1];
	this->val[2] -= m.val[2];
	this->val[3] -= m.val[3];
	return *this;
}

DKMatrix2& DKMatrix2::operator *= (float f)
{
	this->val[0] *= f;
	this->val[1] *= f;
	this->val[2] *= f;
	this->val[3] *= f;
	return *this;
}

DKMatrix2& DKMatrix2::operator /= (float f)
{
	float inv = 1.0f / f;
	this->val[0] *= inv;
	this->val[1] *= inv;
	this->val[2] *= inv;
	this->val[3] *= inv;
	return *this;
}

bool DKMatrix2::operator == (const DKMatrix2& m) const
{
	return
		this->val[0] == m.val[0] && this->val[1] == m.val[1] &&
		this->val[2] == m.val[2] && this->val[3] == m.val[3];
}

bool DKMatrix2::operator != (const DKMatrix2& m) const
{
	return
		this->val[0] != m.val[0] || this->val[1] != m.val[1] ||
		this->val[2] != m.val[2] || this->val[3] != m.val[3];
}

DKVector2 DKMatrix2::Row1() const
{
	return DKVector2(m[0][0], m[0][1]);
}

DKVector2 DKMatrix2::Row2() const
{
	return DKVector2(m[1][0], m[1][1]);
}

DKVector2 DKMatrix2::Column1() const
{
	return DKVector2(m[0][0], m[1][0]);
}

DKVector2 DKMatrix2::Column2() const
{
	return DKVector2(m[0][1], m[1][1]);
}
