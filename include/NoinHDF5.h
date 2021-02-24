/*
 * =====================================================================================
 *
 *       Filename:  NoinHDF5.h
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
 *        Created:  02/17/2021 01:23:21 PM
 *       Revision:  none
 *       Compiler:  icc
 *          Flags: -qopt-report=5 -xcore-avx2 -qopenmp (if using gcc -lm)
 *                 -lcudnn -lcublas -lcudart_static -lmpi -lrt -ldl
 *                 -I/usr/local/cuda/include -I/usr/local/include 
 *                 -L/usr/local/lib64/stubs -L/usr/local/cuda/lib64 -L/usr/local/lib
 *
 *         Author:  choinoin (최인혁, Inhyeok Choi), ihchoi@kangwon.ac.kr
 *   Organization:  Kangwon National University
 *
 * =====================================================================================
 */

#ifndef NoinHDF5
#define NoinHDF5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hdf5.h>

#include "NoinHDF5_type.h"

#ifdef __cplusplus
extern "C" {
#endif

int NoinHDF5_set_File(NoinHDF5_File *file, char *filename);
int NoinHDF5_set_Group(NoinHDF5_Group *group);
int NoinHDF5_set_Dataset(NoinHDF5_Dataset *dataset);
int NoinHDF5_set_Attr(NoinHDF5_Attribute *attr);

int NoinHDF5_unset_File(NoinHDF5_File *file);
int NoinHDF5_unset_Group(NoinHDF5_Group *group);
int NoinHDF5_unset_Dataset(NoinHDF5_Dataset *dataset);
int NoinHDF5_unset_Attr(NoinHDF5_Attribute *attr);

size_t NoinHDF5_get_type_num(hid_t tid);
size_t NoinHDF5_get_type_num_ex(hid_t tid, H5T_class_t *type);

//typedef void (*print_type_ptr[3])(void *data);

void print_int(void *data);
void print_float(void *data);
void print_str(void *data);

size_t malloc_size_int(void **tar, int ndata, int dsize, void **buff);
size_t malloc_size_float(void **tar, int ndata, int dsize, void **buff);
size_t malloc_size_str1(void **tar, int ndata, int dsize, void **buff);
size_t malloc_size_str2(void **tar, int ndata, int dsize, void **buff);

#ifdef __cplusplus
}
#endif

#endif
