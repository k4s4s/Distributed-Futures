#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/move/move.hpp>
#include <boost/format.hpp>
#include <boost/serialization/shared_ptr.hpp>

template <class T>
class Matrix
{
  public:
  
BOOST_COPYABLE_AND_MOVABLE(Matrix)    
    
friend class boost::serialization::access;

template <typename Archive>
void serialize(Archive & ar, unsigned)
  { 
    ar & height;
    ar & width;
    ar & data;
  }
  
  std::size_t height;
  std::size_t width;
  std::vector<T> data;  
      
  Matrix(std::size_t h=0, std::size_t w=0, T const & c=T())
  :height(h),width(w),data(w*h,c)
  {}
  
  Matrix(size_t h, size_t w, std::vector<T> &d)
  :height(h),width(w),data(d)
  {}
  
	~Matrix() {
		std::cout << "Deleting matrix!" << std::endl;
	}

  T& operator()(std::size_t i, std::size_t j)
  { return data[i+j*height]; }
  
  T const & operator()(std::size_t i, std::size_t j)const
  { return data[i+j*height]; }  
  
  Matrix(const Matrix& p) // Copy constructor (as usual)
  : height(p.height),width(p.width),data(p.data)
  {
    //std::cout<<"Copy constructor"<<std::endl;
  }

  Matrix& operator=(BOOST_COPY_ASSIGN_REF(Matrix) p) // Copy assignment
   {
      if (this != &p){
      height = p.height;
      width = p.width;
      data = p.data;	
      }
      //std::cout<<"Copy assignment"<<std::endl;
      return *this;
   }

   //Move semantics...
   Matrix(BOOST_RV_REF(Matrix) p) //Move constructor
   {
     height = p.height;
     width = p.width;
     data = p.data;
   }

   Matrix& operator=(BOOST_RV_REF(Matrix) p) //Move assignment
   {
      if (this != &p){
      height = p.height;
      width = p.width;
      data = p.data;
      }
      return *this;
   }
  
};

#endif // MATRIX_H
