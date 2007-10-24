#ifndef __CN_H__
#define __CN_H__

#include <math.h>
#include <complex>
#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_sort_vector.h>
#include "MOOSLib.h"
using namespace std;

int CN_intersect_func(double x11, double y11, double x22, double y22, double dx, double dy, double r11, double r22, gsl_matrix *sol, gsl_matrix *xy_sol, bool debug);
int CN_intersect_call_func(gsl_matrix *cn,int r1,int r2, double cov_range, double cov_odometry, gsl_matrix *sol,gsl_matrix *xy_sol,gsl_matrix *sigma_xy[2],bool debug);
int CN_compute_transition_likelihood_func(gsl_vector *a,gsl_matrix *cov_a,gsl_vector *b,gsl_matrix *cov_b,gsl_vector *d,gsl_matrix *cov_d,double *trans_l);
void CN_compute_covariances_func(double x11,double y11,double x22,double y22,double r11,double r22,double dx,double dy,gsl_matrix *G,gsl_matrix *sol,gsl_matrix *sigma_xy[2]);
int CN_add_to_cn_table(gsl_matrix **cn,bool *cn_init, int max_length, double my_x, double my_y, double my_cov_xx, double my_cov_xy, double my_cov_yx, double my_cov_yy, double ov_x, double ov_y, double ov_cov_xx, double ov_cov_xy, double ov_cov_yx, double ov_cov_yy, int ov_id, double range, bool debug);
void CN_push_stack_func(gsl_matrix **old_stack,bool *stack_init,gsl_vector *new_vector, bool debug);
int CN_shrink_stack_func(gsl_matrix **old_stack,int new_depth,int sort_column);
void CN_pFramer_to_cn_table(double pos_x, double pos_y, double my_cov_xx, double my_cov_xy, double my_cov_yx, double my_cov_yy, string new_frame, double owt, double speed_of_sound, double dfLatOrigin, double dfLongOrigin, gsl_matrix **cn, bool *cn_init, int max_length, bool debug);

#endif
