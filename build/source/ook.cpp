/*
    Ook
    
    Copyright (c) 2010 Robin Southern
                                                                                  
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
    
*/


#ifndef OOK_H
#define OOK_H

#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <iostream>

namespace Gorilla
{
namespace ook
{
 
 inline size_t find_first_of_escaped(const std::string& str, char search, bool ignore_escapes)
 {
  char c = 0, l = 0;
  for (size_t i=0;i < str.length();i++)
  {
   c = str[i];
   if (i != 0)
    l = str[i-1];
   if (c == search)
   {
    if (ignore_escapes)
    {
     if (l == '\\')
      continue;
     return i;
    }
    else
    {
     return i;
    }
   }
  }
  return std::string::npos;
 }

 inline void remove_all_except_escaped(std::string& str, char search)
 {
  size_t c = 0;
  while((c = find_first_of_escaped(str, '"', true)) != std::string::npos)
   str.erase(c, 1);
 }

 inline void escape_string(std::string& str)
 {
  std::stringstream s;
  char c = 0;
  for (size_t i=0; i < str.length(); i++)
  {
   c = str[i];
   if (c == '"')
    s << "\\";
   s << c;
  }
  str.assign(s.str());
 }
 
 inline void unescape_string(std::string& str)
 {
  std::stringstream s;
  char c = 0;
  for (size_t i=0; i < str.length(); i++)
  {
   c = str[i];
   if (c == '\\')
    continue;
   s << c;
  }
  str.assign(s.str());
 }

 template<typename T> inline T from_string(const std::string& value)
 {
  std::istringstream s(value);
  T ret;
  s >> ret;
  return ret;
 }

 template<typename T> inline std::string to_string(const T& value)
 {
  std::ostringstream s;
  s << value;
  return s.str();
 }
 
 inline std::string to_string(const float& value)
 {
  std::ostringstream s;
  float floored = floor(value);
  if (floored == value)
  {
   s << floored << ".0";
   return s.str();
  }
  s.precision(8);
  s << value;
  return s.str();
 }

 inline std::string to_string(const bool& value)
 {
  std::ostringstream s;
  if (value)
   s << "yes";
  else
   s << "no";
  return s.str();
 }
 
 inline bool from_string(const std::string& value)
 {
  if (value.length() == 0)
   return false;
  size_t first_character = value.find_first_not_of(" \t\n\r");
  if (first_character == std::string::npos)
   return false;
  char c = value[first_character];
  if (c == 't' || c == 'T' || c == '1' || c == 'y' || c == 'Y')
   return true;
  return false;
 }

 template<typename T> inline std::string to_string(T* begin, T* end)
 {
  std::ostringstream s;
  for (T* it = begin; it != end; it++)
  {
   s << *it;
   if (it != end - 1)
    s << ", ";
  }
  return s.str();
 }
 
 /*! class. Value
     desc.
         Container of single or multiple bool, float, int or string values, under one key.
 */
 class Value
 {
    
   public:
    
    enum Type
    {
     type_null = 0,
     type_int = 1,
     type_float = 2,
     type_bool = 3,
     type_string = 4
    };
    
    /*! constructor.
        desc.
            Default constructor.
        note.
            The value is automatically set to null.
    */
    Value()
    {
     _value._int = 0;
     _type = type_null;
     _length = 0;
    }
    
    /*! destructor.
        desc.
            Value destructor.
        note.
            All allocated memory is automatically freed.
    */
   ~Value()
    {
     _free();
    }
    
    /*! function. unserialise
        desc.
            From a string, parse the contents into an int(s), float(s), bool(s) or a string.
        note.
            String must be trimmed before use.
    */
    void unserialise(const std::string& value)
    {
     _free();
      
     if (value.length() == 0)
      return; // null
     
     char c = tolower(value[0]);

     // Is an array?
     if (c == '[' && value.length() > 2)
     {
      std::string safe_value(value.substr(1, value.length()-2));
      //std::replace( safe_value.begin(), safe_value.end(), '\n', ' ' );
      size_t start = 0, pos = 0;
      c = safe_value[0];
      // Is bools?
      if (c == 'y' || c == 'n' || c == 't' || c == 'f')
      {
          do
          {
           pos = safe_value.find_first_of(',', start);
           if (pos == start)
            start = pos + 1;
           else if (pos == std::string::npos)
           {
            appendBool(from_string(safe_value.substr(start)));
            break;
           }
           else
           {
            appendBool(from_string(safe_value.substr(start, pos - start)));
            start = pos + 1;
           }
           start = safe_value.find_first_not_of(',', start);
          } while (pos != std::string::npos);
      }
      // Is floats?
      else if (safe_value.find_first_of('.') != std::string::npos)
      {
          do
          {
            pos = safe_value.find_first_of(',', start);
            if (pos == start)
             start = pos + 1;
            else if (pos == std::string::npos)
            {
             appendFloat(from_string<float>(safe_value.substr(start)));
             break;
            }
            else
            {
             appendFloat(from_string<float>(safe_value.substr(start, pos - start)));
             start = pos + 1;
            }
            start = safe_value.find_first_not_of(',', start);
           } while (pos != std::string::npos);
      }
      // must ints
      else
      {
          do
          {
            pos = safe_value.find_first_of(',', start);
            if (pos == start)
             start = pos + 1;
            else if (pos == std::string::npos)
            {
             appendInt(from_string<int>(safe_value.substr(start)));
             break;
            }
            else
            {
             appendInt(from_string<int>(safe_value.substr(start, pos - start)));
             start = pos + 1;
            }
            start = safe_value.find_first_not_of(',', start);
           } while (pos != std::string::npos);
      }
     }
     // Is a string?
     else if (c == '\"' && value.length() > 2)
     {
      std::string safe_value(value.substr(1, value.length()-2));
      remove_all_except_escaped(safe_value, '\"');
      unescape_string(safe_value);
      fromString(safe_value);
     }
     // Scalar (float, int, bool)
     else
     {
      // Is a bool?
      if (c == 'y' || c == 'n' || c == 't' || c == 'f')
      {
       if (c == 'y' || c == 't')
        fromBool(true);
       else
        fromBool(false);
       return;
      }
      // Is it a float?
      else if (value.find_first_of('.') != std::string::npos)
      {
       fromFloat(from_string<float>(value));
       return;
      }
      // must be an int
      else
      {
       fromInt(from_string<int>(value));
       return;
      }
     }
    }
    
    /*! function. serialise
        desc.
            Save the contents into a human readable string.
        note.
            indent is the number of characters in the line that the contents is intended to be, so
            additional lines can match up.
    */
    void serialise(std::ostream& stream, size_t indent)
    {
     if (_type == type_null)
      stream << "null";
     else if (_length > 1)
     {
      if (_type == type_string)
      {
       std::string str_value(_value._str);
       escape_string(str_value);
       stream << "\"";
       for (size_t i=0;i < str_value.length();i++)
       {
        if (str_value[i] == '\n' && i != str_value.length() - 1)
        {
         stream << "\"\n";
         for (size_t i=0;i < indent;i++)
          stream << " ";
         stream << "\"";
         continue;
        }
        stream << str_value[i];
       }
       stream << "\"";
       return;
      }
      stream << "[";
      if (_type == type_int)
      {
       for(size_t i=0; i < _length;i++)
       {
        stream << _value._int[i];
        if (i != _length - 1)
         stream << ", ";
       }
      }
      else if (_type == type_float)
      {
       for(size_t i=0; i < _length;i++)
       {
        stream << to_string(_value._float[i]);
        if (i != _length - 1)
         stream << ", ";
       }
      }
      else if (_type == type_bool)
      {
       for(size_t i=0; i < _length;i++)
       {
        stream << (_value._bool[i] ? "yes" : "no");
        if (i != _length - 1)
         stream << ", ";
       }
      }
      stream << "]";
     }
     else
     {
      if (_type == type_int)
       stream << to_string(_value._int[0]);
      else if (_type == type_float)
       stream << to_string(_value._float[0]);
      else if (_type == type_bool)
       stream << (_value._bool[0] ? "yes" : "no");
     }
    }
    
    /*! function. operator= (int)
        desc.
            Assign an integer into this value.
    */
    void operator=(int val)
    {
     fromInt(val);
    }
    
    /*! function. operator+= (int)
        desc.
            Append an integer.
        note.
            If the value is anything but an array of integers; then the old contents is cleared, and
            this new value becomes the first value in the array.
    */
    void operator+=(int val)
    {
     appendInt(val);
    }
    
    /*! function. operator= (float)
        desc.
            Assign an float into this value.
    */
    void operator=(float val)
    {
     fromFloat(val);
    }
    
    /*! function. operator+= (float)
        desc.
            Append an float.
        note.
            If the value is anything but an array of floats; then the old contents is cleared, and
            this new value becomes the first value in the array.
    */
    void operator+=(float val)
    {
     appendFloat(val);
    }
    
    /*! function. operator= (bool)
        desc.
            Assign an bool into this value.
    */
    void operator=(bool val)
    {
     fromBool(val);
    }
    
    /*! function. operator+= (bool)
        desc.
            Append an bool.
        note.
            If the value is anything but an array of booleans; then the old contents is cleared, and
            this new value becomes the first value in the array.
    */
    void operator+=(bool val)
    {
     appendBool(val);
    }
    
    /*! function. operator= (string)
        desc.
            Assign an string into this value.
    */
    void operator=(const char* val)
    {
     fromString(val);
    }
    
    /*! function. operator= (string)
        desc.
            Assign an string into this value.
    */
    void operator=(const std::string& val)
    {
     fromString(val);
    }
    
    /*! function. operator int
        desc.
            Get the value as a integer, or if it's not an integer get the best value.
    */
    operator int()
    {
     return asInt();
    }
    
    /*! function. atInt
        desc.
            From an array of integers; fetch the one in the index, or if it's not an integer array get the best value.
    */
    int atInt(size_t index)
    {
     if (index < _length)
     {
      if (_type == type_int)
       return _value._int[index];
      else if (_type == type_float)
       return (int) _value._float[index];
      else if (_type == type_bool)
       return (_value._bool[index] ? 1 : 0);
      else if (_type == type_string)
       return _value._str[index];
     }
     return 0;
    }
    
    /*! function. operator float
        desc.
            Get the value as a float, or if it's not an float get the best value.
    */
    operator float()
    {
     return asFloat();
    }
    
    /*! function. atInt
        desc.
            From an array of floats; fetch the one in the index, or if it's not an float array get the best value.
    */
    float atFloat(size_t index)
    {
     if (index < _length)
     {
      if (_type == type_int)
       return (float) _value._int[index];
      else if (_type == type_float)
       return _value._float[index];
      else if (_type == type_bool)
       return (_value._bool[index] ? 1.0f : 0.0f);
      else if (_type == type_string)
       return (float) _value._str[index];
     }
     return 0;
    }
    
    /*! function. operator bool
        desc.
            Get the value as a bool, or if it's not an bool get the best value.
    */
    operator bool()
    {
     return asBool();
    }
    
    /*! function. atBool
        desc.
            From an array of booleans; fetch the one in the index, or if it's not an boolean array get the best value.
    */
    bool atBool(size_t index)
    {
     if (index < _length)
     {
      if (_type == type_int)
       return _value._int[index] != 0;
      else if (_type == type_float)
       return _value._float[index] != 0.0f;
      else if (_type == type_bool)
       return _value._bool[index];
      else if (_type == type_string)
       return (_value._str[index] == 't' ||
               _value._str[index] == 'T' ||
               _value._str[index] == 'y' ||
               _value._str[index] == 'Y');
     }
     return false;
    }
    
    /*! function. operator std::String
        desc.
            Get the value as a string, or if it's not a string get the best value.
    */
    operator std::string() const
    {
     return asString();
    }
    
    /*! function. atBool
        desc.
            From an string; get a character in the string, or if it's not an string get the best value.
    */
    char atString(size_t index)
    {
     if (index < _length)
     {
      if (_type == type_int)
       return (char) _value._int[index];
      else if (_type == type_float)
       return (char) _value._float[index];
      else if (_type == type_bool)
       return _value._bool[index] ? 't' : 'f';
      else if (_type == type_string)
       return (_value._str[index]);
     }
     return false;
    }
    
    /*! function. asInt
        desc.
            Get the value as a integer, or if it's not an integer get the best value.
    */
    int asInt() const
    {
     if (_type == type_int)
      return _value._int[0];
     else if (_type == type_float)
      return (int) _value._float[0];
     else if (_type == type_bool)
      return (_value._bool[0] ? 1 : 0);
     else if (_type == type_string)
      return from_string<int>(std::string(_value._str));
     return 0;
    }
    
    /*! function. asFloat
        desc.
            Get the value as a float, or if it's not an float get the best value.
    */
    float asFloat() const
    {
     if (_type == type_int)
      return (float) _value._int[0];
     else if (_type == type_float)
      return _value._float[0];
     else if (_type == type_bool)
      return (_value._bool[0] ? 1.0f : 0.0f);
     else if (_type == type_string)
      return from_string<float>(std::string(_value._str));
     return 0;
    }
    
    /*! function. asBool
        desc.
            Get the value as a boolean, or if it's not an boolean get the best value.
    */
    bool asBool() const
    {
     if (_type == type_int)
      return _value._int[0] != 0;
     else if (_type == type_float)
      return _value._float[0] != 0.0f;
     else if (_type == type_bool)
      return _value._bool[0];
     else if (_type == type_string)
      return from_string(std::string(_value._str));
     return 0;
    }
    
    /*! function. asString
        desc.
            Get the value as a string, or if it's not an string get the best value.
    */
    std::string asString() const
    {
     if (_type == type_int)
      return to_string(_value._int[0]);
     else if (_type == type_float)
      return to_string(_value._float[0]);
     else if (_type == type_bool)
      return to_string(_value._bool[0]);
     else if (_type == type_string)
      return std::string(_value._str);
     return std::string();
    }

    /*! function. fromNull
        desc.
            Become a null value.
    */
    void fromNull()
    {
     _free();
    }
    
    /*! function. fromInt
        desc.
            Become an integer value.
    */
    void fromInt(int val)
    {
     _free();
     _value._int = (int*) malloc(sizeof(int));
     _value._int[0] = val;
     _type = type_int;
     _length = 1;
     _max_length = 1;
    }
    
    /*! function. fromInt2
        desc.
            Using a class with x, y member values. Append those two into this value.
    */
    template<typename T> void fromInts2(const T& val)
    {
     _free();
     appendInt(val.x);
     appendInt(val.y);
    }
    
    /*! function. fromInt3
        desc.
            Using a class with x, y, z member values. Append those three into this value.
    */
    template<typename T> void fromInts3(const T& val)
    {
     _free();
     appendInt(val.x);
     appendInt(val.y);
     appendInt(val.z);
    }
    
    /*! function. fromInt4
        desc.
            Using a class with w, x, y, z member values. Append those four into this value.
    */
    template<typename T> void fromInts4(const T& val)
    {
     _free();
     appendInt(val.w);
     appendInt(val.x);
     appendInt(val.y);
     appendInt(val.z);
    }
    
    /*! function. appendInt
        desc.
            Append an integer.
        note.
            If the value is anything but an array of integers; then the old contents is cleared, and
            this new value becomes the first value in the array.
    */
    void appendInt(int val)
    {
     if (_type != type_int)
     {
      _free();
      _value._int = (int*) malloc(sizeof(int) * 2);
      _type = type_int;
      _max_length = 2;
     }
     
     _value._int[_length] = val;
     _length++;
     
     if (_length == _max_length)
     {
      int* new_int = (int*) malloc(sizeof(int) * _max_length * 2);
      _max_length *= 2;

      std::copy(_value._int, _value._int + _length, new_int);

      free(_value._int);
      _value._int = new_int;
     }
     
    }
    
    /*! function. fromFloat
        desc.
            Assign an float into this value.
    */
    void fromFloat(float val)
    {
     _free();
     _value._float = (float*) malloc(sizeof(float));
     _value._float[0] = val;
     _type = type_float;
     _length = 1;
     _max_length = 1;
    }
    
    /*! function. fromFloats2
        desc.
            Using a class with x, y member values. Append those two into this value.
    */
    template<typename T> void fromFloats2(const T& val)
    {
     appendFloat(val.x);
     appendFloat(val.y);
    }
    
    /*! function. fromFloats3
        desc.
            Using a class with x, y, z member values. Append those three into this value.
    */
    template<typename T> void fromFloats3(const T& val)
    {
     appendFloat(val.x);
     appendFloat(val.y);
     appendFloat(val.z);
    }
    
    /*! function. fromFloats4
        desc.
            Using a class with w, x, y, z member values. Append those four into this value.
    */
    template<typename T> void fromFloats4(const T& val)
    {
     appendFloat(val.w);
     appendFloat(val.x);
     appendFloat(val.y);
     appendFloat(val.z);
    }
    
    /*! function. operator+= (float)
        desc.
            Append an float.
        note.
            If the value is anything but an array of floats; then the old contents is cleared, and
            this new value becomes the first value in the array.
    */
    void appendFloat(float val)
    {
     if (_type != type_float)
     {
      _free();
      _value._float = (float*) malloc(sizeof(float) * 8);
      _type = type_float;
      _max_length = 8;
     }
     
     _value._float[_length] = val;
     _length++;
     
     if (_length == _max_length)
     {
      float* new_float = (float*) malloc(sizeof(float) * _max_length * 2);
      _max_length *= 2;

      std::copy(_value._float, _value._float + _length, new_float);

      
     }
     
    }
    
    /*! function. fromBool
        desc.
            Assign an boolean into this value.
    */
    void fromBool(bool val)
    {
     _free();
     _value._bool = (bool*) malloc(sizeof(bool));
     _value._bool[0] = val;
     _type = type_bool;
     _length = 1;
     _max_length = 1;
    }
    
    /*! function. appendBool
        desc.
            Append an boolean.
        note.
            If the value is anything but an array of boolean; then the old contents is cleared, and
            this new value becomes the first value in the array.
    */
    void appendBool(bool val)
    {
     if (_type != type_bool)
     {
      _free();
      _value._bool = (bool*) malloc(sizeof(bool) * 8);
      _type = type_bool;
      _max_length = 8;
     }
     
     _value._bool[_length] = val;
     _length++;
     
     if (_length == _max_length)
     {
      bool* new_bool = (bool*) malloc(sizeof(bool) * _max_length * 2);
      _max_length *= 2;

      std::copy(_value._bool, _value._bool + _length, new_bool);


      free(_value._bool);
      _value._bool = new_bool;
     }
     
    }
    
    /*! function. fromString
        desc.
            Assign an string into this value.
    */
    void fromString(const std::string& val)
    {
     _free();
     _value._str = (char*) malloc(val.length() + 1);


     std::copy(val.begin(), val.end(), _value._str);


     _value._str[val.length()] = 0;
     _type = type_string;
     _length = val.length();
    }
    
    /*! function. isNull
        desc.
            Is this value null?
    */
    bool isNull() const
    {
     return _type == type_null;
    }
    
    /*! function. isInt
        desc.
            Is this value int?
    */
    bool isInt() const
    {
     return _type == type_int;
    }
    
    /*! function. isFloat
        desc.
            Is this value float?
    */
    bool isFloat() const
    {
     return _type == type_float;
    }
    
    /*! function. isString
        desc.
            Is this value string?
    */
    bool isString() const
    {
     return _type == type_string;
    }
    
    /*! function. length
        desc.
            Number of elements that this value represents.
    */
    size_t length() const
    {
     return _length;
    }
    
    /*! function. type
        desc.
            Type of value (int, bool, float or string)
    */
    Type type() const
    {
     return _type;
    }

   protected:
    
    void _free()
    {
     
     if (_type == type_int)
     {
      free(_value._int);
     }
     else if (_type == type_float)
     {
      free(_value._float);
     }
     else if (_type == type_bool)
     {
      free(_value._bool);
     }
     else if (_type == type_string)
     {
      free(_value._str);
     }
     
     _value._int = 0;
     _type = type_null;
     _length = 0;
     _max_length = 0;
    }
    
    union cpp
    {
     int*   _int;
     float*  _float;
     bool*  _bool;
     char*  _str;   // must be null terminated, but _length must be string length - 1.
    }       _value;
    Type    _type;
    size_t  _length, _max_length;
   
   
  };

 class Set
 {
   
  public:
   
   Set()
   {
   }
   
  ~Set()
   {
    for (std::map<std::string, Value*>::iterator it = _value.begin(); it != _value.end(); it++)
     delete (*it).second;
   }
   
   Value& operator[](const std::string& key)
   {
    return *value(key);
   }
   
   Value* value(const std::string& key)
   {
    std::map<std::string, Value*>::iterator it = _value.find(key);
    if (it == _value.end())
    {
     Value* value = new Value();
     _value.insert(std::pair<std::string, Value*>(key, value));
     return value;
    }
    return (*it).second;
   }

   void serialise(std::ostream& stream)
   {
    for (std::map<std::string, Value*>::iterator it = _value.begin(); it != _value.end(); it++)
    {
     stream << "   " << (*it).first << " : ";
     (*it).second->serialise(stream, 3 + (*it).first.length() + 3);
     stream << "\n";
    }
   }

  protected:
   
  std::map<std::string, Value*> _value;
   
 };
 
 class Sets
 {
   
  public:
   
   Sets() {}
   
   Sets(std::map<std::string, Set*>::iterator begin, std::map<std::string, Set*>::iterator end)
   : _current(begin), _end(end) {_current_key = (*_current).first;}
   
   operator Set&()
   {
    return *(*_current).second;
   }
   
   bool hasMore() const
   {
    return _current != _end;
   }

   Set& operator++(int)
   {
    Set* cur = (*_current).second;
    _current_key = (*_current).first;
    _current++;
    return *cur;
   }

   std::string key()
   {
    return _current_key;
   }

  protected:
   
   std::map<std::string, Set*>::iterator _current, _end;
   std::string                           _current_key;
 };

 class File
 {
   
  public:
   
   File()
   {
   
   }
  
   File(const std::string& file)
   {
    
   }
   
  ~File()
   {
    for (std::map<std::string, Set*>::iterator it = _sets.begin(); it != _sets.end(); it++)
     delete (*it).second;
   }
   
   Sets all()
   {
    return Sets(_sets.begin(), _sets.end());
   }

   Set& add(const std::string& name)
   {
    Set* set = new Set();
    _sets.insert(std::pair<std::string, Set*>(name, set));
    return *set;
   }
   
   bool exists(const std::string& name)
   {
    return _sets.find(name) != _sets.end();
   }

   Set& at(const std::string& name)
   {
    std::map<std::string, Set*>::iterator it = _sets.find(name);
    if (it == _sets.end())
     return add(name);
    return *(*it).second;
   }
   
   Set& operator[](const std::string& type)
   {
    return at(type);
   }
   
   Set& unnamed()
   {
    static size_t next_anon_id = 0;
    return add("~!" + to_string(next_anon_id++));
   }
   
   void unserialise(std::istream& stream)
   {
    static const std::string whitespace(" \t\r");
    static const std::string valuechars(",.[]\"");
    Set* last_set = 0;
    Value* last_value = 0;
    size_t search_char = 0, search_char2 = 0, line_indent = 0;
    std::string line, value_line, key_line;
    size_t anon_key_id = 0;
    while(getline(stream, line))
    {
     anon_key_id++;
     // Erase after (and including) the first comment character; #
     search_char = line.find_first_of('#');
     if (search_char != std::string::npos)
      line.erase(line.find_first_of('#'));
     
     // Trim line.
     line.erase(line.find_last_not_of(whitespace)+1);
     line.erase(0, line_indent = line.find_first_not_of(whitespace));
     
     // Skip over blank lines.
     if (line.length() == 0)
      continue;
     
     // Push any old value.
     if (line[0] == '-' || line.find_first_of(':') != std::string::npos)
     {
      if (key_line.length())
      {
       last_value->unserialise(value_line);
       key_line.clear();
       value_line.clear();
      }
     }

     // New Set
     if (line[0] == '-' && line_indent == 0)
     {
      
      key_line.clear();
      value_line.clear();
      
      // Cut of '-'
      value_line = line.substr(1);
      
      // Trim all whitespace.
      value_line.erase(value_line.find_last_not_of(whitespace)+1);
      value_line.erase(0, value_line.find_first_not_of(whitespace));
      
      if (value_line.length() == 0)
       value_line = "~" + to_string(anon_key_id);
      
      last_set = new Set();
      _sets.insert(std::pair<std::string, Set*>(value_line, last_set));
      value_line.clear();
      continue;
     }
     else if (line_indent > 0)
     {
      
      search_char = line.find_first_of(':');
      
      // Multi-line.
      if (search_char == std::string::npos)
      {
       // Strings may span multi-lines, remove all extra " " characters and append a \n.
       //remove_all_except_escaped(line, '\"');
       value_line.append("\n");
       value_line.append(line);
       continue;
      }
      else
      {
       key_line.assign(line.substr(0, search_char));
       key_line.erase(key_line.find_last_not_of(whitespace)+1);
       key_line.erase(0, key_line.find_first_not_of(whitespace));
       value_line.assign(line.substr(search_char+1));
       value_line.erase(value_line.find_last_not_of(whitespace)+1);
       value_line.erase(0, value_line.find_first_not_of(whitespace));
       last_value = last_set->value(key_line);
      }
     }
     
    }
    
    if (key_line.length())
    {
      last_value->unserialise(value_line);
    }
   }
   
   void serialise(std::ostream& stream)
   {
    for (std::map<std::string, Set*>::iterator it = _sets.begin(); it != _sets.end(); it++)
    {
     stream << "-";
     if ((*it).first[0] != '~')
      stream << " " << (*it).first;
     stream << '\n';
     (*it).second->serialise(stream);
    }
   }
   
  protected:
   
   std::map<std::string, Set*> _sets;
 };

}
}

#endif
