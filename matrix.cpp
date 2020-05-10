#include "matrix.h"

#include <string>

matrix::matrix() :cols(0), rows(0), data(nullptr) { ; }

matrix::~matrix()
{
	cols = rows = 0;
	delete[] data;
}

matrix::matrix(const matrix& copy):cols(copy.cols),rows(copy.rows)
{
	data = new float[cols * rows];
	memcpy(data, copy.data, cols * rows * sizeof(float));
}

matrix::matrix(int _cols, int _rows, const float* _data):cols(_cols),rows(_rows),data(nullptr)
{
	data = new float[cols * rows];
	memcpy(data, _data, cols * rows * sizeof(float));
}

matrix& matrix::operator=(const matrix& R)
{
	//i wish this worked
	//delete this;
	//matrix(R);
	cols = R.cols;
	rows = R.rows;
	delete[] data;
	data = new float[cols * rows];
	memcpy(data, R.data, cols * rows * sizeof(float));
	return *this;
}

matrix matrix::operator-(void) const
{
	matrix M(*this);
	for (int i = 0; i < cols * rows; ++i)
		M.data[i] *= -1;
	return M;
}

matrix& matrix::operator+=(const matrix& R)
{
	bool e_cols = cols != R.cols;
	bool e_rows = rows != R.rows;
	if (e_cols || e_rows)
	{
		std::string e_message("Domain Mismatch! ");
		e_message += "Cols[ L:" + std::to_string(cols) + ", R:" + std::to_string(R.cols) + "] ";
		e_message += "Rows[ L:" + std::to_string(rows) + ", R:" + std::to_string(R.rows) + "]\n";
		throw std::length_error(e_message);
		return *this;
	}
	
	for (int i = 0; i < cols * rows; ++i)
		data[i] += R.data[i];

	return *this;
}

matrix& matrix::operator-=(const matrix& R)
{
	return *this += -R;
}

matrix matrix::operator+(const matrix& R) const
{
	return matrix(*this) += R;
}

matrix matrix::operator-(const matrix& R) const
{
	return matrix(*this) -= R;
}
