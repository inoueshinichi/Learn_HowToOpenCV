//
//  mat.h
//  matProject
//
//  Created by Inoue Shinichi on 2018/12/28.
//  Copyright © 2018 Inoue Shinichi. All rights reserved.
//

#ifndef mat_h
#define mat_h

#include <string>
#include <assert.h>
#include "mat_util.h"
#include "mat_def.h"
#include "mat_traits.h"

/******************************************
 自作ヘッダー
 主に画像メモリ確保用のクラス宣言など
 VC++ではtemplateクラスの実装を.cppにするときは
 明示的実体化方法をする必要がある。そうしないと、
 LNK2019とLNK1120エラーが発生する。
 VC++のコンパイラの構造上、ヘッダーに実装を書くか、
 上記の対処をしなければならない。
 ここは，インクルードモデルを使って，別のヘッダーに
 実装を書いて，このファイルでインクルードします。
 ******************************************/



namespace my
{
    
    class CMyMatMemStep
    {
    public:
        size_t m_buf[2]; // buf[0]: native mat step, buf[1]: roi mat step
        
    public:
        inline CMyMatMemStep()
        {
            m_buf[0] = m_buf[1] = 0;
        };
        
        inline explicit CMyMatMemStep(size_t s)
        {
            m_buf[0] = s;
            m_buf[1] = 0;
        };
        
        inline const size_t& operator [] (int i) const
        {
            assert(i >= 0 && "index is more than zero !");
            assert(i < 2 && "index is less than 2 !");
            return m_buf[i];
        };
        
        inline size_t& operator [] (int i)
        {
            assert(i >= 0 && "index is more than zero !");
            assert(i < 2 && "index is less than 2 !");
            return m_buf[i];
        };
        
        inline operator size_t () const
        {
            return m_buf[0];
        };
        
        inline CMyMatMemStep& operator = (size_t s)
        {
            m_buf[0] = s;
            return *this;
        };
        
        CMyMatMemStep& operator = (const CMyMatMemStep& memStep)
        {
            this->m_buf[0] = memStep.m_buf[0];
            this->m_buf[1] = memStep.m_buf[1];
            return *this;
        };
    };
    
    
    
     class CMyMatSize
     {
     public:
         int m_buf[2];
         
     public:
         
         inline CMyMatSize(int ori_rows, int ori_cols)
         {
             m_buf[0] = ori_rows;
             m_buf[1] = ori_cols;
         };
         
         
         inline CMyMatSize(const CMyMatSize& sz)
         {
             m_buf[0] = sz.m_buf[0];
             m_buf[1] = sz.m_buf[1];
         };
         
         
         inline CMyMatSize(CMyMatSize&& sz)
         {
             m_buf[0] = sz.m_buf[0];
             m_buf[1] = sz.m_buf[1];
             sz.m_buf[0] = 0;
             sz.m_buf[1] = 0;
         };
         
         
         inline CMyMatSize& operator = (const CMyMatSize& sz)
         {
             m_buf[0] = sz.m_buf[0];
             m_buf[1] = sz.m_buf[1];
             return *this;
         };
         
         
         inline CMyMatSize& operator = (CMyMatSize& sz)
         {
             m_buf[0] = sz.m_buf[0];
             m_buf[1] = sz.m_buf[1];
             sz.m_buf[0] = 0;
             sz.m_buf[1] = 0;
             return *this;
         };
         
         
         inline CMySize operator () () const
         {
             return CMySize(m_buf[1], m_buf[0]); // (width, height)
         };
         
         
         inline const int& operator [] (int i) const
         {
             assert(i >= 0);
             assert(i < 2);
             return m_buf[i];
         };
         
         
         inline int& operator [] (int i)
         {
             assert(i >= 0);
             assert(i < 2);
             return m_buf[i];
         };
         
         
         bool operator == (const CMyMatSize& sz) const
         {
             if (sz.m_buf[0] != m_buf[0] || sz.m_buf[1] != m_buf[1])
                 return false;
             else
                 return true;
         }
     
         
         bool operator != (const CMyMatSize& sz) const
         {
             return !(*this == sz);
         };
    };
    
    
    
    
    
    
    /*
    class CMyUMatData
    {
    public:
        const CMyMatAllocator *prevAllocator;
        const CMyMatAllocator *currAllocator;
        int urefcount;
        int refcount;
        byte *data;
        byte *origdata;
        size_t size;
        
        CMyUMatData::MemoryFlag flags;
        void *handle;
        void *userdata;
        int allocatorFlags_;
        int mapcount;
        CMyMatData *originalUMatData;
        
        enum MemoryFlag
        {
            COPY_ON_MAP = 1,
            HOST_COPY_OBSOLETE = 2,
            DEVICE_COPY_OBSOLETE = 4,
            TEMP_UMAT = 8,
            TEMP_COPIED_UMAT = 24,
            USER_ALLOCATED = 32,
            DEVICE_MEM_MAPPED = 64,
            ASYNC_CLEANUP = 128,
        };
        
    public:
        CMyUMatData(const CMyMatAllocator *allocator);
        ~CMyUMatData();
        
        // スレッドせセーフのための排他制御
        void lock();
        void unlock();
        
        bool hostCopyObsolete() const;
        bool deviceCopyObsolete() const;
        bool deviceMemMapped() const;
        bool copyOnMap() const;
        bool tempCopiedUMat() const;
        void markHostCopyObsolete(bool flag);
        void markDeviceCopyObsolete(bool flag);
        void markDeviceMemMapped(bool flag);
    };
    */
    
    // CMyMat系の基底クラス
    class CMyMat
    {
    public:
        
        // インスタンス別に存在
        int m_flags;
        int m_rows;
        int m_cols;
        CMyMatMemStep m_memStep;
        CMyMatSize m_oriSize;
        
        // sharrow copyでチェインされたグループに1つ存在
        byte* m_pImgData;
        const byte* m_pImgDataStart;
        const byte* m_pImgDataEnd;
        
        // 参照カウンタ: m_pImgDataが参照された回数を保持。
        int* m_pRefCount;
        
        enum { MAGIC_VAL = 0x42FF0000, CONTINUOUS_FLAG = MY_MAT_CONT_FLAG, SUBMATRIX_FLAG = MY_SUBMAT_FLAG };
        enum { MAGIC_MASK = 0xFFFF0000, TYPE_MASK = 0x00000FFF, DEPTH_MASK = 7 };
        
        // constructor
        CMyMat();
        CMyMat(int rows, int cols, int type);
        CMyMat(CMySize size, int type);
        CMyMat(const CMyMat& mat, const CMyRect& roi);
        CMyMat(const CMyMat& mat); // copy constructor
        CMyMat(CMyMat&& mat) noexcept; // move constructor
        
        // destructor
        ~CMyMat();
        
        // operator
        CMyMat  operator () (const CMyRect& roi) const;
        CMyMat& operator = (const CMyMat& mat); // copy assignment operator
        CMyMat& operator = (CMyMat&& mat); // move assignment operator
        
        // function 1
        void CreateMat(int rows, int cols, int type);
        void CreateMat(CMySize size, int type);
        void ReleaseMat();
        bool IsEmpty() const;
        bool IsSubmatrix() const;
        void CopyTo(CMyMat& mat) const;
        CMyMat Clone() const;
        CMyMat& AdjustROI(int top, int bottom, int left, int right);
        
        // function 2
        void ShowState() const;
        bool IsValidType(int type) const;
        int Cols() const; // channelを無視した(1channelとした)場合の行方向ステップサイズ
        int Rows() const;
        virtual int Channels() const;
        virtual int Type() const;
        virtual int TypeSize() const;
        virtual int Depth() const;
        std::string ShowType(int type) const;
        std::string ShowDepth(int depth) const;
        size_t ElemStep() const;
        size_t ElemSize() const;
        size_t MemStep() const;
        size_t MemSize() const;
        size_t TotalMemSize() const;
        
        // function 3
        const byte* BPtr(size_t memIdx) const;
        byte* BPtr(size_t memIdx);
        template <typename T> const T* Ptr(int yIdx) const;
        template <typename T> T* Ptr(int yIdx);
        template <typename T> const T* Ptr(int row, int col) const;
        template <typename T> T* Ptr(int row, int col);
        template <typename T> const T* Ptr(int row, int col, int channel) const;
        template <typename T> T* Ptr(int row, int col, int channel);
        template <typename T> const T& At(int row, int col, int channel) const;
        template <typename T> T& At(int row, int col, int channel);
        template <typename T> const T& At(int row, int col) const;
        template <typename T> T& At(int row, int col);
        template <typename T> const T& At(int yIdx) const;
        template <typename T> T& At(int yIdx);
        const byte& BAt(size_t memIdx) const;
        byte& BAt(size_t memIdx);
        
        template <typename T> const T* Ptr(CMyPoint pt) const;
        template <typename T> T* Ptr(CMyPoint pt);
        template <typename T> const T& At(CMyPoint pt) const;
        template <typename T> T& At(CMyPoint pt);
        
        template <typename T> const T* Ptr(CMyPoint pt, int channel) const;
        template <typename T> T* Ptr(CMyPoint pt, int channel);
        template <typename T> const T& At(CMyPoint pt, int channel) const;
        template <typename T> T& At(CMyPoint pt, int channel);
        
        // function4
        //template<typename T> CMyMatIterator_<T> begin();
        //template<typename T> CMyMatConstIterator_<T> begin() const;
        //template<typename T> CMyMatIterator_<T> end();
        //template<typename T> CMyMatConstIterator_<T> end() const;
        //template<typename T, typename Functor> void forEach(const Functor& operation);
        //template<typename T, typename Functor> void forEach(const Functor& operation) const;
        
        
        //template<typename T> inline
        //CMyMatConstIterator<T> Begin() const
        //{
        //    return CMyMatConstIterator<T>(reinterpret_cast<const CMyMat<T>*>(this));
        //}
        
        //template<typename T> inline
        //CMyMatConstIterator<T> End() const
        //{
        //    CMyMatConstIterator<T> it(reinterpret_cast<const CMyMat<T>*>(this));
        //    it += total();
        //    return it;
        //}
        
        //template<typename T> inline
        //CMyMatIterator<T> Begin()
        //{
        //    return CMyMatConstIterator<T>(reinterpret_cast<CMyMat<T>*>(this));
        //}
        
        //template<typename T> inline
        //CMyMatIterator<T> End()
        //{
        //    CMyMatIterator<T> it(reinterpret_cast<CMyMat<T>*>(this));
        //    it += total();
        //    return it;
        //}
        
    };
    
    // CMyMat_<T>は1channelのみに特化している
    template<typename T>
    class CMyMat_ : public CMyMat
    {
    public:
        //typedef CMyMatIterator_<T> iterator;
        //typedef CMyMatConstIterator_<T> const_iterator;
        
        // constructor
        CMyMat_();
        CMyMat_(int rows, int cols);
        CMyMat_(int rows, int cols, const T& lum_value);
        CMyMat_(CMySize size);
        CMyMat_(CMySize size, const T& value);
        CMyMat_(const CMyMat_& mat_, const CMyRect& roi);
        CMyMat_(const CMyMat_& mat_); // copy constructor
        CMyMat_(const CMyMat& mat);
        CMyMat_(CMyMat_&& mat_) noexcept; // move constructor
        CMyMat_(CMyMat&& mat) noexcept;
        
        // operator
        template<typename U> operator CMyMat_<U> () const; // type conversion operator
        CMyMat_  operator () (const CMyRect& roi) const;
        CMyMat_& operator = (const CMyMat_& mat_); // copy assignment operator
        //CMyMat_& operator = (const CMyMat& mat);
        CMyMat_& operator = (CMyMat_&& mat_);      // move assignment operator
        //CMyMat_& operator = (CMyMat&& mat);
        CMyMat_& operator = (const T& lum_value);
        const T* operator [] (int row) const;
        T*       operator [] (int row);
        const T& operator () (int row) const;
        T&       operator () (int row);
        const T& operator () (int row, int col) const;
        T&       operator () (int row, int col);
        const T& operator () (CMyPoint pt) const;
        T&       operator () (CMyPoint pt);
       
        // function
        void CreateMat(int rows, int cols); // same: CMyMat::CreateMat(rows, cols, CDataType<T>::type)
        void CreateMat(CMySize size); // same: CMyMat::CreateMat(size, CDataType<T>::type)
        
        // function
        CMyMat_ Clone() const;
        CMyMat_& AdjustROI(int top, int bottom, int left, int right);
        int Type() const;
        int TypeSize() const;
        int Depth() const;
        int Channels() const;
                
        // function
        //iterator begin();
        //iterator end();
        //const_iterator begin() const;
        //const_iterator end() const;
        //template<typename T, typename Functor> void forEach(const Functor& operation);
        //template<typename T, typename Functor> void forEach(const Functor& operation) const;
        

    };
    
    
    /*
    // テンプレートの明示インスタンス化
    // この宣言型のみ使える
    template class CMyMat<BYTE>;
    //template class CMyMat<unsigned char>;
    template class CMyMat<char>;
    template class CMyMat<unsigned short>;
    template class CMyMat<short>;
    template class CMyMat<unsigned int>;
    template class CMyMat<int>;
    template class CMyMat<float>;
    template class CMyMat<double>;
     */
}

#include "mat_ini.h"

#endif /* mat_h */
