#include <windows.h>
#include <Fl/gl.h>
#include <gl/glu.h>

#include "camera.h"

#pragma warning(push)
#pragma warning(disable : 4244)

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif 

const float kMouseRotationSensitivity		= 1.0f/90.0f;
const float kMouseTranslationXSensitivity	= 0.03f;
const float kMouseTranslationYSensitivity	= 0.03f;
const float kMouseZoomSensitivity			= 0.08f;

void MakeDiagonal(Mat4f &m, float k)
{
	register int i,j;

	for (i=0; i<4; i++)
		for (j=0; j<4; j++)
			m[i][j] = (i==j) ? k : 0.0f;
}

void MakeHScale(Mat4f &m, const Vec3f &s)	
{
	MakeDiagonal(m,1.0f);
	m[0][0] = s[0]; m[1][1] = s[1];	m[2][2] = s[2];
}

void MakeHTrans(Mat4f &m, const Vec3f &s)
{
	MakeDiagonal(m,1.0f);
	m[0][3] = s[0]; m[1][3] = s[1]; m[2][3] = s[2];
}

void MakeHRotX(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[1][1] = cosTheta;
	m[1][2] = -sinTheta;
	m[2][1] = sinTheta;
	m[2][2] = cosTheta;
}

void MakeHRotY(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[2][0] = -sinTheta;
	m[0][2] = sinTheta;
	m[2][2] = cosTheta;
}

void MakeHRotZ(Mat4f &m, float theta)
{
	MakeDiagonal(m,1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[0][1] = -sinTheta;
	m[1][0] = sinTheta;
	m[1][1] = cosTheta;
}


void Camera::calculateViewingTransformParameters() 
{
	Mat4f dollyXform;
	Mat4f azimXform;
	Mat4f elevXform;
	Mat4f twistXform;
	Mat4f originXform;

	Vec3f upVector;

	MakeHTrans(dollyXform, Vec3f(0,0,mDolly));
	MakeHRotY(azimXform, mAzimuth);
	MakeHRotX(elevXform, mElevation);
	MakeDiagonal(twistXform, 1.0f);
	MakeHTrans(originXform, mLookAt);
	
	mPosition = Vec3f(0,0,0);
	// grouped for (mat4 * vec3) ops instead of (mat4 * mat4) ops
	mPosition = originXform * (azimXform * (elevXform * (dollyXform * mPosition)));

	if ( fmod((double)mElevation, 2.0*M_PI) < 3*M_PI/2 && fmod((double)mElevation, 2.0*M_PI) > M_PI/2 )
		mUpVector= Vec3f(0,-1,0);
	else
		mUpVector= Vec3f(0,1,0);

	mDirtyTransform = false;
}

Camera::Camera() 
{
	mElevation = mAzimuth = mTwist = 0.0f;
	mDolly = -20.0f;
	mElevation = 0.2f;
	mAzimuth = (float)M_PI;

	mLookAt = Vec3f( 0, 0, 0 );
	mCurrentMouseAction = kActionNone;

	calculateViewingTransformParameters();
}

void Camera::clickMouse( MouseAction_t action, int x, int y )
{
	mCurrentMouseAction = action;
	mLastMousePosition[0] = x;
	mLastMousePosition[1] = y;
}

void Camera::dragMouse( int x, int y )
{
	Vec3f mouseDelta   = Vec3f(x,y,0.0f) - mLastMousePosition;
	mLastMousePosition = Vec3f(x,y,0.0f);

	switch(mCurrentMouseAction)
	{
	case kActionTranslate:
		{
			calculateViewingTransformParameters();

			double xTrack =  -mouseDelta[0] * kMouseTranslationXSensitivity;
			double yTrack =  mouseDelta[1] * kMouseTranslationYSensitivity;

			Vec3f transXAxis = mUpVector ^ (mPosition - mLookAt);
			transXAxis /= sqrt((transXAxis*transXAxis));
			Vec3f transYAxis = (mPosition - mLookAt) ^ transXAxis;
			transYAxis /= sqrt((transYAxis*transYAxis));

			setLookAt(getLookAt() + transXAxis*xTrack + transYAxis*yTrack);
			
			break;
		}
	case kActionRotate:
		{
			float dAzimuth		=   -mouseDelta[0] * kMouseRotationSensitivity;
			float dElevation	=   mouseDelta[1] * kMouseRotationSensitivity;
			
			setAzimuth(getAzimuth() + dAzimuth);
			setElevation(getElevation() + dElevation);
			
			break;
		}
	case kActionZoom:
		{
			float dDolly = -mouseDelta[1] * kMouseZoomSensitivity;
			setDolly(getDolly() + dDolly);
			break;
		}
	case kActionTwist:
		// Not implemented
	default:
		break;
	}

}

void Camera::releaseMouse( int x, int y )
{
	mCurrentMouseAction = kActionNone;
}

#define RAD2DEG(x) (x*360/2/M_PI)
void Camera::applyViewingTransform() {
	if (mDirtyTransform)
		calculateViewingTransformParameters();

	// Place the camera at mPosition, aim the camera at
	// mLookAt, and twist the camera such that mUpVector is up
	//gluLookAt(	mPosition[0], mPosition[1], mPosition[2],
	//			mLookAt[0],   mLookAt[1],   mLookAt[2],
	//			mUpVector[0], mUpVector[1], mUpVector[2]);

	lookAt(mPosition, mLookAt, mUpVector);
}

void Camera::lookAt(double *eye, double *at, double *up) {
	Vec3d m_eye, m_at, m_up;
	for (int i = 0; i<3; i++) {
		m_eye[i] = eye[i];
		m_at[i] = at[i];
		m_up[i] = up[i];
	}

	//Viewpoint coordinate system
	Vec3d vx, vy, vz;
	vz = (m_eye - m_at);
	vz.normalize();

	vx = (m_up.cross(vz));
	vx.normalize();

	vy = vz.cross(vx);

	printf("%f %f %f\n", eye[0], eye[1], eye[2]);
	printf("%f %f %f\n", at[0], at[1], at[2]);
	cout << "VX: " << vx << endl;
	cout << "VY: " << vy << endl;;
	cout << "VZ: " << vz << endl;;

	double change_basis_mat[16];

	change_basis_mat[0] = vx[0];
	change_basis_mat[1] = vy[0];
	change_basis_mat[2] = vz[0];
	change_basis_mat[3] = 0;

	change_basis_mat[4] = vx[1];
	change_basis_mat[5] = vy[1];
	change_basis_mat[6] = vz[1];
	change_basis_mat[7] = 0;

	change_basis_mat[8] = vx[2];
	change_basis_mat[9] = vy[2];
	change_basis_mat[10] = vz[2];
	change_basis_mat[11] = 0;

	change_basis_mat[12] = 0;
	change_basis_mat[13] = 0;
	change_basis_mat[14] = 0;
	change_basis_mat[15] = 1;

	glMultMatrixd(change_basis_mat);

	glTranslated(-eye[0], -eye[1], -eye[2]);
	glTranslated(0, -4, -20);
}

void Camera::lookAt(Vec3f eye, Vec3f at, Vec3f up) {
	double _eye[3], _at[3], _up[3];
	for (int i = 0; i < 3; i++) {
		_eye[i] = eye[i];
		_at[i] = at[i];
		_up[i] = up[i];
	}
	lookAt(_eye, _at, _up);
}

#pragma warning(pop)