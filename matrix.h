#pragma once

#include <Windows.h>
#include <stdexcept>

class matrix
{
	int cols, rows;
	float* data;
	
public:
	matrix();
	~matrix();
	matrix(const matrix& copy);
	matrix(int cols, int rows, const float* data);

	//unary operators
	matrix& operator= (const matrix& R);
	matrix  operator- (void) const;
	matrix& operator+=(const matrix& R);
	matrix& operator-=(const matrix& R);

	//binary operators
	matrix  operator+ (const matrix& R) const;
	matrix  operator- (const matrix& R) const;
};

