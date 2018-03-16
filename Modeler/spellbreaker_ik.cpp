#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "modelerui.h"
#include <FL/gl.h>

#include "spellbreaker_globals.h"
#include "modelerapp.h"

#include <math.h>

void static MakeDiagonal(Mat4f &m, float k)
{
	register int i, j;

	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
			m[i][j] = (i == j) ? k : 0.0f;
}

void static MakeHScale(Mat4f &m, const Vec3f &s)
{
	MakeDiagonal(m, 1.0f);
	m[0][0] = s[0]; m[1][1] = s[1];	m[2][2] = s[2];
}

void static MakeHTrans(Mat4f &m, const Vec3f &s)
{
	MakeDiagonal(m, 1.0f);
	m[0][3] = s[0]; m[1][3] = s[1]; m[2][3] = s[2];
}

void static MakeHRotX(Mat4f &m, float theta)
{
	MakeDiagonal(m, 1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[1][1] = cosTheta;
	m[1][2] = -sinTheta;
	m[2][1] = sinTheta;
	m[2][2] = cosTheta;
}

void static MakeHRotY(Mat4f &m, float theta)
{
	MakeDiagonal(m, 1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[2][0] = -sinTheta;
	m[0][2] = sinTheta;
	m[2][2] = cosTheta;
}

void static  MakeHRotZ(Mat4f &m, float theta)
{
	MakeDiagonal(m, 1.0f);
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);
	m[0][0] = cosTheta;
	m[0][1] = -sinTheta;
	m[1][0] = sinTheta;
	m[1][1] = cosTheta;
}

inline double DEG2RAD(double deg) {
	return deg * 2 * M_PI / 360;
}

void SpellBreaker::doIK() {
	
}

void SpellBreaker::doFK() {
	//Make the elbow thing
	Mat4f elbow_trans, elbow_rot;
	MakeHTrans(elbow_trans, Vec3f(0, -upper_arm_len, 0));
	MakeHRotZ(elbow_rot, DEG2RAD(right_upper_arm_rise));

	ik_elbow_pos = elbow_rot * elbow_trans * Vec3f(0, 0, 0);

	//Make the hand thing
	Mat4f hand_trans, hand_rot;
	MakeHTrans(hand_trans, Vec3f(0, -lower_arm_len, 0));
	MakeHRotX(hand_rot, DEG2RAD(right_lower_arm_rise));

	ik_hand_pos = elbow_rot * elbow_trans * hand_rot * hand_trans * Vec3f(0, 0, 0);

	cout << "FK Elbow: " << ik_elbow_pos << endl;
	cout << "FK Hand: " << ik_hand_pos << endl << endl;
}
