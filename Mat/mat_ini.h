//
//  mat_ini.h
//  matProject
//
//  Created by Inoue Shinichi on 2018/12/28.
//  Copyright © 2018 Inoue Shinichi. All rights reserved.
//

#ifndef mat_ini_h
#define mat_ini_h

#include <iostream>
#include <assert.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <bitset>


namespace my
{
    
    ////////////////////////// CMyMat /////////////////////////
    
    // ok
    inline
    CMyMat::CMyMat()
    : m_flags(MAGIC_VAL)
    , m_rows(0)
    , m_cols(0)
    , m_memStep(0)
    , m_oriSize(0, 0)
    , m_pImgData(nullptr)
    , m_pImgDataStart(nullptr)
    , m_pImgDataEnd(nullptr)
    , m_pRefCount(nullptr)
    {
        // Constructor
        std::cout << "constructor_0" << std::endl;
    };
    
    // ok
    inline
    CMyMat::CMyMat(int rows, int cols, int type)
    : m_flags(MAGIC_VAL)
    , m_rows(0)
    , m_cols(0)
    , m_memStep(0)
    , m_oriSize(0, 0)
    , m_pImgData(nullptr)
    , m_pImgDataStart(nullptr)
    , m_pImgDataEnd(nullptr)
    , m_pRefCount(nullptr)
    {
        // Constructor
        std::cout << "constructor_1" << std::endl;
        CreateMat(cols, rows, type);
    };
    
    // ok
    inline
    CMyMat::CMyMat(CMySize size, int type)
    : m_flags(MAGIC_VAL)
    , m_rows(0)
    , m_cols(0)
    , m_memStep(0)
    , m_oriSize(0, 0)
    , m_pImgData(nullptr)
    , m_pImgDataStart(nullptr)
    , m_pImgDataEnd(nullptr)
    , m_pRefCount(nullptr)
    {
        // Constructor
        std::cout << "constructor_2" << std::endl;
        CreateMat(size.m_width, size.m_height, type);
    };
    
    // ok
    inline
    CMyMat::CMyMat(const CMyMat& mat, const CMyRect& roi)
    : m_flags(mat.m_flags | MY_SUBMAT_FLAG) // bit演算 (or)
    , m_rows(roi.m_height)
    , m_cols(roi.m_width)
    , m_memStep(mat.m_memStep)
    , m_oriSize(mat.m_oriSize)
    , m_pImgData(const_cast<byte*>(mat.Ptr<byte>(roi.m_top, roi.m_left))) // ※
    , m_pImgDataStart(mat.m_pImgDataStart) // ※
    , m_pImgDataEnd(mat.m_pImgDataEnd) // ※
    , m_pRefCount(mat.m_pRefCount)
    {
        // Constructor
        std::cout << "constructor_3_ref" << std::endl;
        
        // m_pImgDataを共有する仕様にしている・・・※
        assert(IsValidType(mat.Type()) && "An Invalid Image Type Exception !");
        assert(mat.IsEmpty() == false && "m_pImgDataStart of mat is nullptr !");
        
        // matが以下の3パターンのどれでも対応可能
        // (1): ROI (sharrow copied) mat
        // (2): sharrow copied mat
        // (3): native mat

        // roi memStep
        //size_t tmp = mat.MemStep();
        //size_t tmp_step = (size_t)((mat.MemStep() / mat.m_cols) * m_cols);
        m_memStep[1] = (size_t)((mat.MemStep() / mat.m_cols) * m_cols);
        //size_t out_step = m_memStep[1];
        
        // 参照カウンタのインクリメント
        *m_pRefCount += 1;
    };
    
    // ok
    inline
    CMyMat::CMyMat(const CMyMat& mat)
    : m_flags(mat.m_flags)
    , m_rows(mat.m_rows)
    , m_cols(mat.m_cols)
    , m_memStep(mat.m_memStep)
    , m_oriSize(mat.m_oriSize)
    , m_pImgData(mat.m_pImgData)
    , m_pImgDataStart(mat.m_pImgDataStart)
    , m_pImgDataEnd(mat.m_pImgDataEnd)
    , m_pRefCount(mat.m_pRefCount)
    {
        // Copy Constructor
        std::cout << "copy constructor_ref" << std::endl;
        // m_pImgDataを共有する仕様にしている・・・※
        *m_pRefCount += 1;
    };
    
    
    inline
    CMyMat::CMyMat(CMyMat&& mat) noexcept
    : m_flags(mat.m_flags)
    , m_rows(mat.m_rows)
    , m_cols(mat.m_cols)
    , m_memStep(mat.m_memStep)
    , m_oriSize(mat.m_oriSize)
    , m_pImgData(mat.m_pImgData)
    , m_pImgDataStart(mat.m_pImgDataStart)
    , m_pImgDataEnd(mat.m_pImgDataEnd)
    , m_pRefCount(mat.m_pRefCount)
    {
        // Move Constructor
        std::cout << "move constructor_ref" << std::endl;
        
        mat.m_flags = MAGIC_VAL;
        mat.m_rows = 0;
        mat.m_cols = 0;
        mat.m_memStep[0] = mat.m_memStep[1] = 0;
        mat.m_oriSize[0] = mat.m_oriSize[1] = 0;
        mat.m_pImgData = nullptr;
        mat.m_pImgDataStart = nullptr;
        mat.m_pImgDataEnd = nullptr;
        mat.m_pRefCount = nullptr;
    };
    
    //ok
    inline
    CMyMat::~CMyMat()
    {
        std::cout << "destructor" << std::endl;
        ReleaseMat();
    };
    
    // ok
    inline
    CMyMat CMyMat::operator () (const CMyRect& roi) const
    {
        std::cout << "ROI operator" << std::endl;
        // ROIしたmatを返す。データは共有する
        return CMyMat(*this, roi);
    };
    
    // ok
    inline
    CMyMat& CMyMat::operator = (const CMyMat& mat)
    {
        // コピー代入演算子
        std::cout << "Copy assignment operator" << std::endl;
       
        // 自己代入対策
        if (this != &mat)
        {
            assert(IsValidType(mat.Type()) && "An Invalid Image Type Exception !");
            
            // 代入される側のmatが参照生成でなければ、m_pImgDataが迷子になる前に解放
            // 参照生成されているmatなら参照カウンタを一つ減らす
            ReleaseMat();
            
            m_flags = mat.m_flags;
            m_rows = mat.m_rows;
            m_cols = mat.m_cols;
            m_memStep = mat.m_memStep;
            
            // m_pImgDataを共有する仕様にしている・・・※
            m_pImgData = mat.m_pImgData; // *
            m_pImgDataStart = mat.m_pImgDataStart; // *
            m_pImgDataEnd = mat.m_pImgDataEnd; // *
            
            m_pRefCount = mat.m_pRefCount;
            *m_pRefCount += 1;
        }
        return *this;
    };
    
    inline
    CMyMat& CMyMat::operator = (CMyMat&& mat)
    {
        // ムーブ代入演算子
        std::cout << "Move assignment operator" << std::endl;
        
        if (this != &mat)
        {
            ReleaseMat();
            
            m_flags = mat.m_flags;
            m_rows = mat.m_rows;
            m_cols = mat.m_cols;
            m_memStep = mat.m_memStep;
            m_oriSize = mat.m_oriSize;
            m_pImgData = mat.m_pImgData;
            m_pImgDataStart = mat.m_pImgDataStart;
            m_pImgDataEnd = mat.m_pImgDataEnd;
            m_pRefCount = mat.m_pRefCount;
            
            mat.m_flags = MAGIC_VAL;
            mat.m_rows = 0;
            mat.m_cols = 0;
            mat.m_memStep[0] = mat.m_memStep[1] = 0;
            mat.m_oriSize[0] = mat.m_oriSize[1] = 0;
            mat.m_pImgData = nullptr;
            mat.m_pImgDataStart = nullptr;
            mat.m_pImgDataEnd = nullptr;
            mat.m_pRefCount = nullptr;
        }
        return *this;
    };
    
    // ok
    inline
    void CMyMat::CreateMat(int rows, int cols, int type)
    {
        std::cout << "CreateMat(int cols, int rows, int type)" << std::endl;
        
        // 画像規格をチェック
        assert(rows > 0);
        assert(cols > 0);
        m_rows = rows;
        m_cols = cols;
        
        type &= TYPE_MASK; // bit演算 (type抽出)
        assert(IsValidType(type) && "An Invalid Image Type Exception !");
        
        m_flags |= type; // bit演算 (type情報追加)
        m_memStep[0] = TypeSize() * Channels() * m_cols;  // バイト単位のcol方向のサイズ
        
        m_oriSize[0] = m_rows;
        m_oriSize[1] = m_cols;
        
        // 画像メモリ確保
        m_pImgData = new byte[TotalMemSize()];
        memset(m_pImgData, 0, TotalMemSize());     // 0詰め初期化
            
        m_pImgDataStart = m_pImgData;
        m_pImgDataEnd = &m_pImgData[TotalMemSize()];// last_index + 1 in an Unit of byte type
            
        m_pRefCount = new int;                      // 参照カウンタの確保
        *m_pRefCount = 0;
    };
    
    // ok
    inline
    void CMyMat::ReleaseMat()
    {
        if (m_pRefCount)
        {
            std::cout << "*m_pRefCount=" << *m_pRefCount << std::endl;
            
            if (*m_pRefCount > 0)
            {
                // 参照カウンタを一つ外す
                *m_pRefCount -= 1;
            }
            else
            {
                // 参照カウンタが0の時に解放
                delete[] m_pImgDataStart;
                m_pImgDataStart = nullptr;
                m_pImgData = nullptr;
                m_pImgDataEnd = nullptr;
                delete m_pRefCount;
                m_pRefCount = nullptr;
            }
        }
        else
            std::cout << "*m_pRefCount=" << "NULL" << std::endl;
    };
    
    // ok
    inline
    bool CMyMat::IsEmpty() const
    {
        if (m_pImgDataStart == nullptr)
            return true;
        else
            return false;
    };
    
    // ok
    inline
    void CMyMat::CopyTo(CMyMat& mat) const
    {
        // Deep Copy
        mat.CreateMat(m_cols, m_rows, Type());
        memcpy(const_cast<byte*>(mat.m_pImgDataStart), m_pImgDataStart, m_memStep[0]); // All Memory Copy
        
        // コピー元がSubmatrixの場合、フラグを継承
        if (IsSubmatrix())
        {
            mat.m_flags |= MY_SUBMAT_FLAG;
            mat.m_memStep[0] = m_memStep[0];
            mat.m_memStep[1] = m_memStep[1];
            mat.m_oriSize[0] = m_oriSize[0];
            mat.m_oriSize[1] = m_oriSize[1];
        }
    };
    
    // ok
    inline
    CMyMat CMyMat::Clone() const
    {
        // 同じリソースを複製
        CMyMat clone_mat;
        CopyTo(clone_mat);
        
        // 関数戻り値がオブジェクトの場合、コピーコンストラクタが呼ばれてメンバ変数がコピーされて関数の呼び出し元に戻るが、
        // CMyMatのコピーコンストラクタでは、*m_refCountをインクリメントさせているので、clone_matはこの関数スコープを
        // 抜けても、m_pImgDataStartが解放されない。
        return clone_mat;
    };
    
    inline
    void CMyMat::ShowState() const
    {
        std::cout << "Mat State is below." << std::endl;
        std::cout << "m_rows: " << m_rows << std::endl;
        std::cout << "m_cols: " << m_cols << std::endl;
        std::cout << "m_flags: ";
        std::cout << "(10)" << std::dec << m_flags;
        std::cout << " 0x"  << std::hex << m_flags;
        std::cout << " 0b"  << std::bitset<32>(m_flags) << std::endl;
        
        std::cout << std::dec;
        std::cout << "Type: "     << Type() << " " << ShowType(Type()) << std::endl;
        std::cout << "TypeSize: " << TypeSize() << "byte" << std::endl;
        std::cout << "Depth: "    << Depth() << " " << ShowDepth(Depth()) << std::endl;
        std::cout << "Channels: " << Channels() << std::endl;
        
        std::cout << "IsEmpty: "      << IsEmpty() << std::endl;
        std::cout << "IsSubmatrix: "  << IsSubmatrix() << std::endl;
        
        std::cout << "m_memStep[0]: " << m_memStep[0] << " byte" << std::endl;
        std::cout << "m_memStep[1]: " << m_memStep[1] << " byte" << std::endl;
        
        std::cout << "m_oriSize[0]: " << m_oriSize[0] << std::endl;
        std::cout << "m_oriSize[1]: " << m_oriSize[1] << std::endl;
        
        std::cout << "m_pImgData: "      << reinterpret_cast<void *>(m_pImgData) << std::endl;
        std::cout << "m_pImgDataStart: " << reinterpret_cast<const void *>(m_pImgDataStart) << std::endl;
        std::cout << "m_pImgDataEnd: "   << reinterpret_cast<const void *>(m_pImgDataEnd) << std::endl;
        
        std::cout << "m_pRefCount: "  << reinterpret_cast<void *>(m_pRefCount) << std::endl;
        std::cout << "*m_pRefCount: " << *m_pRefCount << std::endl;
        std::cout << std::endl;
        
    };
    
    // ok
    inline
    bool CMyMat::IsValidType(int type) const
    {
        if (type == MY_8UC1 || type == MY_8UC2 || type == MY_8UC3 || type == MY_8UC4 ||
            type == MY_8SC1 || type == MY_8SC2 || type == MY_8SC3 || type == MY_8SC4 ||
            type == MY_16UC1 || type == MY_16UC2 || type == MY_16UC3 || type == MY_16UC4 ||
            type == MY_16SC1 || type == MY_16SC2 || type == MY_16SC3 || type == MY_16SC4 ||
            //type == MY_32UC1 || type == MY_32UC2 || type == MY_32UC3 || type == MY_32UC4 ||
            type == MY_32SC1 || type == MY_32SC2 || type == MY_32SC3 || type == MY_32SC4 ||
            //type == MY_64SC1 || type == MY_64SC2 || type == MY_64SC3 || type == MY_64SC4 ||
            //type == MY_64UC1 || type == MY_64UC2 || type == MY_64UC3 || type == MY_64UC4 ||
            type == MY_32FC1 || type == MY_32FC2 || type == MY_32FC3 || type == MY_32FC4 ||
            type == MY_64FC1 || type == MY_64FC2 || type == MY_64FC3 || type == MY_64FC4)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
    
    // ok
    inline
    bool CMyMat::IsSubmatrix() const
    {
        // ROI or NOT
        return ((m_flags & MY_SUBMAT_FLAG) != 0); // bit演算 (Submatrixフラグ抽出)
    };
    
    // ok
    inline
    int CMyMat::Channels() const
    {
        return MY_MAT_CN(m_flags);
    };
    
    // ok
    inline
    int CMyMat::Type() const
    {
        return MY_MAT_TYPE(m_flags);
    };
    
    // ok
    inline
    std::string CMyMat::ShowType(int type) const
    {
        std::string typeStr; // 空の文字列
        
        // MY_8U
        if (type == MY_8UC1) typeStr = "MY_8UC1"; else if (type == MY_8UC2) typeStr = "MY_8UC2";
        else if (type == MY_8UC3) typeStr = "MY_8UC3"; else if (type == MY_8UC4) typeStr = "MY_8UC4";
        // MY_8S
        else if (type == MY_8SC1) typeStr = "MY_8SC1"; else if (type == MY_8SC2) typeStr = "MY_8SC2";
        else if (type == MY_8SC3) typeStr = "MY_8SC3"; else if (type == MY_8SC4) typeStr = "MY_8SC4";
        // MY_16U
        else if (type == MY_16UC1) typeStr = "MY_16UC1"; else if (type == MY_16UC2) typeStr = "MY_16UC2";
        else if (type == MY_16UC3) typeStr = "MY_16UC3"; else if (type == MY_16UC4) typeStr = "MY_16UC4";
        // MY_16S
        else if (type == MY_16SC1) typeStr = "MY_16SC1"; else if (type == MY_16SC2) typeStr = "MY_16SC2";
        else if (type == MY_16SC3) typeStr = "MY_16SC3"; else if (type == MY_16SC4) typeStr = "MY_16SC4";
        // MY_32S
        else if (type == MY_32SC1) typeStr = "MY_32SC1"; else if (type == MY_32SC2) typeStr = "MY_32SC2";
        else if (type == MY_32SC3) typeStr = "MY_32SC3"; else if (type == MY_32SC4) typeStr = "MY_32SC4";
        // MY_32F
        else if (type == MY_32FC1) typeStr = "MY_32FC1"; else if (type == MY_32FC2) typeStr = "MY_32FC2";
        else if (type == MY_32FC3) typeStr = "MY_32FC3"; else if (type == MY_32FC4) typeStr = "MY_32FC4";
        // MY_64F
        else if (type == MY_64FC1) typeStr = "MY_64FC1"; else if (type == MY_64FC2) typeStr = "MY_64FC2";
        else if (type == MY_64FC3) typeStr = "MY_64FC3"; else if (type == MY_64FC4) typeStr = "MY_64FC4";
        // InValid Type
        else typeStr = "InValid Type";
        
        return typeStr;
    };
    
    // ok
    inline
    int CMyMat::TypeSize() const
    {
        int depth = Depth();
        int typeSize = 0;
        
        if      (depth == Depth::MY_8U  || depth == Depth::MY_8S)
            typeSize = 1; // 1byte
        else if (depth == Depth::MY_16U || depth == Depth::MY_16S)
            typeSize = 2; // 2byte
        else if (depth == Depth::MY_32S || depth == Depth::MY_32F)
            typeSize = 4; // 4byte
        else
            typeSize = 8; // 8byte
        
        return typeSize;
    }
    
    // ok
    inline
    int CMyMat::Depth() const
    {
        return MY_MAT_DEPTH(m_flags);
    };
    
    // ok
    inline
    std::string CMyMat::ShowDepth(int depth) const
    {
        std::string depthStr; // 空の文字列
        if (depth == MY_8U) depthStr = "MY_8U";
        else if (depth == MY_8S) depthStr = "MY_8S";
        else if (depth == MY_16U) depthStr = "MY_16U";
        else if (depth == MY_16S) depthStr = "MY_16S";
        else if (depth == MY_32S) depthStr = "MY_32S";
        else if (depth == MY_32F) depthStr = "MY_32F";
        else if (depth == MY_64F) depthStr = "MY_64F";
        else depthStr = "InValid Depth";
        
        return depthStr;
    };
    
    // ok
    inline
    int CMyMat::Cols() const
    {
        return m_cols;
    }; // channelを無視した(1channelとした)場合の行方向ステップサイズ
    
    // ok
    inline
    int CMyMat::Rows() const
    {
        return m_rows;
    };
    
    // ok
    inline
    size_t CMyMat::ElemStep() const
    {
        return MemStep() / TypeSize();
    };
    
    // ok
    inline
    size_t CMyMat::ElemSize() const
    {
        return m_rows * ElemStep();
    };
    
    // ok
    inline
    size_t CMyMat::MemStep() const
    {
        if (IsSubmatrix())
            return m_memStep[1]; // roi step
        else
            return m_memStep[0]; // native step
    };
    
    // ok
    inline
    size_t CMyMat::MemSize() const
    {
        return m_rows * MemStep();
    };
    
    
    inline
    size_t CMyMat::TotalMemSize() const
    {
        return m_oriSize[0] * m_oriSize[1] * Channels() * TypeSize();
    }
    
    // ok
    inline
    CMyMat& CMyMat::AdjustROI(int top, int bottom, int left, int right)
    {
        assert(IsEmpty() == false && "m_pImgDataStart of mat is nullptr !");
        assert(top > 0 && "top must be > 0 !");
        assert(bottom < m_rows && "bottom must be < m_rows !");
        assert(top < bottom && "top must be less than bottom !");
        assert(left > 0 && "left must be > 0 !");
        assert(right < m_cols && "right must be < m_cols !");
        assert(left < right && "left must be less than right !");
        assert(*m_pRefCount == 0 && "mat must be native !");
        
        // 参照カウンタが乗っているmatはAjustROIできない
        
        int tmp_cols = right - left + 1;
        int tmp_rows = bottom - top + 1;
        size_t tmp_memStep = (MemStep() / m_cols) * tmp_cols;
        size_t tmp_memSize = tmp_rows * tmp_memStep;
        byte* tmp_pImgData = new byte[tmp_memSize];
        
        for (int y = 0, yy = 0; y < tmp_rows; y++, yy = y + top)
            memcpy(&tmp_pImgData[y * tmp_memStep], this->Ptr<byte>(yy, left), tmp_memStep);
        
        delete[] m_pImgData;
        m_pImgData = tmp_pImgData; // ROIメモリのポインタの受け渡し
        m_pImgDataStart = m_pImgData;
        m_pImgDataEnd = &m_pImgData[tmp_memSize]; // last_index + 1
        delete[] tmp_pImgData;
        tmp_pImgData = nullptr;
        
        m_cols = tmp_cols;
        m_rows = tmp_rows;
        m_memStep[0] = m_memStep[1] = tmp_memStep;
        
        return *this;
    };
    
    // ok
    inline
    const byte* CMyMat::BPtr(size_t memIdx) const
    {
        // 1dim byte pointer
        // only-read
        assert(IsEmpty() == false && "m_pImgDataStart of mat is nullptr !");
        assert(memIdx >= 0);
        assert(memIdx < MemSize());
        
        if (IsSubmatrix())
        {
            // m_pImgDataはroiの先頭ポインタ
            int roi_row = (int)(memIdx / m_memStep[1]);
            size_t roi_x = memIdx % m_memStep[1];
            return &m_pImgData[roi_row * m_memStep[0] + roi_x];
        }
        else
        {
            return &m_pImgData[memIdx];
        }
    };
    
    // ok
    inline
    byte* CMyMat::BPtr(size_t memIdx)
    {
        // 1dim byte pointer
        // const unsigned char * -> unsigned char * にconstをはずす
        return const_cast<byte*>((static_cast<const CMyMat&>(*this)).BPtr(memIdx)); // constオブジェクトのBPtr()を呼ぶ
    };
    
    // ok
    template <typename T> inline
    const T* CMyMat::Ptr(int row) const
    {
        // 1dim pointer
        // only-read
        // Y方向のX方向0を起点とする型Tの持つポインタを返す
        assert(IsEmpty() == false && "m_pImgDataStart of mat is nullptr !");
        assert(row >= 0);
        assert(row < m_rows);
        
        if (IsSubmatrix())
            return reinterpret_cast<const T*>(&m_pImgData[row * m_memStep[0]]);
        else
            return reinterpret_cast<const T*>(&m_pImgData[row * m_memStep[1]]);
    };
    
    // ok
    template <typename T> inline
    T* CMyMat::Ptr(int row)
    {
        // 1dim pointer
        // Y方向のX方向0を起点とする型Tの持つポインタを返す
        // const T* -> T* にconstをはずす
        return const_cast<T*>((static_cast<const CMyMat&>(*this)).Ptr<T>(row)); // constオブジェクトのPtr()を呼ぶ
                       
    };
    
    // ok
    template <typename T> inline
    const T* CMyMat::Ptr(int row, int col) const
    {
        // 2dim pointer
        // only-read
        assert(IsEmpty() == false && "m_pImgDataStart of mat is nullptr !");
        assert(row >= 0);
        assert(col >= 0);
        assert(row < m_rows);
        assert(col < m_cols);
        int typeSize = sizeof(T);
        
        if (IsSubmatrix())
            return reinterpret_cast<const T*>(&m_pImgData[row * m_memStep[1] + col * Channels() * typeSize]);
        else
            return reinterpret_cast<const T*>(&m_pImgData[row * m_memStep[0] + col * Channels() * typeSize]);
    };
    
    // ok
    template <typename T> inline
    T* CMyMat::Ptr(int row, int col)
    {
        // 2dim pointer
        return const_cast<T*>((static_cast<const CMyMat&>(*this)).Ptr<T>(row, col));
    };
    
    // ok
    template <typename T> inline
    const T* CMyMat::Ptr(int row, int col, int channel) const
    {
        // 3dim pointer
        // read-only
        assert(IsEmpty() == false && "m_pImgDataStart of mat is nullptr !");
        assert(row >= 0);
        assert(col >= 0);
        assert(row < m_rows);
        assert(col < m_cols);
        assert(channel >= 0);
        assert(channel < Channels());
        int typeSize = sizeof(T);
        
        if (IsSubmatrix())
            return reinterpret_cast<const T*>(&m_pImgData[row * m_memStep[1] + (col * Channels() + channel) * typeSize]);
        else
            return reinterpret_cast<const T*>(&m_pImgData[row * m_memStep[0] + (col * Channels() + channel) * typeSize]);
    };
    
    // ok
    template <typename T> inline
    T* CMyMat::Ptr(int row, int col, int channel)
    {
        // 3dim pointer
        return const_cast<T*>((static_cast<const CMyMat&>(*this)).Ptr<T>(row, col, channel));
    }
    
    // ok
    template <typename T> inline
    const T& CMyMat::At(int row, int col, int channel) const
    {
        // 3dim indexing
        // only-read
        return *((static_cast<const CMyMat&>(*this)).Ptr<T>(row, col, channel));
    };
    
    // ok
    template <typename T> inline
    T& CMyMat::At(int row, int col, int channel)
    {
        // 3dim indexing
        return const_cast<T&>((static_cast<const CMyMat&>(*this)).At<T>(row, col, channel));
    };
    
    // ok
    template <typename T> inline
    const T& CMyMat::At(int row, int col) const
    {
        // 2dim indexing
        // only-read
        return *((static_cast<const CMyMat&>(*this)).Ptr<T>(row, col));
    };
    
    // ok
    template <typename T> inline
    T& CMyMat::At(int row, int col)
    {
        // 2dim indexing
        return const_cast<T&>((static_cast<const CMyMat&>(*this)).At<T>(row, col));
    };
    
    // ok
    template <typename T> inline
    const T& CMyMat::At(int row) const
    {
        // 1dim indexing
        // only-read
        return *((static_cast<const CMyMat&>(*this)).Ptr<T>(row));
    };
    
    // ok
    template <typename T> inline
    T& CMyMat::At(int row)
    {
        // 1dim indexing
        return const_cast<T&>((static_cast<const CMyMat&>(*this)).At<T>(row));
    };
    
    // ok
    inline
    const byte& CMyMat::BAt(size_t memIdx) const
    {
        // 1dim byte indexing
        // only-read
        return *((static_cast<const CMyMat&>(*this)).BPtr(memIdx));
    };
    
    // ok
    inline
    byte& CMyMat::BAt(size_t memIdx)
    {
        // 1dim byte indexing
        return const_cast<byte&>((static_cast<const CMyMat&>(*this)).BAt(memIdx));
    };
    
    
    template<typename T> inline
    const T* CMyMat::Ptr(CMyPoint pt) const
    {
        // 2dim pointer
        // only-read
        assert(IsEmpty() == false && "m_pImgDataStart of mat is nullptr !");
        assert(pt.m_y >= 0);
        assert(pt.m_x >= 0);
        assert(pt.m_y < m_rows);
        assert(pt.m_x < m_cols);
        int typeSize = sizeof(T);
        
        if (IsSubmatrix())
            return reinterpret_cast<const T*>(&m_pImgData[pt.m_y * m_memStep[1] + pt.m_x * Channels() * typeSize]);
        else
            return reinterpret_cast<const T*>(&m_pImgData[pt.m_y * m_memStep[0] + pt.m_x * Channels() * typeSize]);
    };
    
    
    template<typename T> inline
    T* CMyMat::Ptr(CMyPoint pt)
    {
        // 2dim pointer
        return const_cast<T*>((static_cast<const CMyMat&>(*this)).Ptr<T>(pt));
    };
    
    
    template<typename T> inline
    const T& CMyMat::At(CMyPoint pt) const
    {
        // 2dim slicing
        // read-only
        return *((static_cast<const CMyMat&>(*this)).Ptr<T>(pt));
    };
    
    
    template<typename T> inline
    T& CMyMat::At(CMyPoint pt)
    {
        // 2dim slicing
        return const_cast<T&>((static_cast<const CMyMat&>(*this)).At<T>(pt));
    };
    
    
    template<typename T> inline
    const T* CMyMat::Ptr(CMyPoint pt, int channel) const
    {
        // 2dim pointer
        // only-read
        assert(IsEmpty() == false && "m_pImgDataStart of mat is nullptr !");
        assert(pt.m_y >= 0);
        assert(pt.m_x >= 0);
        assert(pt.m_y < m_rows);
        assert(pt.m_x < m_cols);
        assert(channel >= 0);
        assert(channel < Channels());
        int typeSize = sizeof(T);
        
        if (IsSubmatrix())
            return reinterpret_cast<const T*>(&m_pImgData[pt.m_y * m_memStep[1] + pt.m_x * channel * typeSize]);
        else
            return reinterpret_cast<const T*>(&m_pImgData[pt.m_y * m_memStep[0] + pt.m_x * channel * typeSize]);
    };
    
    
    template<typename T> inline
    T* CMyMat::Ptr(CMyPoint pt, int channel)
    {
        // 2dim pointer
        return const_cast<T*>((static_cast<const CMyMat&>(*this)).Ptr<T>(pt, channel));
    };
    
    
    template<typename T> inline
    const T& CMyMat::At(CMyPoint pt, int channel) const
    {
        // 2dim slicing
        // read-only
        return *((static_cast<const CMyMat&>(*this)).Ptr<T>(pt, channel));
    };
    
    
    template<typename T> inline
    T& CMyMat::At(CMyPoint pt, int channel)
    {
        // 2dim slicing
        return const_cast<T&>((static_cast<const CMyMat&>(*this)).At<T>(pt, channel));
    };
    
    
    
    
    
    
    
    
    
    
    ////////////////////////// CMyMat_<T> /////////////////////////
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_()
    : CMyMat()
    {
        // Constructor
        // CMyMat_<T>は宣言時に型指定をするので、m_flagsに型情報を格納する
        m_flags = (m_flags & ~MY_MAT_TYPE_MASK) + CDataType<T>::type;
    };
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(int rows, int cols)
    : CMyMat(rows, cols, CDataType<T>::type)
    {};
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(int rows, int cols, const T& lum_value)
    : CMyMat(rows, cols, CDataType<T>::type)
    {
        // Constructor
        *this = lum_value; // 全画素にlum_valueを格納
    };
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(CMySize size)
    : CMyMat(size.m_height, size.m_width, CDataType<T>::type)
    {
        // Constructor
    };
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(CMySize size, const T& lum_value)
    : CMyMat(size.m_height, size.m_width, CDataType<T>::type)
    {
        // Constructor
        *this = lum_value; // 全画素にlum_valueを格納
    };
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(const CMyMat_& mat_, const CMyRect& roi)
    : CMyMat(mat_, roi)
    {
        // Constructor
    };
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(const CMyMat_& mat_)
    : CMyMat(mat_)
    {
      // Copy Constructor
    };
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(const CMyMat& mat)
    : CMyMat(mat)
    {
        // Copy Constructor
        // CMyMat_<T>は宣言時に型指定をするので、m_flagsに型情報を格納する
        m_flags = (m_flags & ~MY_MAT_TYPE_MASK) + CDataType<T>::type;
    };
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(CMyMat_&& mat_) noexcept
    : CMyMat(std::move(mat_))
    {
        // Move Constructor
    };
    
    
    template<typename T> inline
    CMyMat_<T>::CMyMat_(CMyMat&& mat) noexcept
    : CMyMat(std::move(mat))
    {
        // Move Constructor
        // CMyMat_<T>は宣言時に型指定をするので、m_flagsに型情報を格納する
        m_flags = (m_flags & ~MY_MAT_TYPE_MASK) + CDataType<T>::type;
    };
    
    
    template<typename T> template<typename U> inline
    CMyMat_<T>::operator CMyMat_<U> () const
    {
        // Type Conversion Operator
        return CMyMat_<U>(*this);
    };
    
    
    template<typename T> inline
    CMyMat_<T> CMyMat_<T>::operator () (const CMyRect& roi) const
    {
        return static_cast<CMyMat_<T>>(*this, roi);
    };
    
    
    template<typename T> inline
    CMyMat_<T>& CMyMat_<T>::operator = (const CMyMat_& mat_)
    {
        CMyMat::operator = (mat_);
        return *this;
    };
    
    
    //template<typename T> inline
    //CMyMat_<T>& CMyMat_<T>::operator = (const CMyMat& mat)
    //{
    //    // Typeが一致する場合
    //    if (CDataType<T>::type == mat.Type())
    //    {
    //        CMyMat::operator = (mat);
    //        return *this;
    //    }
    //
    //    // Depthは一致するが、Channelsが一致しない場合
    //    if (CDataType<T>::depth == mat.Depth())
    //    {
    //        // Channelを増減する処理をいれる?
    //    }
    //
    //};
    
    
    template<typename T> inline
    CMyMat_<T>& CMyMat_<T>::operator = (CMyMat_&& mat_)
    {
        CMyMat::operator = (std::move(mat_));
        return *this;
    };
    
    
    //template<typename T> inline
    //CMyMat_<T>& CMyMat_<T>::operator = (CMyMat&& mat)
    //{
    //    // Typeが一致する場合
    //    if (CDataType<T>::type == mat.Type())
    //    {
    //        CMyMat::operator = (std::move(mat));
    //        return *this;
    //    }
    //
    //    // Depthは一致するが、Channelsが一致しない場合
    //    if (CDataType<T>::depth == mat.Depth())
    //    {
    //        // Channelを増減する処理をいれる?
    //    }
    //};
    
    
    template<typename T> inline
    CMyMat_<T>& CMyMat_<T>::operator = (const T& lum_value)
    {
        int rows = this->m_rows;
        int cols = this->m_cols;
        int channel = this->Channels();
        
        for (int y = 0; y < rows; y++)
            for (int x = 0; x < cols; x++)
                for (int c = 0; c < channel; c++)
                    this->At<T>(y, x, c) = lum_value;
        
        return *this;
    };
    
    
    template<typename T> inline
    const T* CMyMat_<T>::operator [] (int row) const
    {
        return (static_cast<const CMyMat_<T>&>(*this)).Ptr<T>(row);
    };
    
    
    template<typename T> inline
    T* CMyMat_<T>::operator [] (int row)
    {
        return const_cast<T*>((static_cast<const CMyMat_<T>&>(*this)).Ptr<T>(row));
    };
    
    
    template<typename T> inline
    const T& CMyMat_<T>::operator () (int row) const
    {
        return (static_cast<const CMyMat_<T>&>(*this)).At<T>(row);
    };
    
    
    template<typename T> inline
    T& CMyMat_<T>::operator () (int row)
    {
        return const_cast<T&>((static_cast<const CMyMat_<T>&>(*this)).At<T>(row));
    };
    
    
    template<typename T> inline
    const T& CMyMat_<T>::operator () (int row, int col) const
    {
        return (static_cast<const CMyMat_<T>&>(*this)).At<T>(row, col);
    };
    
    
    template<typename T> inline
    T& CMyMat_<T>::operator () (int row, int col)
    {
        return const_cast<T&>((static_cast<const CMyMat_<T>&>(*this)).At<T>(row, col));
    };
    
    
    template<typename T> inline
    const T& CMyMat_<T>::operator () (CMyPoint pt) const
    {
        return (static_cast<const CMyMat_<T>&>(*this)).At<T>(pt);
    };
    
    
    template<typename T> inline
    T& CMyMat_<T>::operator () (CMyPoint pt)
    {
        return const_cast<T&>((static_cast<const CMyMat_<T>&>(*this)).At<T>(pt));
    };
    
    
    template<typename T> inline
    void CMyMat_<T>::CreateMat(int rows, int cols)
    {
        CMyMat::CreateMat(rows, cols, CDataType<T>::type);
    };
    
    
    template<typename T> inline
    void CMyMat_<T>::CreateMat(CMySize size)
    {
        CMyMat::CreateMat(size, CDataType<T>::type);
    };
    
    
    template<typename T> inline
    CMyMat_<T> CMyMat_<T>::Clone() const
    {
        return CMyMat_(CMyMat::Clone());
    };
    
    
    template<typename T> inline
    CMyMat_<T>& CMyMat_<T>::AdjustROI(int top, int bottom, int left, int right)
    {
        return static_cast<CMyMat_<T>&>(CMyMat::AdjustROI(top, bottom, left, right));
    };
    
    
    template<typename T> inline
    int CMyMat_<T>::Type() const
    {
        assert(CMyMat::Type() == CDataType<T>::type);
        return CDataType<T>::type;
    };
    
    
    template<typename T> inline
    int CMyMat_<T>::Depth() const
    {
        assert(CMyMat::Depth() == CDataType<T>::depth);
        return CDataType<T>::depth;
    };
    
    
    template<typename T> inline
    int CMyMat_<T>::Channels() const
    {
        assert(CMyMat::Channels() == CDataType<T>::channels);
        return CDataType<T>::channels; // 1 channelのみ
    };
    
    
    template<typename T> inline
    int CMyMat_<T>::TypeSize() const
    {
        assert(CMyMat::Type() == CDataType<T>::type);
        return CMyMat::TypeSize();
    };
}

#endif /* mat_ini_h */
