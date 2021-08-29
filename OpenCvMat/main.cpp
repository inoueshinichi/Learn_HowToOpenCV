//
//  main.cpp
//  matProject
//
//  Created by Inoue Shinichi on 2018/12/28.
//  Copyright © 2018 Inoue Shinichi. All rights reserved.
//

#include <iostream>
#include "mat.h"
using namespace my;

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Test CMyMat!\n";
    
    // CMyMat()
    CMyMat mat1; // native
    int type1 = mat1.Type();
    CMyMat mat2(16, 16, MY_8UC3); // native
    int type2 = mat2.Type();
    mat2.ShowState();
    
    CMySize size(16, 16);
    CMyMat mat3(size, MY_8UC3); // native
    int type3 = mat3.Type();
    
    CMyMat sharrow_mat4(mat3); // sharrow
    int type4 = sharrow_mat4.Type();
    
    CMyRect roi(1, 14, 1, 14);
    CMyMat roi_mat5(sharrow_mat4, roi); // roi
    int type5 = roi_mat5.Type();
    
    CMyMat sharrow_mat6;
    sharrow_mat6 = roi_mat5; // sharrow
    int type6 = sharrow_mat6.Type();
    
    CMyMat sharrow_mat7(roi_mat5); // sharrow
    int type7 = sharrow_mat7.Type();
    
    CMyRect roi2(1, 12, 1, 12);
    CMyMat roi_mat8(sharrow_mat7, roi2);
    int type8 = roi_mat8.Type();
    
    CMyMat roi_mat9 = mat2(roi2);
    int type9 = roi_mat9.Type();
    roi_mat9.ShowState();
    
    CMyMat deepCopied_mat10;
    roi_mat9.CopyTo(deepCopied_mat10);
    int type10 = deepCopied_mat10.Type();
    deepCopied_mat10.ShowState();
    
    CMyMat deepCopied_mat11;
    deepCopied_mat11 = roi_mat9.Clone();
    deepCopied_mat11.ShowState();
    
    CMyMat moved_mat12(std::move(deepCopied_mat11));
    moved_mat12.ShowState();
    //deepCopied_mat11.ShowState();
    
    size_t elemStep = moved_mat12.ElemStep();
    size_t elemSize = moved_mat12.ElemSize();
    size_t memStep = moved_mat12.MemStep();
    size_t memSize = moved_mat12.MemSize();
    
    CMyMat mat13(32, 32, MY_32FC3);
    mat13.ShowState();
    
    // data operation for mat13
    float_32* pMat13_data = mat13.Ptr<float_32>(0);
    *pMat13_data = 3.14;
    float_32 out_data = mat13.At<float_32>(0);
    
    return 0;
}
