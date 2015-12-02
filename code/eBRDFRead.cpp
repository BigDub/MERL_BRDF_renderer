// Copyright 2005 Mitsubishi Electric Research Laboratories All Rights Reserved.

// Permission to use, copy and modify this software and its documentation without
// fee for educational, research and non-profit purposes, is hereby granted, provided
// that the above copyright notice and the following three paragraphs appear in all copies.

// To request permission to incorporate this software into commercial products contact:
// Vice President of Marketing and Business Development;
// Mitsubishi Electric Research Laboratories (MERL), 201 Broadway, Cambridge, MA 02139 or 
// <license@merl.com>.

// IN NO EVENT SHALL MERL BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL,
// OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND
// ITS DOCUMENTATION, EVEN IF MERL HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

// MERL SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED
// HEREUNDER IS ON AN "AS IS" BASIS, AND MERL HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
// UPDATES, ENHANCEMENTS OR MODIFICATIONS.


#include "stdlib.h"
#include "math.h"
#include <stdio.h>
#include "image.h"
#include "vector3.h"
#include "matrix3.h"
#include <ctime>
#include <cmath>
#include <string>

#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360

#define RED_SCALE (1.0/1500.0)
#define GREEN_SCALE (1.15/1500.0)
#define BLUE_SCALE (1.66/1500.0)
#define PI	3.1415926535897932384626433832795
#define THIRD (1.0/3.0)

// cross product of two vectors
void cross_product (double* v1, double* v2, double* out)
{
	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

// normalize vector
void normalize(double* v)
{
	// normalize
	double len = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0] = v[0] / len;
	v[1] = v[1] / len;
	v[2] = v[2] / len;
}

// rotate vector along one axis
void rotate_vector(double* vector, double* axis, double angle, double* out)
{
	double temp;
	double cross[3];
	double cos_ang = cos(angle);
	double sin_ang = sin(angle);

	out[0] = vector[0] * cos_ang;
	out[1] = vector[1] * cos_ang;
	out[2] = vector[2] * cos_ang;

	temp = axis[0]*vector[0]+axis[1]*vector[1]+axis[2]*vector[2];
	temp = temp*(1.0-cos_ang);

	out[0] += axis[0] * temp;
	out[1] += axis[1] * temp;
	out[2] += axis[2] * temp;

	cross_product (axis,vector,cross);

	out[0] += cross[0] * sin_ang;
	out[1] += cross[1] * sin_ang;
	out[2] += cross[2] * sin_ang;
}


// convert standard coordinates to half vector/difference vector coordinates
void std_coords_to_half_diff_coords(double theta_in, double fi_in, double theta_out, double fi_out,
								double& theta_half,double& fi_half,double& theta_diff,double& fi_diff )
{

	// compute in vector
	double in_vec_z = cos(theta_in);
	double proj_in_vec = sin(theta_in);
	double in_vec_x = proj_in_vec*cos(fi_in);
	double in_vec_y = proj_in_vec*sin(fi_in);
	double in[3]= {in_vec_x,in_vec_y,in_vec_z};
	normalize(in);


	// compute out vector
	double out_vec_z = cos(theta_out);
	double proj_out_vec = sin(theta_out);
	double out_vec_x = proj_out_vec*cos(fi_out);
	double out_vec_y = proj_out_vec*sin(fi_out);
	double out[3]= {out_vec_x,out_vec_y,out_vec_z};
	normalize(out);


	// compute halfway vector
	double half_x = (in_vec_x + out_vec_x)/2.0f;
	double half_y = (in_vec_y + out_vec_y)/2.0f;
	double half_z = (in_vec_z + out_vec_z)/2.0f;
	double half[3] = {half_x,half_y,half_z};
	normalize(half);

	// compute  theta_half, fi_half
	theta_half = acos(half[2]);
	fi_half = atan2(half[1], half[0]);


	double bi_normal[3] = {0.0, 1.0, 0.0};
	double normal[3] = { 0.0, 0.0, 1.0 };
	double temp[3];
	double diff[3];

	// compute diff vector
	rotate_vector(in, normal , -fi_half, temp);
	rotate_vector(temp, bi_normal, -theta_half, diff);

	// compute  theta_diff, fi_diff
	theta_diff = acos(diff[2]);
	fi_diff = atan2(diff[1], diff[0]);

}


// Lookup theta_half index
// This is a non-linear mapping!
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int theta_half_index(double theta_half)
{
	if (theta_half <= 0.0)
		return 0;
	double theta_half_deg = ((theta_half / (PI/2.0))*BRDF_SAMPLING_RES_THETA_H);
	double temp = theta_half_deg*BRDF_SAMPLING_RES_THETA_H;
	temp = sqrt(temp);
	int ret_val = (int)temp;
	if (ret_val < 0) ret_val = 0;
	if (ret_val >= BRDF_SAMPLING_RES_THETA_H)
		ret_val = BRDF_SAMPLING_RES_THETA_H-1;
	return ret_val;
}


// Lookup theta_diff index
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int theta_diff_index(double theta_diff)
{
	int tmp = int(theta_diff / (PI * 0.5) * BRDF_SAMPLING_RES_THETA_D);
	if (tmp < 0)
		return 0;
	else if (tmp < BRDF_SAMPLING_RES_THETA_D - 1)
		return tmp;
	else
		return BRDF_SAMPLING_RES_THETA_D - 1;
}


// Lookup phi_diff index
inline int phi_diff_index(double phi_diff)
{
	// Because of reciprocity, the BRDF is unchanged under
	// phi_diff -> phi_diff + PI
	if (phi_diff < 0.0)
		phi_diff += PI;

	// In: phi_diff in [0 .. pi]
	// Out: tmp in [0 .. 179]
	int tmp = int(phi_diff / PI * BRDF_SAMPLING_RES_PHI_D / 2);
	if (tmp < 0)	
		return 0;
	else if (tmp < BRDF_SAMPLING_RES_PHI_D / 2 - 1)
		return tmp;
	else
		return BRDF_SAMPLING_RES_PHI_D / 2 - 1;
}


// Given a pair of incoming/outgoing angles, look up the BRDF.
int lookup_brdf_val(double* brdf, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double& red_val,double& green_val,double& blue_val)
{
	// Convert to halfangle / difference angle coordinates
	double theta_half, fi_half, theta_diff, fi_diff;
	
	std_coords_to_half_diff_coords(theta_in, fi_in, theta_out, fi_out,
		       theta_half, fi_half, theta_diff, fi_diff);


	// Find index.
	// Note that phi_half is ignored, since isotropic BRDFs are assumed
	int ind = phi_diff_index(fi_diff) +
		  theta_diff_index(theta_diff) * BRDF_SAMPLING_RES_PHI_D / 2 +
		  theta_half_index(theta_half) * BRDF_SAMPLING_RES_PHI_D / 2 *
					         BRDF_SAMPLING_RES_THETA_D;

	red_val = brdf[ind] * RED_SCALE;
	green_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D/2] * GREEN_SCALE;
	blue_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D] * BLUE_SCALE;

	
	if (red_val < 0.0 || green_val < 0.0 || blue_val < 0.0)
	{
		//fprintf(stderr, "Below horizon.\n");
		return 0;
	}
	return 1;
}
// Given a pair of incoming/outgoing angles, look up the BRDF.
int lookup_aniso_brdf_val(double* brdf1, double* brdf2, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double& red_val,double& green_val,double& blue_val)
{
	// Convert to halfangle / difference angle coordinates
	double theta_half, fi_half, theta_diff, fi_diff;
	
	std_coords_to_half_diff_coords(theta_in, fi_in, theta_out, fi_out,
		       theta_half, fi_half, theta_diff, fi_diff);


	// Find index.
	// Note that phi_half is ignored, since isotropic BRDFs are assumed
	int ind = phi_diff_index(fi_diff) +
		  theta_diff_index(theta_diff) * BRDF_SAMPLING_RES_PHI_D / 2 +
		  theta_half_index(theta_half) * BRDF_SAMPLING_RES_PHI_D / 2 *
					         BRDF_SAMPLING_RES_THETA_D;
	double mix = 0.5 * (sin(2 * fi_half) + 1.0);
	double red1 = 0;
	double red2 = 0;
	double green1 = 0;
	double green2 = 0;
	double blue1 = 0;
	double blue2 = 0;

	red1 = brdf1[ind] * RED_SCALE;
	green1 = brdf1[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D/2] * GREEN_SCALE;
	blue1 = brdf1[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D] * BLUE_SCALE;

	if (red1 < 0.0 || green1 < 0.0 || blue1 < 0.0)
	{
		//fprintf(stderr, "Below horizon.\n");
		return 0;
	}

	red2 = brdf2[ind] * RED_SCALE;
	green2 = brdf2[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D/2] * GREEN_SCALE;
	blue2 = brdf2[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D] * BLUE_SCALE;

	if (red2 < 0.0 || green2 < 0.0 || blue2 < 0.0)
	{
		//fprintf(stderr, "Below horizon.\n");
		return 0;
	}

	red_val = mix * red1 + (1 - mix) * red2;
	green_val = mix * green1 + (1 - mix) * green2;
	blue_val = mix * blue1 + (1 - mix) * blue2;
	
	return 1;
}



// Read BRDF data
bool read_brdf(const char *filename, double* &brdf)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		return false;

	int dims[3];
	fread(dims, sizeof(int), 3, f);
	int n = dims[0] * dims[1] * dims[2];
	if (n != BRDF_SAMPLING_RES_THETA_H *
		 BRDF_SAMPLING_RES_THETA_D *
		 BRDF_SAMPLING_RES_PHI_D / 2) 
	{
		fprintf(stderr, "Dimensions don't match\n");
		fclose(f);
		return false;
	}

	brdf = (double*) malloc (sizeof(double)*3*n);
	fread(brdf, sizeof(double), 3*n, f);

	fclose(f);
	return true;
}

int ray_sphere_intersection(Vector3 center, double radius, Vector3 origin, Vector3 direction, Vector3& intersection, double& distance)
{
	Vector3 difference = center - origin;
	// The distance along the ray, from the origin, to a line perpendicular to `difference` that goes through the center of the sphere.
	double d2pl = difference.dot_product(direction);

	// If this is negative the intersection is behind the origin and is of no interest.
	if (d2pl < 0)
	{
		return 0;
	}

	// This is the squared length along the perpendicular line to the intersetion with the ray.
	double length = difference.dot_product(difference) - d2pl * d2pl;

	double radius_square = radius * radius;

	// If this is a greater distance than the radius squared it does not intersect with the sphere.
	if (length > radius_square)
	{
		return 0;
	}

	// This is the offset from `d2pl` along the ray where the insection(s) occur
	// Found the same way as `length` using pythagorean theorem
	double offset = sqrt(radius_square - length);

	// I am only interested in the closer of the two intersections.
	distance = d2pl - offset;

	intersection = origin + (direction * distance);
	return 1;
}

int normal_tangent(Vector3 normal, Vector3& tangent, Vector3& bitangent)
{
	double angle = atan2(normal.x, normal.z) - PI / 2;
	tangent = Vector3(
			sin(angle),
			0,
			cos(angle)
		);
	bitangent = normal.cross_product(tangent);
	//fprintf(stdout, "Normal(%f, %f, %f)\tTangent(%f, %f, %f)\tBiTangent(%f, %f, %f)\n", normal.x, normal.y, normal.z, tangent.x, tangent.y, tangent.z, bitangent.x, bitangent.y, bitangent.z);
	return 0;
}

#define NUM_LIGHTS 1

int main(int argc, char *argv[])
{
	int img_size;
	int anim_time;
	int fps;
	char *infilename1;
	char *infilename2;
	char *outfilename;
	try
	{
		if (argc < 7)
		{
			throw std::exception();
		}
		img_size = atoi(argv[1]);
		anim_time = atoi(argv[2]);
		fps = atoi(argv[3]);
		infilename1 = argv[4];
		infilename2 = argv[5];
		outfilename = argv[6];
	}
	catch (std::exception const& e)
	{
		fprintf(stdout, "USAGE: size, time, fps, brdf, output\n"
			"\tsize:\tThe width and height of the output images.\n"
			"\ttime:\tThe duration of the animation.\n"
			"\tfps:\tFrames per second of the animation.\n"
			"\tbrdf:\tFilename of the brdf to use.\n"
			"\tbrdf:\tFilename of the brdf to use.\n"
			"\toutput:\tBase filename for the output (excluding extension).\n");
		exit(1);
	}
	double* brdf1;
	double* brdf2;

	// read brdf
	if (!read_brdf(infilename1, brdf1))
	{
		fprintf(stderr, "Error reading %s\n", infilename1);
		exit(1);
	}
	if (!read_brdf(infilename2, brdf2))
	{
		fprintf(stderr, "Error reading %s\n", infilename2);
		exit(1);
	}

	Vector3 camera = Vector3(0,0,-2.5);
	Vector3 lightPositions[] = {
		Vector3(5, 5, -5),
		Vector3(-5, -5, 5)
	};
	Vector3 lightColors[] = {
		Vector3(25, 25, 25) * 255,
		Vector3(10, 10, 15) * 255
	};
	Vector3 sphere = Vector3(0);
	double radius = 1;

	int num_images = anim_time * fps;

	for (int image_number = 0; image_number < num_images; image_number++) {
		fprintf(stdout, "\rProcessing image %03i/%03i...", (image_number + 1), num_images);
		fflush(stdout);
		double percent = (double)image_number / num_images;
		double angle = percent * 2 * PI;
		
		lightPositions[0] = Vector3(sin(angle), cos(angle), -0.5) * 5;

		//lightColors[0] = Vector3(25, 25, 25) * percent * 255.0;
		//lightColors[1] = Vector3(10, 10, 15) * percent * 255.0;

		Image image = Image(img_size);

		for (int x = 0; x < img_size; x++)
		{
			for (int y = 0; y < img_size; y++)
			{
				double xDir = (2*((double)x / (double)img_size) - 1) * (radius * 1.25);
				double yDir = (-2*((double)y / (double)img_size) + 1) * (radius * 1.25);
				Vector3 viewDir = Vector3(xDir, yDir, 0) - camera;
				viewDir.normalize();
				Vector3 intersection = Vector3(0);
				double distance = 0;
				if (ray_sphere_intersection(sphere, radius, camera, viewDir, intersection, distance))
				{
					Vector3 surface = (intersection - sphere) / radius;
					double red = 0;
					double green = 0;
					double blue = 0;
					Vector3 normal = surface.normal();
					Vector3 toView = -viewDir;
					for (int light_index = 0; light_index < NUM_LIGHTS; light_index++) {
						Vector3 toLight = (lightPositions[light_index] - intersection).normal();

						// Only process points that face the light
						if (!normal.dot_product(toLight) <= 0)
						{
							double theta_out = normal.angle_between(toView);
							double theta_in = normal.angle_between(toLight);

							Vector3 tangent;
							Vector3 bitangent;
							normal_tangent(normal, tangent, bitangent);

							Matrix3 worldToTangent = Matrix3(tangent, normal, bitangent).inverse();

							Vector3 out = worldToTangent * toView;
							Vector3 in = worldToTangent * toLight;

							double phi_out = atan2(out.z, out.x);

							double phi_in = atan2(in.z, in.x);

							lookup_aniso_brdf_val(brdf1, brdf2,
								theta_in, phi_in,
								theta_out, phi_out,
								red, green, blue);

							red *= lightColors[light_index].x;
							green *= lightColors[light_index].y;
							blue *= lightColors[light_index].z;
						}
					}

					image.set(x, y, Pixel(red, green, blue));
				}
			}
		}

		char* filename = new char[50];
		sprintf(filename, "%s%04i.bmp", outfilename, image_number);
		image.save(filename);
		delete filename;
	}
	fprintf(stdout, " Done.\n");
	return 0;
}
