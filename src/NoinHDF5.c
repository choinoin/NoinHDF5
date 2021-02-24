/*
 * =====================================================================================
 *
 *       Filename:  NoinHDF5.c
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
 *        Created:  02/17/2021 01:24:11 PM
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

#include "../include/NoinColor.h"
#include "../include/NoinHDF5.h"

#define NoinHDF5_DEBUG

enum NoinHDF5_NumSet {

	NoinHDF5_TypeStrLen = 8,
};

static const char NoinHDF5_TYPE_STRING[] = "INTEGER\0FLOAT\0\0\0STRING\0\0";


hid_t NoinHDF5_NumTyp[3];/* = {
				H5T_NATIVE_INT,
				H5T_NATIVE_FLOAT,
				H5T_NATIVE_STRING,
			   };*/
/*
NoinHDF5_NumTyp[0] = H5T_NATIVE_INT;
NoinHDF5_NumTyp[1] = H5T_NATIVE_FLOAT;
NoinHDF5_NumTyp[2] = H5T_NATIVE_STRING;
*/

void (*print_type_ptr[3])(void *data) = {
						print_int,
						print_float,
						print_str,
					};

size_t (*malloc_size_ptr[4])(void **tar, int ndata, int dsize, void **buff) = {
									malloc_size_int,
									malloc_size_float,
									malloc_size_str1,
									malloc_size_str2,
								  };

int NoinHDF5_set_File(NoinHDF5_File *file, char *filename) {

	NoinHDF5_NumTyp[0] = H5T_NATIVE_INT;
	NoinHDF5_NumTyp[1] = H5T_NATIVE_FLOAT;
	NoinHDF5_NumTyp[2] = H5T_NATIVE_CHAR;

	strcpy(file->name, filename);

	file->fid = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);

	file->Groups = (NoinHDF5_Group *)malloc(sizeof(NoinHDF5_Group));

	file->Groups[0].depth = 0;
	file->Groups[0].gid = H5Gopen(file->fid, "/", H5P_DEFAULT);

#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "\n" CYELLOW "File Routine" CCYAN "[\"%s\"]" CWHITE": Start!\n\n", filename);
#endif

	NoinHDF5_set_Group(file->Groups);

#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "\n" CYELLOW "File Routine" CCYAN "[\"%s\"]" CWHITE ": End!\n\n", filename);
#endif

	return 0;
}

int NoinHDF5_set_Group(NoinHDF5_Group *group) {

	int i, j1, j2, k;

	char indent[NoinHDF5_MaxArrLen];

	hid_t otype;

	group->NGroup = 0;
	group->NDataset = 0;

	H5Iget_name(group->gid, group->name, NoinHDF5_MaxArrLen);

	group->NAttr = H5Aget_num_attrs(group->gid);

	memset(indent, '\0', sizeof(indent));

	for (i = 0; i < group->depth && i < NoinHDF5_MaxArrLen - 1; i++) {

		indent[i] = '\t';
	}

	if (group->NAttr) {

#ifdef NoinHDF5_DEBUG
		fprintf(stderr, "%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Attribute Reading Routine Start!\n", indent, group->name, group->depth);
		fprintf(stderr, "\n%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Total Attribute Num %d\n", indent, group->name, group->depth, group->NAttr);
#endif

		group->Attrs = (NoinHDF5_Attribute *)malloc(sizeof(NoinHDF5_Attribute) * group->NAttr);
		
		for (i = 0; i < group->NAttr; i++) {

			group->Attrs[i].aid = H5Aopen_idx(group->gid, i);

			group->Attrs[i].depth = group->depth + 1;

			NoinHDF5_set_Attr(&(group->Attrs[i]));
		}
#ifdef NoinHDF5_DEBUG
		fprintf(stderr, "%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Attribute Reading Routine End!\n\n", indent, group->name, group->depth);
#endif
	}

	H5Gget_num_objs(group->gid, (hsize_t *)&k);

	if (k) {

#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "\n%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Object Reading Routine Start!\n", indent, group->name, group->depth);
	fprintf(stderr, "\n%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": %d Object Detected!\n", indent, group->name, group->depth, k);
#endif

		for (i = 0; i < k; i++) {

			otype = H5Gget_objtype_by_idx(group->gid, (size_t)i);

			if (otype == H5G_GROUP) {
				
				group->NGroup++;
			} else if (otype == H5G_DATASET) {

				group->NDataset++;
			}
		}
#ifdef NoinHDF5_DEBUG
		if (group->NGroup) fprintf(stderr, "%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": %d of %d is Group\n", indent, group->name, group->depth, k, group->NGroup);
		if (group->NDataset) fprintf(stderr, "%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": %d of %d is Dataset\n", indent, group->name, group->depth, k, group->NDataset);
#endif

#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "\n%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Total Object Num %d\n", indent, group->name, group->depth, group->NGroup + group->NDataset);
#endif

		group->Groups = (NoinHDF5_Group *)malloc(sizeof(NoinHDF5_Group) * group->NGroup);
		group->Datasets = (NoinHDF5_Dataset *)malloc(sizeof(NoinHDF5_Dataset) * group->NDataset);
		
		for (i = 0, j1 = 0, j2 = 0; i < k; i++) {

			otype = H5Gget_objtype_by_idx(group->gid, (size_t)i);

			if (otype == H5G_GROUP) {
				
				H5Gget_objname_by_idx(group->gid, (hsize_t)i, group->Groups[j1].name, (size_t)NoinHDF5_MaxArrLen);
				group->Groups[j1].gid = H5Gopen(group->gid, group->Groups[j1].name, H5P_DEFAULT);
				group->Groups[j1].depth = group->depth + 1;
				NoinHDF5_set_Group(&(group->Groups[j1]));
				j1++;
			} else if (otype == H5G_DATASET) {

				H5Gget_objname_by_idx(group->gid, (hsize_t)i, group->Datasets[j2].name, (size_t)NoinHDF5_MaxArrLen);
				group->Datasets[j2].did = H5Dopen(group->gid, group->Datasets[j2].name, H5P_DEFAULT);
				group->Datasets[j2].depth = group->depth + 1;
				NoinHDF5_set_Dataset(&(group->Datasets[j2]));
				j2++;
			}
		}
#ifdef NoinHDF5_DEBUG
		fprintf(stderr, "%s" CYELLOW "Group Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Object Reading Routine End!\n\n\n", indent, group->name, group->depth);
#endif
	}

	return 0;
}

#define NoinHDF5_AttrBuffMaxLen 2048

int NoinHDF5_set_Dataset(NoinHDF5_Dataset *dataset) {

	int i, j1, j2, k;

	char indent[NoinHDF5_MaxArrLen];

	hid_t otype;

	H5Iget_name(dataset->did, dataset->name, NoinHDF5_MaxArrLen);

	dataset->NAttr = H5Aget_num_attrs(dataset->did);

	memset(indent, '\0', sizeof(indent));

	for (i = 0; i < dataset->depth && i < NoinHDF5_MaxArrLen - 1; i++) {

		indent[i] = '\t';
	}

	if (dataset->NAttr) {

#ifdef NoinHDF5_DEBUG
		fprintf(stderr, "%s" CYELLOW "Dataset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Attribute Reading Routine Start!\n", indent, dataset->name, dataset->depth);
		fprintf(stderr, "\n%s" CYELLOW "Dataset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Total Attribute Num %d\n", indent, dataset->name, dataset->depth, dataset->NAttr);
#endif

		dataset->Attrs = (NoinHDF5_Attribute *)malloc(sizeof(NoinHDF5_Attribute) * dataset->NAttr);
		
		for (i = 0; i < dataset->NAttr; i++) {

			dataset->Attrs[i].aid = H5Aopen_idx(dataset->did, i);

			dataset->Attrs[i].depth = dataset->depth + 1;

			NoinHDF5_set_Attr(&(dataset->Attrs[i]));
		}
#ifdef NoinHDF5_DEBUG
		fprintf(stderr, "%s" CYELLOW "Dataset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Attribute Reading Routine End!\n\n", indent, dataset->name, dataset->depth);
#endif
	}

#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "%s" CYELLOW "Dataset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Data Reading Routine Start!\n", indent, dataset->name, dataset->depth);
#endif
	dataset->dspace = H5Dget_space(dataset->did);
	dataset->dtype = H5Dget_type(dataset->did);
	dataset->dsize = H5Dget_storage_size(dataset->did);
	dataset->rank = H5Sget_simple_extent_ndims(dataset->dspace);

	dataset->type_num = NoinHDF5_get_type_num_ex(dataset->dtype, &dataset->type);

	dataset->dim = (hsize_t *)malloc(sizeof(hsize_t) * dataset->rank);
	H5Sget_simple_extent_dims(dataset->dspace, dataset->dim, NULL);

	dataset->mspace = H5Screate_simple(dataset->rank, dataset->dim, NULL);

	for (i = 0, dataset->ndata = 1; i < dataset->rank; i++) dataset->ndata *= dataset->dim[i];
	dataset->data = malloc(sizeof(float) * dataset->ndata);

	H5Dread(dataset->did, NoinHDF5_NumTyp[dataset->type_num], dataset->mspace, dataset->dspace, H5P_DEFAULT, (void *)dataset->data);

#ifdef NoinHDF5_DEBUG
/*
	for (i = 0; i < dataset->ndata; i++) {

		fprintf(stderr, "%s\tPrinted data[%04d]: %12.9f\n", indent, i, ((float *)dataset->data)[i]);
	}
*/
	fprintf(stderr, "%s" CYELLOW "Dataset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Data Reading Routine End!\n\n", indent, dataset->name, dataset->depth);
#endif

	return 0;
}

int NoinHDF5_set_Attr(NoinHDF5_Attribute *attr) {

	int i, j, k;

	void *buff[NoinHDF5_AttrBuffMaxLen];

	char indent[NoinHDF5_MaxArrLen];

	memset(indent, '\0', sizeof(indent));

	for (i = 0; i < attr->depth && NoinHDF5_MaxArrLen - 1; i++) {

		indent[i] = '\t';
	}

	H5Aget_name(attr->aid, NoinHDF5_MaxArrLen, attr->name);

#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "\n%s" CYELLOW "Attribute Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Data Reading Routine Start!\n", indent, attr->name, attr->depth);
#endif

	attr->dspace = H5Aget_space(attr->aid);
	attr->dtype  = H5Aget_type(attr->aid);
	attr->rank   = H5Sget_simple_extent_ndims(attr->dspace);

	attr->type_num = NoinHDF5_get_type_num(attr->dtype);

	if (attr->rank) {

		H5Aread(attr->aid, attr->dtype, (void *)buff);

		attr->dim = (hsize_t *)malloc(sizeof(hsize_t) * attr->rank);

		H5Sget_simple_extent_dims(attr->dspace, attr->dim, NULL);

		attr->dsize = H5Aget_storage_size(attr->aid);

		if (attr->dsize) {

			for (i = 0, attr->ndata = 1; i < attr->rank; i++) attr->ndata *= attr->dim[i];

//			fprintf(stderr, "%d %d", attr->ndata, attr->dsize / attr->ndata);
//			fprintf(stderr, "%s\t" CRED "Data" CBLUE "[type]\n" CWHITE, indent);

			attr->dsize = malloc_size_ptr[attr->type_num + 1](&attr->data, attr->ndata, attr->dsize, buff);

#ifdef NoinHDF5_DEBUG
			for (i = 0; i < attr->ndata; i++) {

				fprintf(stderr, "%s\t" CRED "Data" CBLUE "[%s]: " CWHITE, indent, &NoinHDF5_TYPE_STRING[attr->type_num * NoinHDF5_TypeStrLen]);
				print_type_ptr[attr->type_num](attr->data + i * attr->dsize);
				fprintf(stderr, "\n");
			}
#endif
//			free(attr->data);
		}
	} else {

		H5Aread(attr->aid, attr->dtype, (void *)buff);

		attr->dsize = strlen((char *)*buff) + 1;
		attr->ndata = 1;

//		fprintf(stderr, "%d %d", attr->ndata, attr->dsize);
//		fprintf(stderr, "%s\t" CRED "Data" CBLUE "[type]\n" CWHITE, indent);

		if (attr->dsize) {

			malloc_size_ptr[attr->type_num](&attr->data, attr->ndata, attr->dsize, buff);

#ifdef NoinHDF5_DEBUG
			for (i = 0; i < attr->ndata; i++) {
			fprintf(stderr, "%s\t" CRED "Data" CBLUE "[%s]: " CWHITE, indent, &NoinHDF5_TYPE_STRING[attr->type_num * NoinHDF5_TypeStrLen]);
			print_type_ptr[attr->type_num](attr->data);
			fprintf(stderr, "\n");
#endif
//			free(attr->data);
			}
		}
	}
#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "%s" CYELLOW "Attribute Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Data Reading Routine End!\n\n", indent, attr->name, attr->depth);
#endif

	return 0;
}

#undef NoinHDF5_AttrBuffMaxLen

int NoinHDF5_unset_File(NoinHDF5_File *file) {

#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "\n" CYELLOW "File Unset Routine" CCYAN "[\"%s\"]" CWHITE": Start!\n\n", file->name);
#endif
	NoinHDF5_unset_Group(file->Groups);

	H5Gclose(file->Groups[0].gid);
	free(file->Groups);
#ifdef NoinHDF5_DEBUG
	fprintf(stderr, "\n" CYELLOW "File Unset Routine" CCYAN "[\"%s\"]" CWHITE ": End!\n\n", file->name);
#endif

	H5Fclose(file->fid);

	return 0;
}

int NoinHDF5_unset_Group(NoinHDF5_Group *group) {

	int i;

	char indent[NoinHDF5_MaxArrLen];

	memset(indent, '\0', sizeof(indent));

	for (i = 0; i < group->depth && i < NoinHDF5_MaxArrLen - 1; i++) {

		indent[i] = '\t';
	}

	if (group->NGroup) {

		while (group->NGroup--) {
#ifdef NoinHDF5_DEBUG
			fprintf(stderr, "%s" CYELLOW "Group Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Group Unset Routine Start!\n", indent, group->name, group->depth);
#endif
			NoinHDF5_unset_Group(&group->Groups[group->NGroup]);
			H5Gclose(group->Groups[group->NGroup].gid);
#ifdef NoinHDF5_DEBUG
			fprintf(stderr, "%s" CYELLOW "Group Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Group Unset Routine End!\n\n", indent, group->name, group->depth);
#endif
		}
		free(group->Groups);
	}

	if (group->NDataset) {

		while (group->NDataset--) {
#ifdef NoinHDF5_DEBUG
			fprintf(stderr, "%s" CYELLOW "Group Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Dataset Unset Routine Start!\n", indent, group->name, group->depth);
#endif
			NoinHDF5_unset_Dataset(&group->Datasets[group->NDataset]);
			H5Dclose(group->Datasets[group->NDataset].did);
#ifdef NoinHDF5_DEBUG
			fprintf(stderr, "%s" CYELLOW "Group Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Dataset Unset Routine End!\n\n", indent, group->name, group->depth);
#endif
		}
		free(group->Datasets);
	}

	if (group->NAttr) {

		while (group->NAttr--) {
#ifdef NoinHDF5_DEBUG
			fprintf(stderr, "%s" CYELLOW "Group Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Attribute Unset Routine Start!\n", indent, group->name, group->depth);
#endif
			NoinHDF5_unset_Attr(&group->Attrs[group->NAttr]);
			H5Aclose(group->Attrs[group->NAttr].aid);
#ifdef NoinHDF5_DEBUG
			fprintf(stderr, "%s" CYELLOW "Group Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Attribute Unset Routine End!\n\n", indent, group->name, group->depth);
#endif
		}
		free(group->Attrs);
	}

	return 0;
}

int NoinHDF5_unset_Dataset(NoinHDF5_Dataset *dataset) {

	int i;

	char indent[NoinHDF5_MaxArrLen];

	memset(indent, '\0', sizeof(indent));

	for (i = 0; i < dataset->depth && i < NoinHDF5_MaxArrLen - 1; i++) {

		indent[i] = '\t';
	}

	if (dataset->NAttr) {

		while (dataset->NAttr--) {
#ifdef NoinHDF5_DEBUG
			fprintf(stderr, "%s" CYELLOW "Dataset Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Attribute Unset Routine Start!\n", indent, dataset->name, dataset->depth);
#endif
			NoinHDF5_unset_Attr(&dataset->Attrs[dataset->NAttr]);
			H5Aclose(dataset->Attrs[dataset->NAttr].aid);
#ifdef NoinHDF5_DEBUG
			fprintf(stderr, "%s" CYELLOW "Dataset Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Attribute Unset Routine End!\n\n", indent, dataset->name, dataset->depth);
#endif
		}
		free(dataset->Attrs);
	}

	free(dataset->data);

	H5Tclose(dataset->dtype);
	H5Sclose(dataset->dspace);
	H5Sclose(dataset->mspace);

	return 0;
}

int NoinHDF5_unset_Attr(NoinHDF5_Attribute *attr) {

	int i;

	char indent[NoinHDF5_MaxArrLen];

	memset(indent, '\0', sizeof(indent));

	for (i = 0; i < attr->depth && i < NoinHDF5_MaxArrLen - 1; i++) {

		indent[i] = '\t';
	}

	if (attr->ndata) {

#ifdef NoinHDF5_DEBUG
		fprintf(stderr, "%s" CYELLOW "Attribute Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Data Unset Routine Start!\n", indent, attr->name, attr->depth);
#endif
		free(attr->data);
#ifdef NoinHDF5_DEBUG
		fprintf(stderr, "%s" CYELLOW "Attribute Unset Routine" CCYAN "[\"%s\" | %d]" CWHITE ": Data Unset Routine End!\n\n", indent, attr->name, attr->depth);
#endif
	}

	H5Tclose(attr->dtype);
	H5Sclose(attr->dspace);

	return 0;
}

size_t NoinHDF5_get_type_num(hid_t tid) {

	H5T_class_t type;

	type = H5Tget_class(tid);

	if (type == H5T_INTEGER) {

		return 0;
//		return sizeof(int);
	} else if (type == H5T_FLOAT) {

		return 1;
//		return sizeof(float);
	} else if (type == H5T_STRING) {

		return 2;
//		return sizeof(char);
#ifdef NoinHDF5_ExMode

	} else if (type == H5T_BITFIELD) {


	} else if (type == H5T_OPAQUE) {


	} else if (type == H5T_COMPOUND) {


	} else if (type == H5T_ARRAY) {


	} else if (type == H5T_ENUM) {

#endif
	}
	return -1;
}

size_t NoinHDF5_get_type_num_ex(hid_t tid, H5T_class_t *type) {

	hid_t _tid;

	H5T_class_t _type;

	_type = H5Tget_class(tid);
	_tid = H5Tget_native_type(tid, H5T_DIR_ASCEND);

	*type = H5Tget_class(_tid);

	if (_type == H5T_INTEGER) {

		return 0;
//		return sizeof(int);
	} else if (_type == H5T_FLOAT) {

		return 1;
//		return sizeof(float);
	} else if (_type == H5T_STRING) {

		return 2;
//		return sizeof(char);
	}
	
	return -1;
}

void print_int(void *data) {

	fprintf(stderr, "%d ", *((int *)data));
}

void print_float(void *data) {

	fprintf(stderr, "%f ", *((float *)data));
}

void print_str(void *data) {

	fprintf(stderr, "%s ", (char *)data);
}

size_t malloc_size_int(void **tar, int ndata, int dsize, void **buff) {

	size_t size = dsize * sizeof(int);

	*tar = (int *)malloc(size);

	memcpy(*tar, *buff, size);

//	fprintf(stderr, "\nTest value: %d\n", *tar);

	return size;
}

size_t malloc_size_float(void **tar, int ndata, int dsize, void **buff) {

	size_t size = dsize * sizeof(float);

	*tar = (float *)malloc(size);

	memcpy(*tar, *buff, size);

//	fprintf(stderr, "\nTest value: %f\n", *tar);

	return size;
}

size_t malloc_size_str1(void **tar, int ndata, int dsize, void **buff) {

	size_t size = dsize * sizeof(char);

	*tar = (char *)malloc(size);

	memcpy(*tar, *buff, size);

//	fprintf(stderr, "\nTest string: %s\n", *tar);

	return size;
}

size_t malloc_size_str2(void **tar, int ndata, int dsize, void **buff) {
/*
	size_t size = dsize * sizeof(char);

	*tar = (char *)malloc(size);

	memcpy(*tar, buff, size);

//	fprintf(stderr, "\nTest string: %s\n", *tar);
*/

	int i;

	size_t size = (dsize + ndata) * sizeof(char);
	int chunk = dsize / ndata;
	int pos = chunk + 1;

	*tar = (char *)malloc(size);

	for (i = 0; i < ndata; i++) {

		memcpy(((char *)*tar) + pos * i, ((char *)buff) + chunk * i, chunk * sizeof(char));

//		fprintf(stderr, "\nTest string[%d]: %s\n", i, (char *)*tar + pos * i);
	}

	return pos;
}
