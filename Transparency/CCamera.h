#ifndef _CCamera_HG_
#define _CCamera_HG_

#include <Windows.h>
//xnamath.h needs windows.h before
#include <xnamath.h>


/** 
 * @brief CCamera first person camera class.
 * @file
 * @author  Adapted from Frank Luna's book.
 * @version 1.0
 *
 * @section LICENSE
 *
 * All rights are reserved. 
 *
 *
 * @section DESCRIPTION
 *
 * The CCamera class represents a first person camera.
 * This code is based on 3D Game Programming with Direct X 11. 
 */
class CCamera
{
public:
	/**
	* CCamera constructor, initializes CCamera class objects.
	*/
	CCamera();
	/**
	* CCamera destructor.
	*/
	virtual ~CCamera();

	// Get/Set world camera position.
	XMVECTOR	GetPositionXM()const;					///< Returns camera position.
	XMFLOAT3	GetPosition()const;						///< Returns camera position.
	void		SetPosition(float x, float y, float z);	///< Sets camera position.
	void		SetPosition(const XMFLOAT3& v);			///< Sets camera position.
	
	// Get camera basis vectors.
	XMVECTOR	GetRightXM()const;	///< Get right.
	XMFLOAT3	GetRight()const;	///< Get right.
	XMVECTOR	GetUpXM()const;		///< Get up.
	XMFLOAT3	GetUp()const;		///< Get up.
	XMVECTOR	GetLookXM()const;	///< Get look.
	XMFLOAT3	GetLook()const;		///< Get look.

	// Get frustum properties.
	float		GetNearZ()const;	///< Get near.
	float		GetFarZ()const;		///< Get far.
	float		GetAspect()const;	///< Get aspect.
	float		GetFovY()const;		///< Get fov.
	float		GetFovX()const;		///< Get fov.

	// Get near and far plane dimensions in view space coordinates.
	float		GetNearWindowWidth()const;	///< Get near window width.
	float		GetNearWindowHeight()const;	///< Get near window height.
	float		GetFarWindowWidth()const;	///< Get far window width.
	float		GetFarWindowHeight()const; ///< Get window height.
	
	XMFLOAT2 mLastMousePos;

	/**
	* Set frustum.
	* @param fovY
	* @param aspect
	* @param zn
	* @param zf
	*/
	void SetLens(float fovY, float aspect, float zn, float zf); ///< Set frustum.

	
	/**
	* Define camera space via LookAt parameters.
	* @param pos camera position.
	* @param target camera target.
	* @param worldUp world up vector.
	*/
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp); ///< Define camera space via LookAt parameters.
	/**
	* Define camera space via LookAt parameters.
	* @param pos camera position.
	* @param target camera target.
	* @param worldUp world up vector.
	*/
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);	///< Define camera space via LookAt parameters.

	// Get View/Proj matrices.
	XMMATRIX View()const;	///< Get View matrix.
	XMMATRIX Proj()const;	///< Get Projection matrix.
	XMMATRIX ViewProj()const;	///< Get View / Projection matrix.

	// Strafe/Walk the camera a distance d.
	void Strafe(float d); ///< Strafe camera.
	void Walk(float d);		///< Walk camera.

	// Rotate the camera.
	void Pitch(float angle);	///< Rotate the camera.
	void RotateY(float angle);	///< Rotate the camera.

	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();	///< After modifying camera position/orientation, call to rebuild the view matrix.

private:

	// Camera coordinate system with coordinates relative to world space.
	XMFLOAT3 mPosition;		///< Position.
	XMFLOAT3 mRight;		///< Right.
	XMFLOAT3 mUp;			///< Up.
	XMFLOAT3 mLook;			///< Look at.

	// Cache frustum properties.
	float mNearZ;		///< Near Z.
	float mFarZ;		///< Far Z.
	float mAspect;		///< Aspect.
	float mFovY;		///< FovY.
	float mNearWindowHeight;	///< Near window height.
	float mFarWindowHeight;		///< Far window height.

	// Cache View/Proj matrices.
	XMFLOAT4X4 mView;	///< View matrix.
	XMFLOAT4X4 mProj;	///< Projection matrix.

};
#endif