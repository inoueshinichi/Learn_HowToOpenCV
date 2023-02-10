//
//  mat_iterator_ini.h
//  matProject
//
//  Created by Inoue Shinichi on 2019/01/02.
//  Copyright © 2019 Inoue Shinichi. All rights reserved.
//

#ifndef mat_iterator_ini_h
#define mat_iterator_ini_h
#include "mat_iterator.h"

namespace my
{
    ////////////////////////// CMyMatConstIterator /////////////////////////
    
    inline
    CMyMatConstIterator::CMyMatConstIterator()
    : m_pMat(nullptr)
    , m_ptr(nullptr)
    , m_pSliceStart(nullptr)
    , m_pSliceEnd(nullptr)
    , m_oriSize(0, 0)
    {
        // Constructor
    };
    
    
    inline
    CMyMatConstIterator::CMyMatConstIterator(const CMyMat* pmat)
    : m_pMat(pmat)
    , m_ptr(nullptr)
    , m_pSliceStart(nullptr)
    , m_pSliceEnd(nullptr)
    , m_oriSize(pmat->m_oriSize)
    {
        // Constructor
        m_pSliceStart = pmat->BPtr(0);
        m_pSliceEnd = pmat->BPtr(pmat->TotalMemSize());
    };
    
    
    inline
    CMyMatConstIterator::CMyMatConstIterator(const CMyMat* pmat, int row, int col)
    : m_pMat(pmat)
    , m_ptr(nullptr)
    , m_pSliceStart(nullptr)
    , m_pSliceEnd(nullptr)
    , m_oriSize(pmat->m_oriSize)
    {
        // Constructor
        m_pSliceStart = pmat->BPtr(0);
        m_pSliceend = pmat->BPtr(pmat->TotalMemSize());
        m_ptr = pmat->Ptr<byte>(row, col);
    };
    
    
    inline
    CMyMatConstIterator::CMyMatConstIterator(const CMyMat* pmat, CMyPoint pt)
    : m_pMat(pmat)
    , m_ptr(nullptr)
    , m_pSliceStart(nullptr)
    , m_pSliceEnd(nullptr)
    , m_oriSize(pmat->m_oriSize)
    {
        // Constructor
        m_pSliceStart = pmat->BPtr(0);
        m_pSliceend = pmat->BPtr(pmat->TotalMemSize());
        m_ptr = pmat->Ptr<byte>(pt.m_y, pt.m_x);
    };
    
    
    inline
    CMyMatConstIterator::CMyMatConstIterator(const CMyMatConstIterator& it)
    : m_pMat(it.m_pMat)
    , m_ptr(it.m_ptr)
    , m_pSliceStart(it.m_pSliceStart)
    , m_pSliceEnd(it.m_pSliceEnd)
    , m_oriSize(it.m_oriSize)
    {
        // Constructor
    };
    
    
    inline
    CMyMatConstIterator& CMyMatConstIterator::operator = (const CMyMatConstIterator& it)
    {
        m_pMat = it.m_pMat;
        m_ptr = it.m_ptr;
        m_pSliceStart = it.m_pSliceStart;
        m_pSliceEnd = it.m_pSliceEnd;
        m_oriSize = it.m_oriSize;
        return *this;
    };
    
    
    inline
    const byte* CMyMatConstIterator::operator * () const
    {
        return m_ptr;
    };
    
    
    inline
    const byte* CMyMatConstIterator::operator [] (ptrdiff_t i) const
    {
        return *(*this + i); // ??? 1/2
    };
    
    
    
    
    
    ////////////////////////// CMyMatConstIterator_<T> /////////////////////////
}

#endif /* mat_iterator_ini_h */
