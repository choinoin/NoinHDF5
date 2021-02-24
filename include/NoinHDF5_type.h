/*
 * =====================================================================================
 *
 *       Filename:  NoinHDF5_type.h
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
 *        Created:  02/17/2021 01:44:48 PM
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

#ifndef NoinHDF5_TYPE
#define NoinHDF5_TYPE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hdf5.h>

#define NoinHDF5_MaxArrLen 512

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NoinHDF5_Dataset   NoinHDF5_Dataset;
typedef struct NoinHDF5_Attribute NoinHDF5_Attribute;
typedef struct NoinHDF5_Group     NoinHDF5_Group;
typedef struct NoinHDF5_File      NoinHDF5_File;

typedef struct NoinHDF5_Dataset {

	hid_t did;

	H5T_class_t type;

	int depth;
	int rank;
	int ndata;
	int NAttr;

	char name[NoinHDF5_MaxArrLen];

	size_t type_num;
	hsize_t dsize;
	hsize_t *dim;

	hid_t dtype;
	hid_t dspace;
	hid_t mspace;

	NoinHDF5_Attribute *Attrs;
	void *data;
} NoinHDF5_Dataset;

typedef struct NoinHDF5_Attribute {

	hid_t aid;

	int depth;
	int rank;
	int ndata;

	char name[NoinHDF5_MaxArrLen];

	size_t type_num;
	hsize_t dsize;
	hsize_t *dim;

	hid_t dtype;
	hid_t dspace;

	void *data;
} NoinHDF5_Attribute;

typedef struct NoinHDF5_Group {

	hid_t gid;

	int depth;
	int NGroup;
	int NDataset;
	int NAttr;

	char name[NoinHDF5_MaxArrLen];

	NoinHDF5_Group *Groups;
	NoinHDF5_Dataset *Datasets;
	NoinHDF5_Attribute *Attrs;
} NoinHDF5_Group;

typedef struct NoinHDF5_File {

	hid_t fid;

	char name[NoinHDF5_MaxArrLen];

	NoinHDF5_Group *Groups;
} NoinHDF_File;

#ifdef __cplusplus
}
#endif

#endif
