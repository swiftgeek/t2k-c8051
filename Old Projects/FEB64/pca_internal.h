/********************************************************************\

  Name:         pca_internal.h
  Created by:     							


  Contents:     Internal PCA

  $Id: pca_internal.h 58 2008-02-28 22:40:33Z fgddaq $

\********************************************************************/

#ifndef  _INTERNAL_PCA_H
#define  _INTERNAL_PCA_H

//
// mode refers to:
//  1 : Initialization
//  2 : turn OFF PCA
//  3 : turn  ON PCA
// 
void pca_operation(unsigned char mode);

#endif