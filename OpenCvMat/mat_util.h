//
//  mat_util.h
//  matProject
//
//  Created by Inoue Shinichi on 2018/12/28.
//  Copyright © 2018 Inoue Shinichi. All rights reserved.
//

#ifndef mat_util_h
#define mat_util_h

#include <cmath>
#include <assert.h>

namespace my
{
    class CMyPoint
    {
    public:
        int m_x;
        int m_y;
        
    private:
        double m_r;
        
    public:
        CMyPoint()
        : m_x(0)
        , m_y(0)
        , m_r(0.0)
        {}
        
        CMyPoint(int x, int y)
        : m_x(x)
        , m_y(y)
        , m_r(sqrt(x * x + y * y))
        {}
        
        CMyPoint(const CMyPoint& p)
        : m_x(p.m_x)
        , m_y(p.m_y)
        , m_r(p.Dist())
        {// copy constructor
        }
        
        
        virtual ~CMyPoint() {}
        
        
        CMyPoint& operator=(const CMyPoint& p)
        {
            // コピー代入演算子
            
            // 自己代入対策
            if (this != &p)
            {
                m_x = p.m_x;
                m_y = p.m_y;
            }
            return *this;
        }
        
        
        friend CMyPoint operator+(const CMyPoint& p1, const CMyPoint& p2)
        {
            CMyPoint p;
            p.m_x = p1.m_x + p2.m_x;
            p.m_y = p1.m_x + p2.m_x;
            p.m_r = sqrt(p.m_x * p.m_x + p.m_y * p.m_y);
            return p;
        }
        
        
        friend CMyPoint operator-(const CMyPoint& p1, const CMyPoint& p2)
        {
            CMyPoint p;
            p.m_x = p1.m_x - p2.m_x;
            p.m_y = p1.m_y - p2.m_y;
            p.m_r = sqrt(p.m_x * p.m_x + p.m_y * p.m_y);
            return p;
        }
        
        
        friend bool operator==(CMyPoint& p1, CMyPoint& p2)
        {
            if ((p1.m_x == p2.m_x) && (p1.m_y == p2.m_y))
                return true;
            return false;
        }
        
        
        friend bool operator!=(CMyPoint& p1, CMyPoint& p2)
        {
            if ((p1.m_x != p2.m_x) && (p1.m_y != p2.m_y))
                return true;
            return false;
        }
        
        
        friend bool operator>(CMyPoint& p1, CMyPoint& p2)
        {
            if ((p1.m_x > p2.m_x) && (p1.m_y > p2.m_y))
                return true;
            return false;
        }
        
        
        friend bool operator>=(CMyPoint& p1, CMyPoint& p2)
        {
            if ((p1.m_x >= p2.m_x) && (p1.m_y >= p2.m_y))
                return true;
            return false;
        }
        
        
        friend bool operator<(CMyPoint& p1, CMyPoint& p2)
        {
            if ((p1.m_x < p2.m_x) && (p1.m_y < p2.m_y))
                return true;
            return false;
        }
        
        
        friend bool operator<=(CMyPoint& p1, CMyPoint& p2)
        {
            if ((p1.m_x <= p2.m_x) && (p1.m_y <= p2.m_y))
                return true;
            return false;
        }
        
        
        double Dist() const { return m_r; }
        
    };
    
    
    
    class CMySize
    {
    public:
        int m_width;
        int m_height;
        size_t m_area;
        
        CMySize()
        : m_width(0)
        , m_height(0)
        , m_area(0)
        {}
        
        CMySize(int width, int height)
        : m_width(width)
        , m_height(height)
        , m_area(width * height)
        {}
        
        CMySize(const CMySize& size)
        : m_width(size.m_width)
        , m_height(size.m_height)
        , m_area(size.m_area)
        {// copy constructor
        }
        
        virtual ~CMySize() {}
        
        CMySize& operator=(const CMySize& size)
        {
            // コピー代入演算子
            
            // 自己代入のチェック
            if (this != &size)
            {
                m_width = size.m_width;
                m_height = size.m_height;
                m_area = size.m_area;
            }
            return *this;
        }
        
        virtual inline int GetWidth() const { return m_width; }
        virtual inline int GetHeight() const { return m_height; }
        virtual inline size_t GetArea() const { return m_area; }
    };
    
    
    
    class CMyRect
    : public CMySize
    {
    public:
        int m_top;
        int m_left;
        int m_bottom;
        int m_right;
        int m_cx;
        int m_cy;
        
    public:
        
        CMyRect()
        : CMySize() // CMySizeのデフォルトコンストラクタが呼ばれる
        , m_top(0)
        , m_bottom(0)
        , m_left(0)
        , m_right(0)
        , m_cx(0)
        , m_cy(0)
        {}
        
        
        CMyRect(int top, int bottom, int left, int right)
        : CMySize()
        {
            assert(top <= bottom && "Top is more than Bottom !");
            assert(left <= right && "Left is more than Right !");
            
            m_top = top;
            m_bottom = bottom;
            m_left = left;
            m_right = right;
            m_cx = (int)((m_left + m_right) / 2);
            m_cy = (int)((m_top + m_bottom) / 2);
            
            if (m_top != m_bottom)
                m_width = m_right - m_left + 1;
            
            if (m_left != m_right)
                m_height = m_bottom - m_top + 1;
            
            m_area = m_width * m_height;
        }
        
        
        CMyRect(CMyPoint p1, CMyPoint p2)
        : CMySize()
        {
            if (p1.m_x <= p2.m_x)
            {
                m_left = p1.m_x;
                m_right = p2.m_x;
            }
            else
            {
                m_left = p2.m_x;
                m_right = p1.m_x;
            }
            
            if (p1.m_y <= p2.m_y)
            {
                m_top = p1.m_y;
                m_bottom = p2.m_y;
            }
            else
            {
                m_top = p2.m_y;
                m_bottom = p1.m_y;
            }
            
            m_cx = (int)((m_left + m_right) / 2);
            m_cy = (int)((m_top + m_bottom) / 2);
            
            if (m_top != m_bottom)
                m_width = m_right - m_left + 1;
            
            if (m_left != m_right)
                m_height = m_bottom - m_top + 1;
            
            m_area = m_width * m_height;
        }
        
        
        virtual ~CMyRect() {}
        
        
        CMyRect(const CMyRect& rect)
        : CMySize(rect.m_width, rect.m_height)
        , m_top(rect.m_top)
        , m_bottom(rect.m_bottom)
        , m_left(rect.m_left)
        , m_right(rect.m_right)
        , m_cx(rect.m_cx)
        , m_cy(rect.m_cy)
        {// copy constructor
        }
        
        
        CMyRect& operator=(const CMyRect& rect)
        {
            // コピー代入演算子
            
            // 自己代入のチェック
            if (this != &rect)
            {
                m_top = rect.m_top;
                m_bottom = rect.m_bottom;
                m_left = rect.m_left;
                m_right = rect.m_right;
                m_cx = rect.m_cx;
                m_cy = rect.m_cy;
                
                m_width = rect.m_width;
                m_height = rect.m_height;
                m_area = rect.m_area;
            }
            return *this;
        }
        
        
        virtual inline size_t Area() const { return GetArea(); }
        
    };
}


#endif /* mat_util_h */
