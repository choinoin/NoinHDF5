/*
 * =====================================================================================
 *
 *       Filename:  NoinHDF5_test.c
 *
 *    Description:  
 *
 *            ===================================================================
 *            |         Section (or function) Name           Line Number        |
 *            | ####  --------------+---------------||-----#####  ~  #####----- |
 *            |                                     ||                          |
 *            | 0000                                ||     00000  ~  00000      |
 *            |                                     ||                          |
 *            | ####  ------------------------------||-----#####  ~  #####----- |
 *            |                                                                 |
 *            ===================================================================
 *
 *
 *        Version:  1.0
 *        Created:  02/19/2021 01:48:04 PM
 *       Revision:  none
 *       Compiler:  icc
 *          Flags: -qopt-report=5 -xcore-avx2 -qopenmp (if using gcc -lm)
 *                 -lcudnn -lcublas -lcudart_static -lmpi -lrt -ldl -lX11
 *                 -I/usr/local/cuda/include -I/usr/local/include 
 *                 -L/usr/local/lib64/stubs -L/usr/local/cuda/lib64 -L/usr/local/lib
 *
 *         Author:  choinoin (최인혁, Inhyeok Choi), ihchoi@kangwon.ac.kr
 *   Organization:  Kangwon National University
 *
 * =====================================================================================
 */

#include "../include/NoinHDF5.h"

int main(int argc, char *argv) {

	int i, j, k;

	NoinHDF5_File file;

	NoinHDF5_set_File(&file, "./1d_cnn.h5");

	NoinHDF5_unset_File(&file);

	return 0;
}
