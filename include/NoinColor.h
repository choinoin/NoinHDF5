/*
 * =====================================================================================
 *
 *       Filename:  NoinColor.h
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
 *        Created:  02/15/2021 01:35:37 PM
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

#ifndef NoinColor
#define NoinColor

#ifdef __cplusplus
extern "C" {
#endif

#define CRED     "\033[1;31m"
#define CGREEN   "\033[0;32m"
#define CYELLOW  "\033[1;33m"
#define CBLUE    "\033[1;34m"
#define CMAGENTA "\033[0;35m"
#define CCYAN    "\033[0;36m"
#define CWHITE   "\033[0m"

#define  CRGB(O, R, G, B, S) fprintf(O ,"\033[38;2;%d;%d;%dm" S, R, G, B)
#define CBRGB(O, R, G, B, S) fprintf(O ,"\033[48;2;%d;%d;%dm" S, R, G, B)

#ifdef __cplusplus
}
#endif

#endif
