/*
-----------------------------------------------------------------------------

Copyright (c) 2010 Nigel Atkinson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include <exception>

/*
 * ward_ptr
 *
 * Similar to a weak reference, holds a pointer to a object.  Does not control
 * the life time of the object, but can be marked 'invalid', afterwhich any
 * attempt to dereference results in an exception.
 *
 */

template<class T>
class ward_ptr
{
private:
    T** ptr;
    int *ref_count;

public:
    ward_ptr() : ptr(NULL)
    {
    }
    ward_ptr( const ward_ptr<T>& p )
    {
        ptr = p.ptr;
        ref_count = p.ref_count;
        (*ref_count)++;
    }
    ward_ptr( T* p )
    {
        ptr = new T*;
        ref_count = new int;
        *ptr = p;
        *ref_count = 1;
    }
    T* get() const
    {
        if( ! ptr || ! *ptr )
            throw std::logic_error( "ward_ptr: exception - instance pointer not set." );
        return *ptr;
    }
    T* operator->() const
    {
        if( ! ptr || ! *ptr )
            throw std::logic_error( "ward_ptr: exception - instance pointer not set." );
        return *ptr;
    }
    operator T*() const
    {
        if( ! ptr || ! *ptr )
            throw std::logic_error( "ward_ptr: exception - instance pointer not set." );
        return *ptr;
    }
    void invalidate()
    {
        *ptr = NULL;
    }
    virtual ~ward_ptr()
    {
        (*ref_count)--;
        if( *ref_count == 0 )
        {
            delete ref_count;
            delete ptr;
        }
    }
};

template<class T>
T* get_pointer( ward_ptr<T> p )
{
    return p.get();
}
