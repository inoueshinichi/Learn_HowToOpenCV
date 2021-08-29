//
//  mat_const_iterator.h
//  matProject
//
//  Created by Inoue Shinichi on 2019/01/02.
//  Copyright © 2019 Inoue Shinichi. All rights reserved.
//

#ifndef mat_const_iterator_h
#define mat_const_iterator_h
#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include "mat.h"

namespace my
{
    class CMyMatConstIterator
    {
    public:
        typedef byte* value_type;
        typedef ptrdiff_t difference_type;
        typedef const byte** pointer;
        typedef byte* reference;
        
        typedef std::random_access_iterator_tag iterator_category;
        
    public:
        const CMyMat* m_pMat;
        const byte* m_ptr;
        const byte* m_pSliceStart
        const byte* m_pSliceEnd;
        CMyMatSize m_oriSize;
        
    public:
        // Constructor
        CMyMatConstIterator();
        CMyMatConstIterator(const CMyMat* pmat);
        CMyMatConstIterator(const CMyMat* pmat, int row, int col=0);
        CMyMatConstIterator(const CMyMat* pmat, CMyPoint pt);
        CMyMatConstIterator(const CMyMatConstIterator& it);
        
        // Operator
        CMyMatConstIterator& operator = (const CMyMatConstIterator& it);
        const byte* operator * () const;
        const byte* operator [] (ptrdiff_t i) const;
        CMyMatConstIterator& operator += (ptrdiff_t ofs);
        CMyMatConstIterator& operator -= (ptrdiff_t ofs);
        CMyMatConstIterator& operator ++ ();
        CMyMatConstIterator& operator ++ (int);
        CMyMatConstIterator& operator -- ();
        CMyMatConstIterator& operator -- (int);
        
        // function
        virtual CMyPoint Pos() const;
        //ptrdiff_t LPos() const;
        //void Seek(ptrdiff_t ofs, bool relative=false);
        //void Seek(const int* idx, bool relative=false);
        
    };
    
    // read-only iterator
    template<typename T>
    class CMyMatConstIterator_
    : public CMyMatConstIterator
    {
    public:
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef const T* pointer;
        typedef const T& reference;
        
        typedef std::random_access_iterator_tag iterator_category;
    
    public:
        // Constructor
        CMyMatConstIterator_();
        CMyMatConstIterator_(const CMyMat_<T>* pmat_);
        CMyMatConstIterator_(const CMyMat_<T>* pmat_, int row, int col=0);
        CMyMatConstIterator_(const CMyMat_<T>* pmat_, CMyPoint pt);
        CMyMatConstIterator_(const CMyMatConstIterator_& it_);
        
        // Operator
        CMyMatConstIterator_& operator = (const CMyMatConstIterator_& it);
        const T& operator * () const;
        const T& operator [] (ptrdiff_t i) const;
        CMyMatConstIterator_& operator += (ptrdiff_t ofs);
        CMyMatConstIterator_& operator -= (ptrdiff_t ofs);
        CMyMatConstIterator_& operator ++ ();
        CMyMatConstIterator_ operator ++ (int);
        CMyMatConstIterator_& operator -- ();
        CMyMatConstIterator_ operator -- (int);
        
        // function
        CMyPoint Pos() const;
    };
    
    // read-write iterator
    template<typename T>
    class CMyMatIterator_
    : public CMyMatConstIterator_<T>
    {
    public:
        typedef T* pointer;
        typedef T& reference;
        
        typedef std::random_access_iterator_tag iterator_category;
        
        // Constructor
        CMyMatIterator_();
        CMyMatIterator_(CMyMat_<T>* pmat_);
        CMyMatIterator_(CMyMat_<T>* pmat_, int row, int col=0);
        CMyMatIterator_(CMyMat_<T>* pmat_, CMyPoint pt);
        CMyMatIterator_(const CMyMatIterator_& it_);
        
        // Operator
        CMyMatIterator_& operator = (const CMyMatIterator_& it_);
        T& operator * () const;
        T& operator [] (ptrdiff_t i) const;
        CMyMatIterator_& operator += (ptrdiff_t ofs);
        CMyMatIterator_& operator -= (ptrdiff_t ofs);
        CMyMatIterator_& operator ++ ();
        CMyMatIterator_  operator ++ (int);
        CMyMatIterator_& operator -- ();
        CMyMatIterator_  operator -- (int);
        
    };
    
    
#include "mat_iterator_ini.h"
    
};


#endif /* mat_const_iterator_h */
