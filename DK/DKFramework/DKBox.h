//
//  File: DKBox.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2016 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../DKFoundation.h"
#include "DKVector3.h"
#include "DKQuaternion.h"

#pragma pack(push, 4)
namespace DKFramework
{
	class DKLine;
	/// @brief Oriented bounding box, a utility class.
	class DKGL_API DKBox
	{
	public:
		DKBox();
		/// c,x,y,z is absolute position value.
		DKBox(const DKVector3& c, const DKVector3& x, const DKVector3& y, const DKVector3& z);
		/// c: position of center
		/// u,v,w: unit vector of box direction.
		/// hu, hv, hw: box extent
		DKBox(const DKVector3& c, const DKVector3& u, const DKVector3& v, float hu, float hv, float hw);
		DKBox(const DKVector3& c, const DKVector3& u, const DKVector3& v, const DKVector3& w, float hu, float hv, float hw);
		/// box from matrix
		DKBox(const DKMatrix4& m);

		bool IsValid() const;
		bool IsPointInside(const DKVector3& pos) const;

		float Volume() const;
		DKMatrix4 LocalTransform() const;  ///< transform of box
		DKMatrix4 AffineTransform() const; ///< affine transform of box
		DKQuaternion Orientation() const;  ///< orientation of box
		DKBox& RotateOrientation(const DKQuaternion& q);

		bool RayTest(const DKLine& ray, DKVector3* hitPoint = NULL) const;

		DKVector3 center;
		DKVector3 u,v,w;	///< unit vectors of each extent direction.
		float hu, hv, hw;
	};
}
#pragma pack(pop)
