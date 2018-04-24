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

inline double bound(double orig, double lower, double upper) {
	if (orig <= lower) return lower;
	if (orig >= upper) return upper;
	return orig;
}

void SpellBreaker::doIK() {
	Vec3f elbow_raw_axis(0, 0, 1.0);
	Vec3f hand_raw_axis(1.0, 0, 0);
	Vec3f staff_raw_axis(0, 1.0, 0);

	Vec3f effector(VAL(IK_STAFF_X), VAL(IK_STAFF_Y), VAL(IK_STAFF_Z));
	Vec3f hand_eff(VAL(IK_HAND_X), VAL(IK_HAND_Y), VAL(IK_HAND_Z));

	double upper_min = VAL(IK_UPPER_MIN_ANGLE);
	double upper_max = VAL(IK_UPPER_MAX_ANGLE);
	double lower_min = VAL(IK_LOWER_MIN_ANGLE);
	double lower_max = VAL(IK_LOWER_MAX_ANGLE);
	double staff_min = VAL(IK_STAFF_MIN_ANGLE);
	double staff_max = VAL(IK_STAFF_MAX_ANGLE);

	for (int i = 0; i < 3000; i++) {
		doFK();

		Mat4f elbow_rot, hand_rot, staff_rot;
		MakeHRotZ(elbow_rot, DEG2RAD(right_upper_arm_rise));
		MakeHRotX(hand_rot, DEG2RAD(right_lower_arm_rise));
		MakeHRotY(staff_rot, DEG2RAD(staff_angle));

		hand_rot = elbow_rot * hand_rot;
		staff_rot = hand_rot * staff_rot;

		Vec3f elbow_axis = elbow_rot * elbow_raw_axis;
		Vec3f hand_axis = hand_rot * hand_raw_axis;
		Vec3f staff_axis = staff_rot * staff_raw_axis;

		Vec3f jac_c1 = elbow_axis.cross(ik_staff_pos);
		Vec3f jac_c2 = hand_axis.cross(ik_staff_pos - ik_elbow_pos);
		Vec3f jac_c3 = staff_axis.cross(ik_staff_pos - ik_hand_pos);

		Vec3f jac2_c1 = elbow_axis.cross(ik_hand_pos);
		Vec3f jac2_c2 = hand_axis.cross(ik_hand_pos - ik_elbow_pos);
		Vec3f jac2_c3(0, 0, 0);

		Mat3f jacob(jac_c1[0], jac_c2[0], jac_c3[0],
			jac_c1[1], jac_c2[1], jac_c3[1],
			jac_c1[2], jac_c2[2], jac_c3[2]);

		Mat3f jacob2(jac2_c1[0], jac2_c2[0], jac2_c3[0],
			jac2_c1[1], jac2_c2[1], jac2_c3[1],
			jac2_c1[2], jac2_c2[2], jac2_c3[2]);

		Vec3f delta_st_tar = effector - ik_staff_pos;
		Vec3f delta_st_deg = jacob.transpose() * delta_st_tar * 0.2;

		Vec3f delta_hd_tar = hand_eff - ik_hand_pos;
		Vec3f delta_hd_deg = jacob2.transpose() * delta_hd_tar * 0.2;

		right_upper_arm_rise += delta_st_deg[0] + delta_hd_deg[0];
		right_lower_arm_rise += delta_st_deg[1] + delta_hd_deg[1];
		staff_angle += delta_st_deg[2] + delta_hd_deg[2];

		right_upper_arm_rise = bound(right_upper_arm_rise, upper_min, upper_max);
		right_lower_arm_rise = bound(right_lower_arm_rise, lower_min, lower_max);
		staff_angle = bound(staff_angle, staff_min, staff_max);

		if (delta_st_tar.length() < 0.01 && delta_hd_tar.length() < 0.01) break;
	}

	//std::cout << "Hand Targ: " << hand_eff << endl;
	//std::cout << "Hand Curr: " << ik_hand_pos << endl << endl;
	//std::cout << "Staff Targ: " << effector << endl;
	//std::cout << "Staff Curr: " << ik_staff_pos << endl << endl << endl;
}

void SpellBreaker::doFK() {
	//Make the elbow thing
	Mat4f elbow_trans, elbow_rot;
	MakeHTrans(elbow_trans, Vec3f(0, -upper_arm_len, 0));
	MakeHRotZ(elbow_rot, DEG2RAD(right_upper_arm_rise));

	ik_elbow_mat = elbow_rot * elbow_trans;
	ik_elbow_pos = ik_elbow_mat * Vec3f(0, 0, 0);
	
	//Make the hand thing
	Mat4f hand_trans, hand_rot;
	MakeHTrans(hand_trans, Vec3f(0, -lower_arm_len, 0));
	MakeHRotX(hand_rot, DEG2RAD(right_lower_arm_rise));

	ik_hand_mat = ik_elbow_mat * hand_rot * hand_trans;
	ik_hand_pos = ik_hand_mat * Vec3f(0, 0, 0);

	//Make the staff things
	Mat4f staff_trans, staff_rot;
	MakeHTrans(staff_trans, Vec3f(0, 0, staff_len));
	MakeHRotY(staff_rot, DEG2RAD(staff_angle));

	ik_staff_mat = ik_hand_mat * staff_rot * staff_trans;
	ik_staff_pos = ik_staff_mat * Vec3f(0, 0, 0);

	//cout << "FK Elbow: " << ik_elbow_pos << endl;
	//cout << "FK Hand: " << ik_hand_pos << endl;
	//cout << "FK Staff: " << ik_staff_pos << endl << endl;
}
