
///////////////////////////////////////////////////////////////////////////////
// No Warranty
// Except as may be otherwise agreed to in writing, no warranties of any
// kind, whether express or implied, are given by MTK with respect to any MTK
// Deliverables or any use thereof, and MTK Deliverables are provided on an
// "AS IS" basis.  MTK hereby expressly disclaims all such warranties,
// including any implied warranties of merchantability, non-infringement and
// fitness for a particular purpose and any warranties arising out of course
// of performance, course of dealing or usage of trade.  Parties further
// acknowledge that Company may, either presently and/or in the future,
// instruct MTK to assist it in the development and the implementation, in
// accordance with Company's designs, of certain softwares relating to
// Company's product(s) (the "Services").  Except as may be otherwise agreed
// to in writing, no warranties of any kind, whether express or implied, are
// given by MTK with respect to the Services provided, and the Services are
// provided on an "AS IS" basis.  Company further acknowledges that the
// Services may contain errors, that testing is important and Company is
// solely responsible for fully testing the Services and/or derivatives
// thereof before they are used, sublicensed or distributed.  Should there be
// any third party action brought against MTK, arising out of or relating to
// the Services, Company agree to fully indemnify and hold MTK harmless.
// If the parties mutually agree to enter into or continue a business
// relationship or other arrangement, the terms and conditions set forth
// hereunder shall remain effective and, unless explicitly stated otherwise,
// shall prevail in the event of a conflict in the terms in any agreements
// entered into between the parties.
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, MediaTek Inc.
// All rights reserved.
//
// Unauthorized use, practice, perform, copy, distribution, reproduction,
// or disclosure of this information in whole or in part is prohibited.
////////////////////////////////////////////////////////////////////////////////
// AcdkCLITest.cpp  $Revision$
////////////////////////////////////////////////////////////////////////////////

//! \file  AcdkCLITest.cpp
//! \brief
 
#define LOG_TAG "CamPipeTest"

#include <stdio.h>
#include <stdlib.h>
//
#include <errno.h>
#include <fcntl.h>

extern int main_camio(int argc, char** argv); 
extern int main_cdp(int argc, char **argv); 
extern int main_postproc(int argc, char **argv); 

void Usage()
{
    printf("Usage: campipetest 0 => camio test, 1 => postproc test, 2  => cdp test \n");
}

int main(int argc, char** argv)
{
    int ret = 0; 
    
    //
    // main_camio(argc, argv); 
    if (argc==2)
    {
        if (atoi(argv[1])==0)
        {
            char* camio[] = {"test", "1"}; 
            main_camio(2, camio); 
        }
        else if (atoi(argv[1])==1)
        {
            char* postproc[] = {"post", "/data/Sunset_800x600_YUY2.yuv", "800", "600", "2"}; 
            // main_postproc(argc, argv); 
            main_postproc(5, postproc);
        }
        else if (atoi(argv[1])==2)
        {
            char* cdp[] = {"cdp", "/data/Sunset_800x600_YUY2.yuv", "800", "600", "2"}; 
            // main_postproc(argc, argv); 
            main_cdp(5, cdp);
        }
        else
        {
            Usage();
        }
    }
    else
    {
         Usage();
    }
    return ret; 
}
